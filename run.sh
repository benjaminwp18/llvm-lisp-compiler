export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
bash ./build.sh
echo "Running..."
bin/LLVM-Lab $@
