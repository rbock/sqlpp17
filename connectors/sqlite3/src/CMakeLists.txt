add_library(sqlpp17-connector-sqlite3
		connection.cpp
		connection_pool.cpp
		prepared_statement.cpp
		prepared_statement_result.cpp
)

target_include_directories(sqlpp17-connector-sqlite3 PUBLIC ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(sqlpp17-connector-sqlite3 PUBLIC sqlpp17)
target_link_libraries(sqlpp17-connector-sqlite3 PUBLIC "${SQLITE3_LIBRARIES}")

install(TARGETS sqlpp17-connector-sqlite3 DESTINATION lib)
