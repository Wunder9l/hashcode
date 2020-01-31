cmake_minimum_required(VERSION 3.15)
project(cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp
        main.cpp
        models.h
        checker.h
        checker.cpp
        simulator.h
        simulator.cpp
        util.h
        util.cpp
        annealing.h
        annealing_simulator.h
        )