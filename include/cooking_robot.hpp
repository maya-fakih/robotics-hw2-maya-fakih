#pragma once
#include "robot.hpp"

class CookingRobot : public Robot {
public:
    CookingRobot(const std::string& id, const std::string& name, int battery);

    void        work() override;   // cooks, uses battery
    std::string type() const override;

    //similar to cleaning robot but inherits robot
    //it does not spawn background work tasks using start_work
    //the rest of the attributes are inherited normally from robot.hpp
};