# ###########################################################
# #                                                         #
# #  Exosuit for Fall intervention and Gait phase detection #
# #   Er Jie Kai <erjiekai@gmail.com>                       #
# #                                                         #
# ###########################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
PROJECT(HerkulexTest)

# #################### Enumser Setting ####################
#used for listing all available ports in windows
if (WIN32)
include_directories(${PROJECT_SOURCE_DIR}/Dependencies/enumser)
FILE(GLOB ENUMSER_SOURCES "${PROJECT_SOURCE_DIR}/Dependencies/enumser/*.cpp")
FILE(GLOB ENUMSER_HEADERS "${PROJECT_SOURCE_DIR}/Dependencies/enumser/*.h")
endif (WIN32)

# #################### Code  Compiling ####################
FILE(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")
FILE(GLOB HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.h")
if (UNIX)
  list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/src/serial_stream.cpp)
  list(REMOVE_ITEM HEADERS ${PROJECT_SOURCE_DIR}/src/serial_stream.hpp)
endif (UNIX)


ADD_EXECUTABLE(app_test ${SOURCES} ${HEADERS} ${ENUMSER_SOURCES} ${ENUMSER_HEADERS})
# target_link_libraries(app_test "${PROJECT_SOURCE_DIR}/lib/drApi.lib")

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11") # -fopenmp -march=native -O2 
ADD_DEFINITIONS(${GCC_COVERAGE_COMPILE_FLAGS})

SET_PROPERTY(TARGET app_test PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET app_test PROPERTY CXX_STANDARD_REQUIRED ON)

