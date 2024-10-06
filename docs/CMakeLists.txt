# Doxygen configuration (documentation)

include(FetchContent)

set(DOXY_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/src/export)
set(DOXY_DOCS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(DOXY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

configure_file(doxy.in doxy.cfg @ONLY)

add_custom_target(
        docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxy.cfg
        WORKING_DIRECTORY ${DOXY_SOURCE_DIRECTORY}
        VERBATIM
        SOURCES
        doxy.in
        # add more here
)
