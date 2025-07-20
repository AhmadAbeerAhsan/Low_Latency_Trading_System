file(GLOB SOURCES "*.cpp")

include_directories(${PROJECT_SOURCE_DIR}/common)

#add_library(libcommon STATIC ${SOURCES})

#list(APPEND LIBS libcommon)

add_executable(thread_example test/thread_utils_test.cpp)
#target_link_libraries(thread_example PUBLIC ${LIBS})