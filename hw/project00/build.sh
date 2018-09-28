rm -rf bin Release cmake-build-debug &> /dev/null
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ..
