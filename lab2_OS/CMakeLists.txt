cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("lab2_OS")

add_executable (lab2_OS "lab2_OS.cpp" "lab2_OS.h" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lab2_OS PROPERTY CXX_STANDARD 20)
endif()

add_executable(MinMaxTest tests/test_min_max.cpp)
target_link_libraries(MinMaxTest ${Boost_LIBRARIES})
add_test(NAME MinMaxTest COMMAND MinMaxTest)