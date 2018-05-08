cmake_minimum_required(VERSION 3.8)
project(EntitySystemTest1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        .settings/language.settings.xml
        .cproject
        .project
        Entity.cpp
        Entity.h
        Entity_test.cpp
        EntityHandle.cpp
        EntityHandle.h
        EntityHandle_test.cpp
        Manager.cpp
        Manager.h
        Manager_test.cpp)

add_executable(EntitySystemTest1 ${SOURCE_FILES})