#!/bin/bash
g++ -std=c++17 -Wall -Wextra \
    src/robot.cpp src/task.cpp src/fleet.cpp \
    src/mobile_robot.cpp src/cleaning_robot.cpp src/cooking_robot.cpp \
    main.cpp -I include -lpthread -o fleet_app