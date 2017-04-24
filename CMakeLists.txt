cmake_minimum_required(VERSION 3.5)
project(ezhic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories(lib-src
        lib-src/Circuitry)

set(SOURCE_FILES
        lib-src/EventWrapper.cpp
        lib-src/Registry.cpp
        lib-src/Bundle.cpp
        lib-src/Event.cpp
        lib-src/BRunner.cpp
        lib-src/Circuitry/CircuitPredicates.cpp
        lib-src/Circuitry/IntentionTagger.cpp
        lib-src/Circuitry/Circuit.cpp
        lib-src/StreamLogger.cpp
        lib-src/EventWrapper.h
        lib-src/Registry.h
        lib-src/AcceptUpdates.h
        lib-src/Bundle.h
        lib-src/BRunner_IMP.h
        lib-src/BRunner.h
        lib-src/Event.h
        lib-src/Handles.h
        lib-src/Traits.h
        lib-src/Circuitry/CircuitPredicates.h
        lib-src/Circuitry/IntentionTagger.h
        lib-src/Circuitry/Circuit.h
        lib-src/Circuitry/Circuitry.h
        lib-src/Circuitry/Travellers.h lib-src/Validator.h lib-src/AcceptUpdates.cpp
        tests/end2end/endtoend.cpp
        lib-src/Timer.h lib-src/Timer.cpp)

add_subdirectory(tests)
#add_subdirectory(benchmarks)
add_executable(endToEnd ${SOURCE_FILES})
target_compile_options(endToEnd PRIVATE "-Werror" "-std=c++11")
