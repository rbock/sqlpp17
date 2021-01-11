include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(SQLPP17_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/sqlpp17 CACHE STRING "Path to Sqlpp17 cmake files")

function(install_component)
    set(options)
    set(oneValueArgs NAME DIRECTORY)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    install(FILES ${PROJECT_SOURCE_DIR}/cmake/configs/${ARG_NAME}Config.cmake 
        DESTINATION ${SQLPP17_INSTALL_CMAKEDIR}
    )

    install(TARGETS ${ARG_TARGETS}
        EXPORT Sqlpp17Targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT Sqlpp17Targets
        DESTINATION ${SQLPP17_INSTALL_CMAKEDIR}  
        NAMESPACE   sqlpp17::                    
    )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/sqlpp17/${ARG_DIRECTORY}
        DESTINATION include/sqlpp17
        FILES_MATCHING
        PATTERN *.h
    )
endfunction()