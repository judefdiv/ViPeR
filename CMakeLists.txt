# To run the code:
#  - mkdir build && cd build
#  - cmake ..
#  - make

cmake_minimum_required(VERSION 2.6)
project(Die2Sim)                                  # creates ${PROJECT_NAME}

set(SOURCES
  src/main.cpp
  src/genFunc.cpp
  src/ParserABC.cpp
  src/ParserBlif.cpp
  src/ForgeSFQblif.cpp
  src/ForgeSFQABC.cpp
  src/ParserDot.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Ensures that the header files of the project is included
target_include_directories(${PROJECT_NAME} PRIVATE 
  ${PROJECT_SOURCE_DIR}/include
)

# Reference to ABC
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include/abc EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} libabc)

# --------------------------------------------------------------------------
# --------------------------- Garbage --------------------------------------
# --------------------------------------------------------------------------

# ABC

# target_compile_options(${PROJECT_NAME}
#   PRIVATE ABC_USE_NO_READLINE=1
# )

# INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/abc)
# include(include/abc/CMakeLists.txt)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC abc::all)

# target_link_libraries(${PROJECT_NAME}
#   ${PROJECT_SOURCE_DIR}/include/libabc.a
# )

# add_compile_definitions(-Wall)

# target_compile_definitions(${PROJECT_NAME}
#   PRIVATE -lm -ldl -lreadline -lpthread
# )