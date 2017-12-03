add_library(sqlpp17-connector-mysql
		connection.cpp
)

if (APPLE)
	target_link_libraries(sqlpp17-connector-mysql "${Boost_THREAD_LIBRARY}")
endif()

target_link_libraries(sqlpp17-connector-mysql PUBLIC sqlpp17)

install(TARGETS sqlpp17-connector-mysql DESTINATION lib)
