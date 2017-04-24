cmake_minimum_required(VERSION 3.5)
project(ezhic)

add_subdirectory(googletest)
include_directories(../lib-src)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(SOURCE_FILES
        ../lib-src/Circuitry/CircuitPredicates.cpp
        ../lib-src/Circuitry/Circuit.cpp
        ../lib-src/Circuitry/IntentionTagger.cpp
        ../lib-src/EventWrapper.cpp
        ../lib-src/Registry.cpp
        ../lib-src/Bundle.cpp
        ../lib-src/Event.cpp
        ../lib-src/BRunner.cpp
        ../lib-src/AcceptUpdates.cpp
        ../lib-src/StreamLogger.cpp

        Helpers.h
        Circuitry/Traversal.t.cpp
        Circuitry/CircuitPredicates.t.cpp
        Task.t.cpp
        Registry.t.cpp
        Bundle.t.cpp
        BRunner.t.cpp
        AcceptUpdates.t.cpp
        StreamLogger.t.cpp
        Validator.t.cpp)

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests gtest gtest_main)