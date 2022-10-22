add_library(box2d
	collision/b2_broad_phase.cpp
	collision/b2_chain_shape.cpp
	collision/b2_circle_shape.cpp
	collision/b2_collide_circle.cpp
	collision/b2_collide_edge.cpp
	collision/b2_collide_polygon.cpp
	collision/b2_collision.cpp
	collision/b2_distance.cpp
	collision/b2_dynamic_tree.cpp
	collision/b2_edge_shape.cpp
	collision/b2_polygon_shape.cpp
	collision/b2_time_of_impact.cpp
	common/b2_block_allocator.cpp
	common/b2_draw.cpp
	common/b2_math.cpp
	common/b2_settings.cpp
	common/b2_stack_allocator.cpp
	common/b2_timer.cpp
	dynamics/b2_body.cpp
	dynamics/b2_chain_circle_contact.cpp
	dynamics/b2_chain_circle_contact.h
	dynamics/b2_chain_polygon_contact.cpp
	dynamics/b2_chain_polygon_contact.h
	dynamics/b2_circle_contact.cpp
	dynamics/b2_circle_contact.h
	dynamics/b2_contact.cpp
	dynamics/b2_contact_manager.cpp
	dynamics/b2_contact_solver.cpp
	dynamics/b2_contact_solver.h
	dynamics/b2_distance_joint.cpp
	dynamics/b2_edge_circle_contact.cpp
	dynamics/b2_edge_circle_contact.h
	dynamics/b2_edge_polygon_contact.cpp
	dynamics/b2_edge_polygon_contact.h
	dynamics/b2_fixture.cpp
	dynamics/b2_friction_joint.cpp
	dynamics/b2_gear_joint.cpp
	dynamics/b2_island.cpp
	dynamics/b2_island.h
	dynamics/b2_joint.cpp
	dynamics/b2_motor_joint.cpp
	dynamics/b2_mouse_joint.cpp
	dynamics/b2_polygon_circle_contact.cpp
	dynamics/b2_polygon_circle_contact.h
	dynamics/b2_polygon_contact.cpp
	dynamics/b2_polygon_contact.h
	dynamics/b2_prismatic_joint.cpp
	dynamics/b2_pulley_joint.cpp
	dynamics/b2_revolute_joint.cpp
	dynamics/b2_weld_joint.cpp
	dynamics/b2_wheel_joint.cpp
	dynamics/b2_world.cpp
	dynamics/b2_world_callbacks.cpp
	rope/b2_rope.cpp)
target_include_directories(box2d
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_features(box2d PUBLIC cxx_std_17)

set_target_properties(box2d PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(MSVC)
  set_target_properties(box2d PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMPILE_PDB_NAME box2d
  )

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/box2d.pdb"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OPTIONAL
  )
endif()

if (BUILD_SHARED_LIBS)
  target_compile_definitions(box2d
    PUBLIC
      B2_SHARED
  )
endif()

install(
  TARGETS box2d
  EXPORT box2dConfig
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  EXPORT box2dConfig
  NAMESPACE box2d::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/box2d"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/box2dConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/box2dConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/box2d"
)
