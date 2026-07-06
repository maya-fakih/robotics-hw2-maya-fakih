#include "robot.hpp"
#include "task.hpp"

Robot::Robot(const std::string& id, const std::string& name, int battery)
    : id_(id), name_(name), battery_(battery), status_("idle") {
    if (battery_ < 0)   battery_ = 0;
    if (battery_ > 100) battery_ = 100;
}

void Robot::charge() {
    battery_ = 100;
    status_ = "charging";
}

std::string Robot::id() const      { return id_; }
std::string Robot::name() const    { return name_; }
int         Robot::battery() const { return battery_; }
std::string Robot::status() const  { return status_; }

bool Robot::operator==(const Robot& other) const {
    return id_ == other.id_;
}

std::string Robot::operator+(const Robot& other) const {
    return name_ + " + " + other.name_;
}

// it is a friend function to let the compiler know that although it's
// not a class function (1st argument is not this)
// it is allowed to bypass protection and acess internal parms like id, type...
std::ostream& operator<<(std::ostream& os, const Robot& r) {
    os << "[" << r.id_ << "] " << r.name_
       << " (" << r.type() << ") "
       << "battery=" << r.battery_ << "% "
       << "status=" << r.status_;
    return os;
}