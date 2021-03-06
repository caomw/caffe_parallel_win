#ifdef USE_LMDB
#include "caffe/util/db_lmdb.hpp"

#include <sys/stat.h>

#include <string>
#ifdef _MSC_VER
#include <direct.h>
//#define mkdir _mkdir
#endif

namespace caffe { namespace db {

// const size_t LMDB_MAP_SIZE = 1099511627776;  // 1 TB
// const size_t LMDB_MAP_SIZE = 214748364800;  // 200 GB
// const size_t LMDB_MAP_SIZE = 107374182400;  // 100 GB
// const size_t LMDB_MAP_SIZE = 150323855360;  // 140 GB TRAIN
 const size_t LMDB_MAP_SIZE = 21474836480;  // 20 GB TEST
// const size_t LMDB_MAP_SIZE = 10737418240;  // 10 GB VAL


void LMDB::Open(const string& source, Mode mode) {
  MDB_CHECK(mdb_env_create(&mdb_env_));
  MDB_CHECK(mdb_env_set_mapsize(mdb_env_, LMDB_MAP_SIZE));
  if (mode == NEW) {
    CHECK_EQ(_mkdir(source.c_str()), 0) << "mkdir " << source << "failed";
  }
  int flags = 0;
  if (mode == READ) {
    flags = MDB_RDONLY | MDB_NOTLS;
  }
  MDB_CHECK(mdb_env_open(mdb_env_, source.c_str(), flags, 0664));
  LOG(INFO) << "Opened lmdb " << source;
}

LMDBCursor* LMDB::NewCursor() {
  MDB_txn* mdb_txn;
  MDB_cursor* mdb_cursor;
  MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, MDB_RDONLY, &mdb_txn));
  MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
  MDB_CHECK(mdb_cursor_open(mdb_txn, mdb_dbi_, &mdb_cursor));
  return new LMDBCursor(mdb_txn, mdb_cursor);
}

LMDBTransaction* LMDB::NewTransaction() {
  MDB_txn* mdb_txn;
  MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, 0, &mdb_txn));
  MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
  return new LMDBTransaction(&mdb_dbi_, mdb_txn);
}

void LMDBTransaction::Put(const string& key, const string& value) {
  MDB_val mdb_key, mdb_value;
  mdb_key.mv_data = const_cast<char*>(key.data());
  mdb_key.mv_size = key.size();
  mdb_value.mv_data = const_cast<char*>(value.data());
  mdb_value.mv_size = value.size();
  MDB_CHECK(mdb_put(mdb_txn_, *mdb_dbi_, &mdb_key, &mdb_value, 0));
}

}  // namespace db
}  // namespace caffe
#endif  // USE_LMDB
