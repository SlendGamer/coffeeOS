cmake_minimum_required(VERSION 3.23)
project(coffeeOS)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(coffeeOS
        src/binaries.h
        src/cli.cpp
        src/cli.h
        src/cli_cmd.cpp
        src/coffeeOS.cpp
        src/coffeeOS.h
        src/config.cpp
        src/config.h
        src/const.h
        src/pref.cpp
        src/user_db.cpp)
