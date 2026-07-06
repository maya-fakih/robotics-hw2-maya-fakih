#include "mobile_robot.hpp"
#include <iostream>
#include <chrono>
#include <stdexcept>

MobileRobot::MobileRobot(const std::string& id, const std::string& name,
                          int battery, double speed)
    : Robot(id, name, battery), speed_(speed) {}

void MobileRobot::work() {
    if (battery_ == 0)
        throw std::runtime_error(name_ + " cannot work: battery is empty");
    battery_ -= 20;
    if (battery_ < 0) battery_ = 0;
    status_ = "working";
}

std::string MobileRobot::type() const { return "MobileRobot"; }

void MobileRobot::start_work(int seconds) {
    if (worker_.joinable()) worker_.join();  // clean up any previous run
    stop_ = false;
    worker_ = std::thread([this, seconds]{
        for (int i = 0; i < seconds && !stop_; ++i) {
            try { work(); } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
                break;
            }
            std::cout << *this << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        status_ = "idle";
    });
}