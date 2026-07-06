#pragma once
#include "robot.hpp"
#include "mobile_robot.hpp"
#include <thread>
#include <atomic>

class CleaningRobot : public MobileRobot {
public:
    CleaningRobot(const std::string& id, const std::string& name,
                int battery, double speed);

    void        work()  override;  // moves, uses 30% battery
    std::string type()  const override;

    // mobile robot already has all the variables worker stop...
    // start work is the same for all robots what differs is the work functino it is calling

};
