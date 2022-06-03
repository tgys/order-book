add_subdirectory(gtest)

include_directories(./gtest/googletest/include)

set(test_src
  run_all.cc
        )

add_executable(test_LOB ${test_src})

target_link_libraries(test_LOB
  gtest
  ${LOB_LIB_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${MATH_LIBRARIES}
)

add_test(
  NAME tests
  COMMAND test_LOB
)
