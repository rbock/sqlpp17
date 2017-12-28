add_library(sqlpp17-connector-sqlite3
		connection.cpp
		connection_pool.cpp
		prepared_statement_result.cpp
)

target_link_libraries(sqlpp17-connector-sqlite3 PUBLIC sqlpp17)

install(TARGETS sqlpp17-connector-sqlite3 DESTINATION lib)
