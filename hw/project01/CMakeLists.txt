cmake_minimum_required(VERSION 3.10)
project(project01)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Where are the reflex include and library files?
#

IF(APPLE)
  set(REFLEX_LIB "/usr/local/lib/libreflex.a")
  set(REFLEX_INCLUDE "/usr/local/include/reflex")
  set(REFLEX reflex)
ELSE(APPLE)
  set(REFLEX_LIB "/home/faculty/michal/classes/cis461/RE-flex/lib/libreflex.a")
  set(REFLEX_INCLUDE "/home/faculty/michal/classes/cis461/RE-flex/include")
  set(REFLEX "/home/faculty/michal/classes/cis461/RE-flex/bin/reflex")
ENDIF(APPLE)

# Run re-Flex
add_custom_command(
    COMMAND  ${REFLEX} --bison-cc --bison-locations --header-file ${CMAKE_CURRENT_SOURCE_DIR}/quack.lxx
    OUTPUT   lex.yy.cpp lex.yy.h
    DEPENDS  quack.lxx quack.tab.hxx
)
## Run Bison
#add_custom_command(
#    COMMAND bison -d -v ${CMAKE_CURRENT_SOURCE_DIR}/project00.yxx --report-file=debug.output
#    OUTPUT  quack.tab.cxx quack.tab.hxx location.hh position.hh stack.hh
#    DEPENDS quack.yxx
#)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${REFLEX_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(project01
               main.cpp
               lex.yy.h lex.yy.cpp
               Messages.h Messages.cpp
               ASTNode.h ASTNode.cpp
               )

target_link_libraries(project01 ${REFLEX_LIB})
