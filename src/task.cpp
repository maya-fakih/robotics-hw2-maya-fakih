#include "task.hpp"

// I dont understand why we are using < to compare and get max :|
// couldnt understand claude explanation on this :/
bool Task::operator<(const Task& other) const {
    return priority < other.priority;
}

std::ostream& operator<<(std::ostream& os, const Task& t) {
    os << "Task[" << t.name << "] priority=" << t.priority
       << " assigned_to=" << t.assigned_to;
    return os;
}