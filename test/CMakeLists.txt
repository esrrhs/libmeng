add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic PRIVATE meng)
add_test(NAME test_basic COMMAND test_basic)

add_executable(test_cpp test_cpp.cpp)
target_link_libraries(test_cpp PRIVATE meng)
add_test(NAME test_cpp COMMAND test_cpp)

add_executable(test_legacy test.cpp)
target_link_libraries(test_legacy PRIVATE meng)
if(MENG_ENABLE_PROFILER)
    target_link_libraries(test_legacy PRIVATE profiler)
    target_compile_definitions(test_legacy PRIVATE MENG_ENABLE_PROFILER)
endif()
add_test(NAME test_legacy COMMAND test_legacy)

if(MENG_BUILD_BENCHMARKS)
    add_executable(benchmark benchmark.cpp)
    target_link_libraries(benchmark PRIVATE meng)
endif()

# Set output directory to bin/ in build folder
set_target_properties(test_basic test_cpp test_legacy
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
if(MENG_BUILD_BENCHMARKS)
    set_target_properties(benchmark
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()
