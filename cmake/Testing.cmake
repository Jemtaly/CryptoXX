# Modern Test Framework
file(GLOB TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(unit_tests ${TEST_SOURCES})
    target_link_libraries(unit_tests PRIVATE cryptoxx)
    
    # Register tests with CTest
    add_test(NAME unit_tests COMMAND unit_tests)
    
    message(STATUS "Unit tests enabled with simple framework")
else()
    message(STATUS "No test sources found in tests/ directory")
endif()