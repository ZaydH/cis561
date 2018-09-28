rm -rf bin Release cmake-build-debug &> /dev/null
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ..

BIN_DIR=bin
BIN_NAME=project00
chmod 777 $BIN_DIR
chmod 777 $BIN_DIR/$BIN_NAME

