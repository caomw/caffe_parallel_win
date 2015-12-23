echo caffe.pb.h is being generated
"protoc.exe" -I="../src/caffe/proto" --cpp_out="../src/caffe/proto" "../src/caffe/proto/caffe.proto"
cp ../src/caffe/proto/caffe.pb.h ../include/caffe/proto/caffe.pb.h
