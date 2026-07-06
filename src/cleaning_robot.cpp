#include "cleaning_robot.hpp"
#include <stdexcept>

CleaningRobot::CleaningRobot(const std::string& id, const std::string& name,
                              int battery, double speed)
    : MobileRobot(id, name, battery, speed) {}

void CleaningRobot::work() {
    if (battery_ < 30)
        throw std::runtime_error(name_ + " cannot work: battery too low");
    battery_ -= 30;   //"uses 30% battery"
    if (battery_ < 0) battery_ = 0;
    status_ = "cleaning";
}

std::string CleaningRobot::type() const { return "CleaningRobot"; }