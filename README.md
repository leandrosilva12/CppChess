# CppChess
A chess game made with C++.


## Google Test

### Installation

Install these packages:
```
sudo apt-get install libgtest-dev
sudo apt-get install cmake 
```

Create the necessary library files:
```
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make

cd lib
# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
sudo cp *.a /usr/lib
```

Create a 
```
cmake_minimum_required(VERSION 2.6)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests tests.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
```

### Compilation

After having everything set up, run the following commands on the test root folder to compile and run the tests:

```
# creates folder CMakeFiles
cmake CMakeLists.txt

make
./runTests
```

For recompiling, `cmake` is no longer needed.
