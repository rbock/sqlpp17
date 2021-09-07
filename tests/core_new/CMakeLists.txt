add_executable(core_unit_tests)
target_link_libraries(core_unit_tests PRIVATE Catch2::Catch2 sqlpp17)
target_sources(core_unit_tests
    PRIVATE
        main.cpp
)
target_include_directories(core_unit_tests
  PRIVATE
    ${PROJECT_SOURCE_DIR}/tests/include
)

add_subdirectory(function)

catch_discover_tests(core_unit_tests
    PROPERTIES
        LABELS "UnitTests"
)
