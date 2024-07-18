# [Steed Open Source Project](https://github.com/steed-ict/steed) <!-- omit in toc -->


## [Table of Contents](#table-of-contents) <!-- omit in toc -->
- [1 Description](#1-description)
- [2 Pre-requisites](#2-pre-requisites)
  - [2.1 g++](#21-g)
  - [2.2 CMake](#22-cmake)
  - [2.3 Doxygen](#23-doxygen)
  - [2.4 Graphviz](#24-graphviz)
  - [2.5 LZ4](#25-lz4)
  - [2.6 CLI11](#26-cli11)
  - [2.7 Google Test](#27-google-test)
  - [2.8 All in One](#28-all-in-one)
- [3 Source Code](#3-source-code)
  - [3.1 Download](#31-download)
  - [3.2 Directory Structure](#32-directory-structure)
  - [3.3 Documentation](#33-documentation)
- [4 Build](#4-build)
  - [4.1 Build with CMake](#41-build-with-cmake)
  - [4.2 Build with Make](#42-build-with-make)
  - [4.3 Unittest](#43-unittest)
  - [4.4 Install](#44-install)
  - [4.5 Set Environment Variables](#45-set-environment-variables)
- [5 Example](#5-example)
  - [5.1 Build with Steed shared library](#51-build-with-steed-shared-library)
  - [5.2 JSON Example Data](#52-json-example-data)
  - [5.3 Steed Configuration](#53-steed-configuration)
  - [5.4 Run Example](#54-run-example)
    - [5.4.1 Config and init Static](#541-config-and-init-static)
    - [5.4.2 Create Database](#542-create-database)
    - [5.4.3 Create Table](#543-create-table)
    - [5.4.4 Parse Data](#544-parse-data)
    - [5.4.5 Assemble Data](#545-assemble-data)
    - [5.4.6 Drop Table and Database](#546-drop-table-and-database)



## 1 Description
Steed is a columnar storage engine for JSON data. It is designed to be a lightweight, high-performance, and easy-to-use database engine. Steed is written in C++ and we are going to open source the storage engine, including the JSON parser, column assembler, and columnar data storage.


## 2 Pre-requisites
Steed requires the following software to build:
- C++ compiler with C++11 support
- CMake 
- Doxygen 
- LZ4 
- CLI11 
- Google Test 

We have tested Steed on the following platforms:
- Ubuntu 22.04 or later
    - CMake 3.11 or later
    - Doxygen 1.9.1 or later
    - LZ4 1.9.3 or later
    - CLI11 2.3.2 or later
    - Google Test 1.8.0 or later

### 2.1 g++
Steed requires a C++ compiler with C++11 support. We have tested the following compilers:
- g++ 7.5.0 or later

To install g++, use the following command:
```bash
$ sudo apt-get install g++
```

### 2.2 CMake
Steed uses CMake to build. We have tested the following CMake versions:
- CMake 3.11 or later

To install CMake, use the following command:
```bash
$ sudo apt-get install cmake
```
Or you can download the latest version of CMake from [CMake](https://cmake.org/download/).

### 2.3 Doxygen
Steed uses Doxygen to generate documentation. We have tested the following Doxygen versions:
- 1.9.1 or later

To install Doxygen, use the following command:
```bash
$ sudo apt-get install doxygen
```
Or you can download the latest version of Doxygen from [Doxygen](https://www.doxygen.nl/download.html).

### 2.4 Graphviz
Doxygen uses Graphviz to generate graphs. To install Graphviz, use the following command:
```bash
$ sudo apt-get install graphviz
```

### 2.5 LZ4
Steed uses LZ4 to compress and decompress data. We have tested the following LZ4 versions:
- 1.9.3 or later

To install LZ4, use the following command:
```bash
$ sudo apt-get install liblz4-1 liblz4-dev
```

### 2.6 CLI11
Steed uses CLI11 to parse the configuration file. We have tested the following CLI11 versions:
- 2.3.2 or later

We do contain the CLI11 source code in the `src/thirdparty/cli11` directory and the License of CLI11 is in the `src/thirdparty/cli11/LICENSE` file. CMake will build CLI11 automatically.


### 2.7 Google Test
Steed uses Google Test to run unittests. We have tested the following Google Test versions:
- 1.8.0 or later

We also contain the Google Test source code in the `src/thirdparty/googletest` directory and the License of Google Test is in the `src/thirdparty/googletest/LICENSE` file. CMake will build Google Test automatically.

### 2.8 All in One
You can use the following command to install all the pre-requisites:
```bash
$ sudo apt-get install git g++ cmake doxygen graphviz liblz4-1 liblz4-dev
```


## 3 Source Code
### 3.1 Download
Steed is available on Gitlab. You can clone the Steed repository using the following command: 
```bash
$ git clone https://github.com/steed-ict/steed
```
After cloning the Steed repository, you can see the following directory:
```bash
$ ls
steed_open_source
```
And we enter the `steed_open_source` directory and set the `STEED_HOME` environment variable:
```bash
$ cd steed_open_source/
$ export STEED_HOME=`pwd`
```

### 3.2 Directory Structure
We use `tree` command to show the directory structure of Steed. You can install `tree` using the following command:
```bash
$ sudo apt-get install tree
```

The following command shows the directory structure of Steed. Here we use `tree . -L 3` to show the first three levels of the directory structure:
```bash
$ cd $STEED_HOME
$ tree . -L 2
.
├── CMakeLists.txt      # CMake configuration file
├── CMakeModules        # CMake modules
│   └── FindLZ4.cmake   # Find LZ4 module
├── LICENSE             # open source license
├── README.md           # README file
├── conf                # Steed configuration file
│   └── steed.conf
├── doxygen             # Doxygen related files
│   └── Doxyfile        # Doxygen configuration file
├── examples            # Steed example
│   ├── Makefile        # Makefile to build the example
│   ├── json            # example json text records
│   │   └── try.json    
│   └── main.cpp        # example source code
├── includea            # Steed header files
│   └── steed.h
├── src                 # Steed source code
│   ├── CMakeLists.txt  
│   ├── steed           # Steed core code
│   │   ├── CMakeLists.txt
│   │   ├── assemble    # assemble code from columnar to row data
│   │   ├── base        # base source code
│   │   ├── conf        # conf related source code
│   │   ├── parse       # parse json text records to columnar data
│   │   ├── schema      # schema source code
│   │   ├── steed.cpp   # steed main source code
│   │   ├── store       # binary store source code
│   │   ├── unittest    # unittest source code
│   │   └── util        # util source code
│   └── thirdparty      # thirdparty source code
│       ├── CMakeLists.txt
│       ├── cli11       # CLI11 source code
│       └── googletest  # Google Test source code
└── tools
    ├── CMakeLists.txt
    └── Schema.cpp      # schema tool

20 directories, 16 files
```


### 3.3 Documentation
Steed uses Doxygen to generate documentation. You can use the following command to generate documentation:
```bash
$ cd $STEED_HOME/doxygen
$ doxygen Doxyfile
```

After generating documentation, you can open `docs/html/index.html` to view the documentation.

## 4 Build 
### 4.1 Build with CMake
Steed use CMake to build. The following command shows how to build Steed using CMake:
```bash
$ cd $STEED_HOME
$ mkdir build && cd build/ && cmake .. 
```

The following output shows a successful build:
```bash
$ mkdir build && cd build/ && cmake ..
-- The CXX compiler identification is GNU 11.4.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Could NOT find PkgConfig (missing: PKG_CONFIG_EXECUTABLE)
-- CMAKE_MODULE_PATH @ /home/steeduser/steed_open_source/CMakeModules
-- The C compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- STEED download thirdparty @ /home/steeduser/steed_open_source/build/thirdparty
-- steed @ src/steed includes CLI11 lib @ /home/steeduser/steed_open_source/src/thirdparty/cli11/include
-- Could NOT find Python3 (missing: Python3_EXECUTABLE Interpreter)
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Found liblz4:
INFOCMAKE_CURRENT_SOURCE_DIR: /home/steeduser/steed_open_source/src/steed
-- steed Config includes CLI11 lib @ /home/steeduser/steed_open_source/src/thirdparty/cli11/include
-- Found liblz4-dev: /usr/lib/x86_64-linux-gnu/liblz4.so
-- steed includes lz4 lib @ /usr/lib/x86_64-linux-gnu/liblz4.so
-- Configuring done
-- Generating done
-- Build files have been written to: /home/steeduser/steed_open_source/build
```

### 4.2 Build with Make
Then you can use the following command to build Steed:
```bash
$ make all
```

The following output shows a successful build:
```bash
$ make all
[  0%] Built target CLI11
[  1%] Building CXX object src/thirdparty/googletest/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
[  2%] Linking CXX static library ../../../../lib/libgtest.a
[  2%] Built target gtest
[  3%] Building CXX object src/thirdparty/googletest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o
[  4%] Linking CXX static library ../../../../lib/libgmock.a
[  4%] Built target gmock
[  5%] Building CXX object src/thirdparty/googletest/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
[  6%] Linking CXX static library ../../../../lib/libgmock_main.a
[  6%] Built target gmock_main
[  7%] Building CXX object src/thirdparty/googletest/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
[  8%] Linking CXX static library ../../../../lib/libgtest_main.a
[  8%] Built target gtest_main
[  9%] Building CXX object src/steed/conf/CMakeFiles/steed_conf.dir/Config.cpp.o
[ 10%] Linking CXX static library ../../../lib/libsteed_conf.a
[ 10%] Built target steed_conf
[ 11%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/Allocator.cpp.o
[ 12%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/BoolVector.cpp.o
[ 13%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/Buffer.cpp.o
[ 14%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/DebugInfo.cpp.o
[ 15%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/FileHandler.cpp.o
[ 16%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/FileIO.cpp.o
[ 17%] Building CXX object src/steed/util/CMakeFiles/steed_util.dir/Utility.cpp.o
[ 18%] Linking CXX static library ../../../lib/libsteed_util.a
[ 18%] Built target steed_util
[ 20%] Building CXX object src/steed/base/CMakeFiles/steed_base.dir/Block.cpp.o
[ 21%] Building CXX object src/steed/base/CMakeFiles/steed_base.dir/DataType.cpp.o
[ 22%] Linking CXX static library ../../../lib/libsteed_base.a
[ 22%] Built target steed_base
[ 23%] Building CXX object src/steed/schema/CMakeFiles/steed_schema.dir/SchemaTree.cpp.o
[ 24%] Building CXX object src/steed/schema/CMakeFiles/steed_schema.dir/SchemaTreeMap.cpp.o
[ 25%] Building CXX object src/steed/schema/CMakeFiles/steed_schema.dir/SchemaTreePrinter.cpp.o
[ 26%] Linking CXX static library ../../../lib/libsteed_schema.a
[ 26%] Built target steed_schema
[ 27%] Building CXX object src/steed/store/CMakeFiles/steed_store.dir/CAB.cpp.o
[ 28%] Building CXX object src/steed/store/CMakeFiles/steed_store.dir/CABAppender.cpp.o
[ 29%] Building CXX object src/steed/store/CMakeFiles/steed_store.dir/CABLayouter.cpp.o
[ 30%] Building CXX object src/steed/store/CMakeFiles/steed_store.dir/CABReader.cpp.o
[ 31%] Building CXX object src/steed/store/CMakeFiles/steed_store.dir/CABWriter.cpp.o
[ 32%] Building CXX object src/steed/store/CMakeFiles/steed_store.dir/RecordBuilder.cpp.o
[ 33%] Linking CXX static library ../../../lib/libsteed_store.a
[ 33%] Built target steed_store
[ 34%] Building CXX object src/steed/assemble/CMakeFiles/steed_assemble.dir/ColumnAssembler.cpp.o
[ 35%] Building CXX object src/steed/assemble/CMakeFiles/steed_assemble.dir/ColumnExpressionParser.cpp.o
[ 36%] Building CXX object src/steed/assemble/CMakeFiles/steed_assemble.dir/FSMTable.cpp.o
[ 37%] Building CXX object src/steed/assemble/CMakeFiles/steed_assemble.dir/RecordNestedAssembler.cpp.o
[ 38%] Building CXX object src/steed/assemble/CMakeFiles/steed_assemble.dir/RecordOutput.cpp.o
[ 40%] Linking CXX static library ../../../lib/libsteed_assemble.a
[ 40%] Built target steed_assemble
[ 41%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/CollectionWriter.cpp.o
[ 42%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/ColumnItemGenerator.cpp.o
[ 43%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/ColumnParser.cpp.o
[ 44%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/JSONRecordBuffer.cpp.o
[ 45%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/JSONRecordNaiveParser.cpp.o
[ 46%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/JSONRecordParser.cpp.o
[ 47%] Building CXX object src/steed/parse/CMakeFiles/steed_parse.dir/JSONTypeMapper.cpp.o
[ 48%] Linking CXX static library ../../../lib/libsteed_parse.a
[ 48%] Built target steed_parse
[ 49%] Building CXX object src/steed/CMakeFiles/steed.dir/steed.cpp.o
[ 50%] Building CXX object src/steed/CMakeFiles/steed.dir/conf/Config.cpp.o
[ 51%] Building CXX object src/steed/CMakeFiles/steed.dir/util/Allocator.cpp.o
[ 52%] Building CXX object src/steed/CMakeFiles/steed.dir/util/BoolVector.cpp.o
[ 53%] Building CXX object src/steed/CMakeFiles/steed.dir/util/Buffer.cpp.o
[ 54%] Building CXX object src/steed/CMakeFiles/steed.dir/util/DebugInfo.cpp.o
[ 55%] Building CXX object src/steed/CMakeFiles/steed.dir/util/FileHandler.cpp.o
[ 56%] Building CXX object src/steed/CMakeFiles/steed.dir/util/FileIO.cpp.o
[ 57%] Building CXX object src/steed/CMakeFiles/steed.dir/util/Utility.cpp.o
[ 58%] Building CXX object src/steed/CMakeFiles/steed.dir/base/Block.cpp.o
[ 60%] Building CXX object src/steed/CMakeFiles/steed.dir/base/DataType.cpp.o
[ 61%] Building CXX object src/steed/CMakeFiles/steed.dir/store/CAB.cpp.o
[ 62%] Building CXX object src/steed/CMakeFiles/steed.dir/store/CABAppender.cpp.o
[ 63%] Building CXX object src/steed/CMakeFiles/steed.dir/store/CABLayouter.cpp.o
[ 64%] Building CXX object src/steed/CMakeFiles/steed.dir/store/CABReader.cpp.o
[ 65%] Building CXX object src/steed/CMakeFiles/steed.dir/store/CABWriter.cpp.o
[ 66%] Building CXX object src/steed/CMakeFiles/steed.dir/store/RecordBuilder.cpp.o
[ 67%] Building CXX object src/steed/CMakeFiles/steed.dir/schema/SchemaTree.cpp.o
[ 68%] Building CXX object src/steed/CMakeFiles/steed.dir/schema/SchemaTreeMap.cpp.o
[ 69%] Building CXX object src/steed/CMakeFiles/steed.dir/schema/SchemaTreePrinter.cpp.o
[ 70%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/CollectionWriter.cpp.o
[ 71%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/ColumnItemGenerator.cpp.o
[ 72%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/ColumnParser.cpp.o
[ 73%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/JSONRecordBuffer.cpp.o
[ 74%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/JSONRecordNaiveParser.cpp.o
[ 75%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/JSONRecordParser.cpp.o
[ 76%] Building CXX object src/steed/CMakeFiles/steed.dir/parse/JSONTypeMapper.cpp.o
[ 77%] Building CXX object src/steed/CMakeFiles/steed.dir/assemble/ColumnAssembler.cpp.o
[ 78%] Building CXX object src/steed/CMakeFiles/steed.dir/assemble/ColumnExpressionParser.cpp.o
[ 80%] Building CXX object src/steed/CMakeFiles/steed.dir/assemble/FSMTable.cpp.o
[ 81%] Building CXX object src/steed/CMakeFiles/steed.dir/assemble/RecordNestedAssembler.cpp.o
[ 82%] Building CXX object src/steed/CMakeFiles/steed.dir/assemble/RecordOutput.cpp.o
[ 83%] Linking CXX shared library ../../lib/libsteed.so
[ 83%] Built target steed
[ 84%] Building CXX object src/steed/unittest/CMakeFiles/test_config.dir/Config.cpp.o
[ 85%] Linking CXX executable ../../../bin/test_config
[ 85%] Built target test_config
[ 86%] Building CXX object src/steed/unittest/CMakeFiles/test_util.dir/Util.cpp.o
[ 87%] Linking CXX executable ../../../bin/test_util
[ 87%] Built target test_util
[ 88%] Building CXX object src/steed/unittest/CMakeFiles/test_base.dir/Base.cpp.o
[ 89%] Linking CXX executable ../../../bin/test_base
[ 89%] Built target test_base
[ 90%] Building CXX object src/steed/unittest/CMakeFiles/test_store.dir/Store.cpp.o
[ 91%] Linking CXX executable ../../../bin/test_store
[ 91%] Built target test_store
[ 92%] Building CXX object src/steed/unittest/CMakeFiles/test_schema.dir/Schema.cpp.o
[ 93%] Linking CXX executable ../../../bin/test_schema
[ 93%] Built target test_schema
[ 94%] Building CXX object src/steed/unittest/CMakeFiles/test_parse.dir/Parse.cpp.o
[ 95%] Linking CXX executable ../../../bin/test_parse
[ 95%] Built target test_parse
[ 96%] Building CXX object src/steed/unittest/CMakeFiles/test_assemble.dir/Assemble.cpp.o
[ 97%] Linking CXX executable ../../../bin/test_assemble
[ 97%] Built target test_assemble
[ 98%] Building CXX object tools/CMakeFiles/schema.dir/Schema.cpp.o
[100%] Linking CXX executable ../bin/schema
[100%] Built target schema
```

### 4.3 Unittest 
During build, Steed will build the unittests and tools. The following command shows how to run the unittests:
```bash
$ make test
```

Steed will run the unittests and show the following output:
```bash
$ make test
Running tests...
Test project /home/steeduser/steed_open_source/build
    Start 1: Test_Config
1/7 Test #1: Test_Config ......................   Passed    0.00 sec
    Start 2: Test_Util
2/7 Test #2: Test_Util ........................   Passed    0.01 sec
    Start 3: Test_Base
3/7 Test #3: Test_Base ........................   Passed    0.00 sec
    Start 4: Test_Store
4/7 Test #4: Test_Store .......................   Passed    0.00 sec
    Start 5: Test_Schema
5/7 Test #5: Test_Schema ......................   Passed    0.00 sec
    Start 6: Test_Parse
6/7 Test #6: Test_Parse .......................   Passed    0.00 sec
    Start 7: Test_Assemble
7/7 Test #7: Test_Assemble ....................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 7

Total Test time (real) =   0.02 sec
```
All the unittests are passed.

The executable files of the unittests and tools are in the `build/bin` directory. 
```bash
$ tree $STEED_HOME/build/bin
/home/steeduser/steed_open_source/build/bin/
├── schema          # schema tool
├── test_assemble   # assemble unittest
├── test_base       # base unittest
├── test_config     # config unittest
├── test_parse      # parse unittest
├── test_schema     # schema unittest
├── test_store      # store unittest
└── test_util       # util unittest
0 directories, 8 files
```
You can also run the executable files in the `build/bin` directory directly.

### 4.4 Install
After build, you can install the steed library and executable file using the following command:
```bash
$ sudo make install
```

The following output shows a successful install:
```bash
$ sudo make install
[  0%] Built target CLI11
Consolidate compiler generated dependencies of target gtest
[  2%] Built target gtest
Consolidate compiler generated dependencies of target gmock
[  4%] Built target gmock
Consolidate compiler generated dependencies of target gmock_main
[  6%] Built target gmock_main
Consolidate compiler generated dependencies of target gtest_main
[  8%] Built target gtest_main
Consolidate compiler generated dependencies of target steed_conf
[ 10%] Built target steed_conf
Consolidate compiler generated dependencies of target steed_util
[ 18%] Built target steed_util
Consolidate compiler generated dependencies of target steed_base
[ 22%] Built target steed_base
Consolidate compiler generated dependencies of target steed_schema
[ 26%] Built target steed_schema
Consolidate compiler generated dependencies of target steed_store
[ 33%] Built target steed_store
Consolidate compiler generated dependencies of target steed_assemble
[ 40%] Built target steed_assemble
Consolidate compiler generated dependencies of target steed_parse
[ 48%] Built target steed_parse
Consolidate compiler generated dependencies of target steed
[ 83%] Built target steed
Consolidate compiler generated dependencies of target test_config
[ 85%] Built target test_config
Consolidate compiler generated dependencies of target test_util
[ 87%] Built target test_util
Consolidate compiler generated dependencies of target test_base
[ 89%] Built target test_base
Consolidate compiler generated dependencies of target test_store
[ 91%] Built target test_store
Consolidate compiler generated dependencies of target test_schema
[ 93%] Built target test_schema
Consolidate compiler generated dependencies of target test_parse
[ 95%] Built target test_parse
Consolidate compiler generated dependencies of target test_assemble
[ 97%] Built target test_assemble
Consolidate compiler generated dependencies of target schema
[100%] Built target schema
Install the project...
-- Install configuration: "release"
-- Up-to-date: /usr/local/include
-- Installing: /usr/local/include/gmock
-- Installing: /usr/local/include/gmock/internal
-- Installing: /usr/local/include/gmock/internal/gmock-port.h
-- Installing: /usr/local/include/gmock/internal/gmock-internal-utils.h
-- Installing: /usr/local/include/gmock/internal/custom
-- Installing: /usr/local/include/gmock/internal/custom/README.md
-- Installing: /usr/local/include/gmock/internal/custom/gmock-port.h
-- Installing: /usr/local/include/gmock/internal/custom/gmock-generated-actions.h
-- Installing: /usr/local/include/gmock/internal/custom/gmock-matchers.h
-- Installing: /usr/local/include/gmock/internal/gmock-pp.h
-- Installing: /usr/local/include/gmock/gmock-spec-builders.h
-- Installing: /usr/local/include/gmock/gmock-cardinalities.h
-- Installing: /usr/local/include/gmock/gmock-more-actions.h
-- Installing: /usr/local/include/gmock/gmock.h
-- Installing: /usr/local/include/gmock/gmock-more-matchers.h
-- Installing: /usr/local/include/gmock/gmock-actions.h
-- Installing: /usr/local/include/gmock/gmock-function-mocker.h
-- Installing: /usr/local/include/gmock/gmock-matchers.h
-- Installing: /usr/local/include/gmock/gmock-nice-strict.h
-- Installing: /usr/local/lib/libgmock.a
-- Installing: /usr/local/lib/libgmock_main.a
-- Installing: /usr/local/lib/pkgconfig/gmock.pc
-- Installing: /usr/local/lib/pkgconfig/gmock_main.pc
-- Installing: /usr/local/lib/cmake/GTest/GTestTargets.cmake
-- Installing: /usr/local/lib/cmake/GTest/GTestTargets-release.cmake
-- Installing: /usr/local/lib/cmake/GTest/GTestConfigVersion.cmake
-- Installing: /usr/local/lib/cmake/GTest/GTestConfig.cmake
-- Up-to-date: /usr/local/include
-- Installing: /usr/local/include/gtest
-- Installing: /usr/local/include/gtest/internal
-- Installing: /usr/local/include/gtest/internal/gtest-string.h
-- Installing: /usr/local/include/gtest/internal/gtest-port.h
-- Installing: /usr/local/include/gtest/internal/custom
-- Installing: /usr/local/include/gtest/internal/custom/README.md
-- Installing: /usr/local/include/gtest/internal/custom/gtest-port.h
-- Installing: /usr/local/include/gtest/internal/custom/gtest-printers.h
-- Installing: /usr/local/include/gtest/internal/custom/gtest.h
-- Installing: /usr/local/include/gtest/internal/gtest-type-util.h
-- Installing: /usr/local/include/gtest/internal/gtest-internal.h
-- Installing: /usr/local/include/gtest/internal/gtest-param-util.h
-- Installing: /usr/local/include/gtest/internal/gtest-filepath.h
-- Installing: /usr/local/include/gtest/internal/gtest-port-arch.h
-- Installing: /usr/local/include/gtest/internal/gtest-death-test-internal.h
-- Installing: /usr/local/include/gtest/gtest-assertion-result.h
-- Installing: /usr/local/include/gtest/gtest-message.h
-- Installing: /usr/local/include/gtest/gtest-param-test.h
-- Installing: /usr/local/include/gtest/gtest-matchers.h
-- Installing: /usr/local/include/gtest/gtest_pred_impl.h
-- Installing: /usr/local/include/gtest/gtest-spi.h
-- Installing: /usr/local/include/gtest/gtest-typed-test.h
-- Installing: /usr/local/include/gtest/gtest-death-test.h
-- Installing: /usr/local/include/gtest/gtest-printers.h
-- Installing: /usr/local/include/gtest/gtest-test-part.h
-- Installing: /usr/local/include/gtest/gtest.h
-- Installing: /usr/local/include/gtest/gtest_prod.h
-- Installing: /usr/local/lib/libgtest.a
-- Installing: /usr/local/lib/libgtest_main.a
-- Installing: /usr/local/lib/pkgconfig/gtest.pc
-- Installing: /usr/local/lib/pkgconfig/gtest_main.pc
-- Installing: /usr/local/lib/libsteed.so
-- Installing: /usr/local/include/steed.h
```

In `CMakeLists.txt`, we define the install directory using the parameter `CMAKE_INSTALL_PREFIX`. The default install directory is `/usr/local/bin`. You can also change the install directory using the following command:
```cmake
$ cmake -DCMAKE_INSTALL_PREFIX=/mydir ..
```
or you can change the install directory in the `CMakeLists.txt` file:
```cmake
set(CMAKE_INSTALL_PREFIX /mydir)
```

Then you can find the following files in the install directory, where `${CMAKE_INSTALL_PREFIX}` is our default install directory `/usr/local`:
```bash
$ tree /usr/local/ -L 2
/usr/local/
├── # other files
├── include
│   └── steed.h     # steed header file
└── lib
    ├── libSteed.so # STEED shared library
    └── # other files 
```
`libSteed.so` is the steed shared library. You can use the steed shared library to build your own application. We will show you how to build your own application with the steed shared library in the next section.

### 4.5 Set Environment Variables

To use the steed shared library, you need to set the `LD_LIBRARY_PATH` environment variable. You can use the following command to set the `LD_LIBRARY_PATH` environment variable:
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```
Also, you can add the above command to the `.bashrc` file to set the `LD_LIBRARY_PATH` environment variable automatically when you log in.

## 5 Example
In this section, we will show you how to build your own application with the steed shared library. We will show you how to build the example application in the `examples` directory. The directory structure of the example is as follows:
```bash
$ cd $STEED_HOME/examples
$ tree
.
├── Makefile
├── json
│   └── try.json # example json text data
└── main.cpp     # example source code
```

In the `examples` directory, we have the following files:
- `Makefile` is the Makefile to build the example application;
- `json` directory contains the example json text data;
- `main.cpp` is the example source code.


### 5.1 Build with Steed shared library
We use the following Makefile to build the example application:
```Makefile
# steed example
PROJECT = SteedExample
LIBSTEED = steed
BINARY = $(PROJECT)

THIRDDIRS = ../src/thirdparty/cli11/include
SRCSUBDIR += ${THIRDDIRS}
SRCSUBDIR += ../src/steed/conf
SRCSUBDIR += ../src/steed/util
SRCSUBDIR += ../src/steed/base
SRCSUBDIR += ../src/steed/schema
SRCSUBDIR += ../src/steed/store
SRCSUBDIR += ../src/steed/assemble
SRCSUBDIR += ../src/steed/parse
SRCSUBDIR += ../src/steed/unittest
INCLUDES  = $(addprefix -I,$(SRCSUBDIR))

# compiler
CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Werror -Wall
CFLAGS += -g -O3 -fPIC
CFLAGS += -I../include $(INCLUDES)

# linker
LDFLAGS = -ldl -L/usr/local/lib -l$(LIBSTEED) 

# source files
SRCS = main.cpp

# executable file
$(BINARY): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# clean
clean:
	rm -rf $(BINARY) $(OBJS)
```

In the Makefile, we use the `LDFLAGS` variable to link the steed shared library. The following is the output of the `make` command:
```bash
$ make
g++ -o SteedExample main.cpp -std=c++11 -Wall -Wextra -Werror -Wall -g -O3 -fPIC -I../include -I../src/thirdparty/cli11/include -I../src/steed/conf -I../src/steed/util -I../src/steed/base -I../src/steed/schema -I../src/steed/store -I../src/steed/assemble -I../src/steed/parse -I../src/steed/unittest -ldl -L/usr/local/lib -lsteed
```

### 5.2 JSON Example Data
In the `examples/json/try.json` file, we have the following json records as an example:
```json
{ "item": "journal", "status": "A", "size": { "h": 14, "w": 21, "uom": "cm" }, "instock": [ { "warehouse": "A", "qty": 5 } ] }
{ "item": "postcard", "status": "A", "size": { "h": 10, "w": 15.25, "uom": "cm" }, "instock": [ { "warehouse": "B", "qty": 15 }, { "warehouse": "C", "qty": 35 } ] }
{ "item": "journal", "qty": 25, "tags": ["blank", "red"], "size": { "h": 14, "w": 21, "uom": "cm" } }
{ "item": "mat", "qty": 85, "tags": ["gray"], "size": { "h": 27.9, "w": 35.5, "uom": "cm" } }
{ "item": "mousepad", "qty": 25, "tags": ["gel", "blue"], "size": { "h": 19, "w": 22.85, "uom": "cm" } }
```

In json records, we have the following fields:
- "item" is a string field and appears in all records;
- "status" is a string field and appears in the first two records;
- "size" is a nested object field and appears in all records. It has three subfields, including "h", "w" and "uom";
- "instock" is a nested array field and appears in the first two records. It has two subfields, including "warehouse" and "qty";
- "qty" is a number field and appears in the last three records;
- "tags" is a string array field and appears in the last three records.
Then we can use the json records as an example to show how to parse json data to columnar data and assemble columnar data to row data.

### 5.3 Steed Configuration 
Steed uses CLI11 to parse the configuration file. The default configuration file is `./conf/steed.conf`. The following is an example of the configuration file:

```bash
### Binary Storage Settings 

# store base dir:
#   the base dir of all column and schema data
store_base = ./data

# column data dir:
#   the dir used to store column data
data_dir = cols      

# schema dir:
#   the dir used to store schema data
schema_dir = schema  


### Steed Runtime Settings

# mem align size:
#   the size of memory align
mem_align_size = 4096

# max column number:
#   the record capacity of a CAB (Column Aligned Block)
cab_recd_num = 8 

# max record number:
#   the record capacity in batch during parsing text json records 
text_recd_num = 16 # record number in a text file
```

In this configuration file, we set the store configuration as follows:
1. option **store_base** set the base directory as `./data`;
2. option **data_dir** set the column data directory name as `cols`;
3. option **schema_dir** set the schema directory name as `schema`.

We also set the runtime configuration as follows:
1. option **mem_align_size** set the memory alignment size as 4096;
2. option **cab_recd_num** set the record number in a CAB file as 8, which means that there are 8 records in a CAB;
3. option **text_recd_num** set the record number in a text file as 16, which means steed will parse 16 records in a batch during parsing JSON records.


### 5.4 Run Example
In the main.cpp file, we use the steed library to parse the json data in the `json` directory to columnar data. The following is the `main.cpp` file:

```cpp
#include "steed.h"

using namespace steed;

int main(int argc, char *argv[])
{
    (void)argc, (void)argv; 

    // use default config file
    string conf_file(""); 
    steed::init(conf_file);

    std::string db("demo"), table("mytry"); 
    createDatabase(db);
    createTable(db, table);

    { // create a column parser to parse JSON records  
        std::string jpath("./json/try.json");
        ifstream ifs(jpath);
        if (!ifs.is_open())
        {
            printf("Main: cannot open [%s]!\n", jpath.c_str());
            return -1;
        } // ifs
    
        steed::ColumnParser *cp = new steed::ColumnParser();
        istream *is = &ifs;
        if (cp->init(db, table, is) < 0)
        {
            printf("Main: ColumnParser init failed!\n");
            return -1;
        } // if
    
        int status = 0;
        while ((status = cp->parseOne()) > 0)
        {   /* do nothing */   }
    
        delete cp; cp = nullptr;
        ifs.close();

        if (status < 0)
        {
            printf("Main: insert failed!\n");
            return -1;
        } // if
    }

    { // create a column assembler to assemble binary records and output
        std::vector< std::string > cols;
        cols.emplace_back("item");
        cols.emplace_back("size.h");

        ColumnAssembler *ca = new ColumnAssembler();
        if (ca->init(db, table, cols) < 0)
        {
            printf("STEED::init ColumnAssembler failed!\n");
            return -1;
        } // if

        char *rbgn = nullptr;
        ostream *ostrm = &std::cout;
        RecordOutput ro( ca->getSchemaTree() );
        while (ca->getNext(rbgn) > 0)
        { ro.outJSON2Strm(ostrm, rbgn); }

        delete ca; ca = nullptr;
    }

    dropTable(db, table);
    dropDatabase(db);

    steed::uninit();

    return 0; 
} // main
```

The example has the following steps:
1. In the main function, we first use the `init` function to initialize the steed library. 
2. Then, we create a database named `demo` and a table named `mytry` in the `demo` database. 
3. Then the example creates an ifstream object to read the json data in the `json` directory and use this object to construct a **ColumnParser** object. Then it uses the ColumnParser object to parse the json data to columnar data.
4. After parsing the json data, the example creates a **ColumnAssembler** object to assemble the columnar data to row data and output the row data to the standard output.
5. Finally, the example drops the `mytry` table and `demo` database.


#### 5.4.1 Config and init Static

Steer uses the `Config` class to store the configuration. We define a global `Config` object named `g_config` in `src/steed/steed.cpp` and use the `init` and `uninit` function to initialize and uninit the steed static data. Here we use an empty string to init and use the default config file `./conf/steed.conf` to initialize the steed library:
```cpp
int main(int argc, char *argv[])
{
    (void)argc, (void)argv; 

    // use default config file
    string conf_file(""); 
    steed::init(conf_file);

    // ...

    steed::uninit();

    return 0; 
} // main
```


#### 5.4.2 Create Database
In this section, we will show you how to create a database. We set the store base directory as `./data` in the configuration file and the directory is empty now.

You can create the `data` directory manually or steed will create the `data` directory automatically when you create a database. Now the `data` directory is empty:
```bash
$ tree ./data
.
0 directories, 0 files
```

In the main function, we use the `createDatabase` function to create a database named `demo`:
```cpp
int main(int argc, char *argv[])
{
    // ...

    std::string db("demo"), table("mytry"); 
    createDatabase(db);

    // ...

} // main
```

Steed will create the `demo` database directory in the store base directory. There are two subdirectories in the database directory, including `cols` and `schema` . The `cols` directory is used to store the columnar data, the `schema` directory is used to store the schema of the table.
```bash
$ tree ./data
./data
└── demo
    ├── cols
    └── schema
3 directories, 0 files
```

#### 5.4.3 Create Table
Then we can create a table named `mytry` in the `demo` database in the main function:
```cpp
int main(int argc, char *argv[])
{
    // ...

    std::string db("demo"), table("mytry"); 
    createDatabase(db);
    createTable(db, table);

    // ...

} // main
```

After that, we can find the `mytry` directory in the `cols` and `schema` directories. the `mytry` directory in the `schema` directory is used to store the schema of the `mytry` table, and the `mytry` directory in the `cols` directory is used to store the columnar data of the `mytry` table. Since there is no data in the `mytry` table, the `mytry` directory in the `cols` directory is empty now.

```bash
$ tree ./data
./data
└── demo
    ├── cols
    │   └── mytry
    └── schema
        └── mytry
```

#### 5.4.4 Parse Data
Now we can parse the json data in the `json` directory to columnar data. In the main function, we use the `parseOne` function to parse the json records to columnar data one by one. Steed also provides the `parseAll` function to parse all the json records in a batch. Here is an example:
```cpp
int main(int argc, char *argv[])
{
    // ...

    { // create a column parser to parse JSON records  
        std::string jpath("./json/try.json");
        ifstream ifs(jpath);
        if (!ifs.is_open())
        {
            printf("Main: cannot open [%s]!\n", jpath.c_str());
            return -1;
        } // ifs
    
        steed::ColumnParser *cp = new steed::ColumnParser();
        istream *is = &ifs;
        if (cp->init(db, table, is) < 0)
        {
            printf("Main: ColumnParser init failed!\n");
            return -1;
        } // if
    
        int status = 0;
        while ((status = cp->parseOne()) > 0)
        {   /* do nothing */   }
    
        delete cp; cp = nullptr;
        ifs.close();

        if (status < 0)
        {
            printf("Main: insert failed!\n");
            return -1;
        } // if
    }

    // ...

} // main
```

Steed use the `ColumnParser` class to parse the json data to columnar data. The `ColumnParser` class provides the `init` function to initialize the `ColumnParser` object. The following is the `init` function in ColumnParser.h:
```cpp
    /**
     * init to parse: infer the SchemaTree from SampleTree
     * @param db   database name
     * @param clt  table name 
     * @param is   JSON text records input stream
     * @return 0 success; <0 failed 
     */
    int init (const string &db, const string &clt, istream *is);
```

You could also create a std::istringstream object to read the json data from a string. Here is an example:
```cpp
    std::string jstr = "{\"item\": \"journal\", \"status\": \"A\", \"size\": { \"h\": 14, \"w\": 21, \"uom\": \"cm\" }, \"instock\": [ { \"warehouse\": \"A\", \"qty\": 5 } ] }";
    std::istringstream iss(jstr);
    ColumnParser cp("demo", "mytry", &iss);
```

Then the ColumnParser object will parse the json data to columnar data and store the columnar data in the `mytry` directory under the `cols` directory.

After that, we can find the columnar data in the `mytry` directory in the `cols` directory. The `mytry` directory contains the columnar data of the `mytry` table. The `mytry` directory contains the following files:
```bash
$ tree ./data
./data
└── demo
    ├── cols
    │   └── mytry
    │       ├── item#1#I.cab
    │       ├── item#1#I.cab.info
    │       ├── size#1#A.h#1#H.cab
    │       ├── size#1#A.h#1#H.cab.info
    │       ├── tags#2#I.cab
    │       ├── tags#2#I.cab.info
    │       └── # other columnar data 
    └── schema
        └── mytry
```
After parsing, steed generates 2 files for each column: `.cab` for the columnar data (aligned by ColumnAlignedBlock, CAB for short) and `.cab.info` for the CAB header information. 

For example, `size#1#A.h#1#H.cab` is the CAB file for the `size.h` column. We use path expression to express the nested field: the `size` column is a nested object column, and the `h` subfield is a number column separated by '.'. The `size#1#A.h#1#H.cab` file is the CAB file for the `h` subfield. 

Steed also update the schema info during the parsing progress. After that, the schema info will be flushed to the file `demo/schema/mytry`.

#### 5.4.5 Assemble Data
Using the `assemble` function, we can assemble the columnar data to row data. In the main function, we use the `assemble` function to assemble the columnar data to row data:

```cpp
int main(int argc, char *argv[])
{
    // ...

    { // create a column assembler to assemble binary records and output
        std::vector< std::string > cols;
        cols.emplace_back("item");
        cols.emplace_back("size.h");

        ColumnAssembler *ca = new ColumnAssembler();
        if (ca->init(db, table, cols) < 0)
        {
            printf("STEED::init ColumnAssembler failed!\n");
            return -1;
        } // if

        char *rbgn = nullptr;
        ostream *ostrm = &std::cout;
        RecordOutput ro( ca->getSchemaTree() );
        while (ca->getNext(rbgn) > 0)
        { ro.outJSON2Strm(ostrm, rbgn); }

        delete ca; ca = nullptr;
    }

    // ...

} // main
```

In this example, we only assemble the `item` and `size.h` columns. Steed will assemble the `item` and `size.h` columns to row data and print the row data to the standard output. We create a ColumnAssembler object to assemble the columnar data to row data. It is inited by the `init` function:
```cpp
    /**
     * init to assemble: infer the SchemaTree from SampleTree
     * @param db   database name
     * @param clt  table name 
     * @param cols column names to assemble
     * @return 0 success; <0 failed 
     */
    int init (const string &db, const string &clt, const vector<string> &cols);
```

Then the ColumnAssemble object will assemble the columnar data to row data and RecordOutput object will output the row data to the standard output.  The following is the row data:
```json
{"item":"journal","size":{"h":14.000000}}
{"item":"postcard","size":{"h":10.000000}}
{"item":"journal","size":{"h":14.000000}}
{"item":"mat","size":{"h":27.900000}}
{"item":"mousepad","size":{"h":19.000000}}
```


#### 5.4.6 Drop Table and Database

Finally, we can drop the table and database using the `dropTable` and `dropDatabase` functions. In the main function, we use the `dropTable` and `dropDatabase` functions to drop the `mytry` table and `demo` database:
```cpp
int main(int argc, char *argv[])
{
    // ...

    dropTable(db, table);
    dropDatabase(db);

    // ...

} // main
```

Then we can find the `demo` directory in the store base directory is removed.
```bash
$ tree ./data
./data
0 directories, 0 files
```


