add_library(sqlpp17-connector-mysql
		connection.cpp
		connection_pool.cpp
		direct_execution_result.cpp
		prepared_statement_result.cpp
)

if (APPLE)
	target_link_libraries(sqlpp17-connector-mysql "${Boost_THREAD_LIBRARY}")
endif()

target_link_libraries(sqlpp17-connector-mysql PUBLIC sqlpp17)

install(TARGETS sqlpp17-connector-mysql DESTINATION lib)
