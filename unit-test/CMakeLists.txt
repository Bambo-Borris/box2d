add_executable(unit_test
    doctest.h
    hello_world.cpp
    collision_test.cpp
    joint_test.cpp
    math_test.cpp
    world_test.cpp)
target_link_libraries(unit_test PUBLIC box2d::box2d)
add_test(NAME unit_test COMMAND unit_test)
