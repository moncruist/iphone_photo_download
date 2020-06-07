add_executable(simple_test simple.cpp)

target_link_libraries(simple_test gmock_main)

add_test(NAME simple_test COMMAND simple_test)