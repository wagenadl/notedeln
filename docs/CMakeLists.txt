set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles" ${CMAKE_MODULE_PATH})
find_package(Sphinx QUIET)

if (SPHINX_EXECUTABLE)
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
    
    add_custom_target(_sphinx ALL
                      COMMAND
                      ${SPHINX_EXECUTABLE} -b html
                      ${SPHINX_SOURCE} ${SPHINX_BUILD}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating documentation with Sphinx")
else()
  message(WARNING "Sphinx not found - not building documentation")
endif()

