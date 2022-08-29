cmake_minimum_required(VERSION 3.23)
project(coffeeOS)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(coffeeOS
        src/main.cpp
        src/coffeeOS.cpp
        src/coffeeOS.h
        src/config.h
        src/user.h
        src/user.cpp
        src/product.cpp
        src/product.h
        src/manager.cpp
        src/manager.h
        src/menu.cpp
        src/menu.h
        src/saver.cpp
        src/saver.h
        )
