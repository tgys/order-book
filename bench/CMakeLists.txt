set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

add_subdirectory(benchmark)
include_directories(benchmark/include)

set(bench_cc
        benchmarks.cc
)

add_executable(bench_LOB ${bench_cc})

target_link_libraries(bench_LOB
  benchmark
  ${LOB_LIB_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${MATH_LIBRARIES}
)
