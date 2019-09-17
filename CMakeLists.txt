# To run the code:
#  - mkdir build && cd build
#  - cmake ..
#  - make

cmake_minimum_required(VERSION 2.6)
project(ViPeR)                                  # creates ${PROJECT_NAME}

set(SOURCES
  src/main.cpp
  src/genFunc.cpp
  src/ParserABC.cpp
  src/ParserBlif.cpp
  src/ForgeSFQblif.cpp
  src/ParserDot.cpp
  src/gdsCpp.cpp
  src/gdsParser.cpp
  src/gdsForge.cpp
  src/placement.cpp
  src/toolFlow.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Ensures that the header files of the project is included
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

# Reference to ABC
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include/abc EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} libabc)

# Debugging...
# set(CMAKE_BUILD_TYPE Debug)