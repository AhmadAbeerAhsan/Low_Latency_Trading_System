file(GLOB SOURCES "*.cpp")

include_directories(${PROJECT_SOURCE_DIR}/common)

add_library(libcommon STATIC ${SOURCES})

list(APPEND LIBS libcommon)

add_executable(thread_test test/thread_utils_test.cpp)
target_link_libraries(thread_test PUBLIC ${LIBS})

add_executable(memory_pool_test test/memory_pool_test.cpp)

add_executable(lock_free_queue_test test/lock_free_queue_test.cpp)

add_executable(logging_test test/logging_test.cpp)

add_executable(tcp_test test/tcp_test.cpp)
target_link_libraries(tcp_test Boost::headers)

