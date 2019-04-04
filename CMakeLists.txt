cmake_minimum_required(VERSION 3.8)
project(EntitySystemTest1)

set(CMAKE_CXX_STANDARD 17)

set(ECS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ecs/)
set(ECS_TEST_SOURCE_DIRECTORY ${ECS_DIRECTORY}/src/tests/)
set(ECS_TEST_SOURCE_FILES
	${ECS_TEST_SOURCE_DIRECTORY}/Entity_test.cpp
    ${ECS_TEST_SOURCE_DIRECTORY}/EntityHandle_test.cpp
    ${ECS_TEST_SOURCE_DIRECTORY}/Manager_test.cpp
    ${ECS_TEST_SOURCE_DIRECTORY}/System_test.cpp
	${ECS_TEST_SOURCE_DIRECTORY}/main_test.cpp
)

add_executable(unit-tests ${ECS_TEST_SOURCE_FILES})
if(MSVC)
  target_compile_options(unit-tests PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(unit-tests PRIVATE -W -Wall -Wextra -Werror)
endif(MSVC)
target_link_libraries(unit-tests pthread gtest gtest_main)
target_include_directories(unit-tests PUBLIC ${ECS_DIRECTORY}/include/)
target_include_directories(unit-tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})