cmake_minimum_required(VERSION 3.10)
project(project00)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Where are the reflex include and library files?
#

IF(APPLE)
  set(REFLEX_LIB "/usr/local/lib/libreflex.a")
  set(REFLEX_INCLUDE "/usr/local/include/reflex")
ELSE(APPLE)
  set(REFLEX_LIB "/home/faculty/michal/classes/cis461/RE-flex/bin/reflex")
  set(REFLEX_INCLUDE "/home/faculty/michal/classes/cis461/RE-flex/")
ENDIF(APPLE)

# Run re-Flex
add_custom_command(
    COMMAND  reflex --bison-cc --bison-locations --header-file ${CMAKE_CURRENT_SOURCE_DIR}/project00.lxx
    OUTPUT   lex.yy.cpp lex.yy.h
    DEPENDS  project00.lxx project00.tab.hxx
)
# Run Bison
add_custom_command(
    COMMAND bison -d -v ${CMAKE_CURRENT_SOURCE_DIR}/project00.yxx --report-file=debug.output
    OUTPUT  project00.tab.cxx project00.tab.hxx location.hh position.hh stack.hh
    DEPENDS project00.yxx
)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${REFLEX_INCLUDE}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(project00 main.cpp lex.yy.cpp lex.yy.h
               Messages.h Messages.cpp tree_node.h html_parser.h
               project00.tab.cxx project00.tab.hxx location.hh position.hh stack.hh)

target_link_libraries(project00 ${REFLEX_LIB})
