cmake_minimum_required(VERSION 3.18)

project(trash)

# Find source files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/*.c
)

# Find header files
file(GLOB_RECURSE HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/*.h
)

# Based on found source and header files, add the executable
add_executable(${PROJECT_NAME}
	${HEADER_FILES}
	${SOURCE_FILES}
)
