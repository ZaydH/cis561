rm -rf bin Release &> /dev/null
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ..
