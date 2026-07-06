#include "fleet.hpp"
#include <iostream>
#include <stdexcept>

// fixed robot fleet to not allow duplicated robot ids as it makes no sense
void Fleet::add(std::shared_ptr<Robot> robot) {
    if (robots_.count(robot->id()) > 0)
        throw std::runtime_error("robot id already exists: "+robot->id() );
    robots_[robot->id()] = robot;
}

void Fleet::remove(const std::string& id) {
    robots_.erase(id);
}

// uses shared pointer to allow us to create copies
// a unique pointer would break it-> second as we canonly have one reference
// with unique pointers
// shared pointers relief us of the delete mess by tracking references and once refs = 0 the instance is deleted

std::shared_ptr<Robot> Fleet::find(const std::string& id) const {
    auto it = robots_.find(id);
    if (it == robots_.end())
        throw std::runtime_error("Robot not found: " + id);
    return it->second;
}

void Fleet::assign_task(const std::string& robot_id, const Task& t) {
    auto robot = find(robot_id);   // throws if id doesn't exist
    tasks_.push(t);
}

void Fleet::show_tasks() const {
    std::priority_queue<Task> copy = tasks_;  // don't destroy the real queue
    while (!copy.empty()) {
        std::cout << copy.top() << "\n";
        copy.pop();
    }
}

void Fleet::work_all() const {
    for (const auto& [id, robot] : robots_) robot->work();
}

void Fleet::charge_all() const {
    for (const auto& [id, robot] : robots_) robot->charge();
}

std::size_t Fleet::size()  const { return robots_.size(); }
bool        Fleet::empty() const { return robots_.empty(); }

Fleet& Fleet::operator+=(std::shared_ptr<Robot> robot) {
    add(robot);
    return *this;
}

Fleet& Fleet::operator-=(const std::string& id) {
    remove(id);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Fleet& f) {
    for (const auto& [id, robot] : f.robots_) os << *robot << "\n";
    return os;
}