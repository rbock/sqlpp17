add_library(sqlpp17-connector-postgresql
		char_result.cpp
		connection.cpp
		connection_pool.cpp
		prepared_statement.cpp
)
set_target_properties(sqlpp17-connector-postgresql PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)


target_include_directories(sqlpp17-connector-postgresql PUBLIC ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(sqlpp17-connector-postgresql PUBLIC sqlpp17)
target_link_libraries(sqlpp17-connector-postgresql PUBLIC "${PostgreSQL_LIBRARIES}")

install(TARGETS sqlpp17-connector-postgresql DESTINATION lib)
