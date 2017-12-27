add_library(sqlpp17-connector-postgresql
		char_result.cpp
		connection.cpp
		connection_pool.cpp
)

if (APPLE)
	target_link_libraries(sqlpp17-connector-postgresql "${Boost_THREAD_LIBRARY}")
endif()

target_link_libraries(sqlpp17-connector-postgresql PUBLIC sqlpp17)

install(TARGETS sqlpp17-connector-postgresql DESTINATION lib)
