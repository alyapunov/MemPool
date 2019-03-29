cmake_minimum_required(VERSION 3.5)
project(MemPool)

SET(DEFAULT_BUILD_TYPE "Debug")
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
ENDIF()
SET(EXPECTED_BUILD_TYPES Debug Release RelWithDebInfo MinSizeRel)
IF("${CMAKE_BUILD_TYPE}" IN_LIST EXPECTED_BUILD_TYPES)
    MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
ELSE()
    MESSAGE(WARNING "Unexcpected build type: ${CMAKE_BUILD_TYPE}")
ENDIF()

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_C_STANDARD 11)
ADD_COMPILE_OPTIONS(-Wall -Wextra -Wpedantic -Werror)

include_directories(. ./Lists)
add_subdirectory(./Lists)

add_executable(GlobalMemPoolUnit.test MemPoolCommon.hpp GlobalMemPool.hpp GlobalMemPoolUnitTest.cpp)
add_executable(GlobalMemPoolPerf.test MemPoolCommon.hpp GlobalMemPool.hpp GlobalMemPoolPerfTest.cpp)
add_executable(LocalMemPoolUnit.test MemPoolCommon.hpp LocalMemPool.hpp GlobalMemPool.hpp LocalMemPoolUnitTest.cpp)
add_executable(LocalMemPoolPerf.test MemPoolCommon.hpp LocalMemPool.hpp GlobalMemPool.hpp LocalMemPoolPerfTest.cpp)

enable_testing()
add_test(NAME GlobalMemPoolUnit.test COMMAND GlobalMemPoolUnit.test)
add_test(NAME LocalMemPoolUnit.test COMMAND LocalMemPoolUnit.test)

