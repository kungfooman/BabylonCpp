# ============================================================================ #
#                       Application name and options                           #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET NodeFastcall)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "App ${TARGET}")

add_executable( NodeFastcall nodefastcall.c)
