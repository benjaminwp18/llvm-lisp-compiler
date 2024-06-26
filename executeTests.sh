# This will run our compiler on a C file, export its LLVM ASM to a .ll file and output how it went to the log.
# The LLVM ASM is than ran and its outut sent to the execution log.
function runTest {
    ./run.sh -i tests/$1.lisp -fAsm -o tests/$1.ll &> tests/$1.log
    lli tests/$1.ll &> tests/$1Execution.log
}

. cleanTests.sh

# Run all the C file tests.
for file in "tests"/*.lisp; do
    file=$(basename $file .lisp)
    echo "Testing: $file"
    runTest $file
done