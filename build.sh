echo "Building..."
mkdir -p build
cd build
cmake ..
if which bear
then
	bear -- make -j8
else
	make -j8
fi
cd ..
