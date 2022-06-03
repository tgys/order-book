set(src
  order_book.cc
        )

add_library(${LOB_LIB_NAME} ${LOB_LIB_TYPE} ${src})

add_executable(orderbook main.cc)
target_link_libraries(orderbook ${LOB_LIB_NAME} ${CMAKE_THREAD_LIBS_INIT} ${MATH_LIBRARIES})

install(TARGETS ${LOB_LIB_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(TARGETS orderbook RUNTIME DESTINATION bin)
install(DIRECTORY ../include/LOB DESTINATION include)
