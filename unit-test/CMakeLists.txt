include(FetchContent)

set(DOCTEST_NO_INSTALL ON)
FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.9)
FetchContent_MakeAvailable(doctest)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
set_target_properties(doctest_with_main PROPERTIES COMPILE_OPTIONS "" EXPORT_COMPILE_COMMANDS OFF)
get_target_property(DOCTEST_INCLUDE_DIRS doctest INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(doctest SYSTEM INTERFACE ${DOCTEST_INCLUDE_DIRS})

add_executable(unit_test
    hello_world.cpp
    collision_test.cpp
    joint_test.cpp
    math_test.cpp
    world_test.cpp)
target_link_libraries(unit_test PUBLIC box2d::box2d doctest::doctest_with_main)
doctest_discover_tests(unit_test)
