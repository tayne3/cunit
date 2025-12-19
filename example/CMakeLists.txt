add_executable(sample sample.c)
add_test(NAME sample COMMAND sample)
target_link_libraries(sample cunit_compile_dependency cunit::cunit)

add_executable(sample_cpp sample_cpp.cpp)
add_test(NAME sample_cpp COMMAND sample_cpp)
target_link_libraries(sample_cpp cunit_compile_dependency cunit::cunit)

add_executable(expect_fail expect_fail.c)
add_test(NAME expect_fail COMMAND expect_fail)
target_link_libraries(expect_fail cunit_compile_dependency cunit::cunit)
set_tests_properties(expect_fail PROPERTIES WILL_FAIL TRUE)

add_executable(collect_mode collect_mode.c)
add_test(NAME collect_mode COMMAND collect_mode)
target_link_libraries(collect_mode cunit_compile_dependency cunit::cunit)
