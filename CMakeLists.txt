cmake_minimum_required(VERSION 3.0.0)
project(conveyor_chaos VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# include(CTest)
enable_testing()

file(GLOB SRCS *.cpp *.hpp)

add_executable(conveyor_chaos ${SRCS})
target_link_libraries(conveyor_chaos gtest_main)
