#include "cooking_robot.hpp"
#include <stdexcept>

CookingRobot::CookingRobot(const std::string& id, const std::string& name, int battery)
    : Robot(id, name, battery) {}

void CookingRobot::work() {
    if (battery_ < 15)
        throw std::runtime_error(name_ + " cannot work: battery too low");
    battery_ -= 15;
    if (battery_ < 0) battery_ = 0;
    status_ = "cooking";
}

std::string CookingRobot::type() const { return "CookingRobot"; }