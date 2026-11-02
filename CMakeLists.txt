cmake_minimum_required(VERSION 3.20)

project(Lab5_StatisticalAnalysis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(StatisticalSample STATIC
    src/StatisticalSample.cpp
    src/ScopeLogger.cpp
)

target_include_directories(StatisticalSample
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(StatisticalSample
    PUBLIC STATISTICS_STATIC
)

add_executable(StatisticalDemo
    demo/main.cpp
)

target_link_libraries(StatisticalDemo
    PRIVATE StatisticalSample
)

add_executable(ExceptionDemo
    exceptions_demo/main.cpp
)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(StatisticalSampleTests
        tests/StatisticalSampleTests.cpp
    )

    target_link_libraries(StatisticalSampleTests
        PRIVATE StatisticalSample GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(StatisticalSampleTests)
endif()