#pragma once
#include "robot.hpp"
#include "task.hpp"
#include <memory>
#include <string>
#include <ostream>

class Fleet {
public:
    void add(std::shared_ptr<Robot> robot);
    void remove(const std::string& id);
    std::shared_ptr<Robot> find(const std::string& id) const;

    // Stores the task in the priority queue and marks the robot as busy.
    void assign_task(const std::string& robot_id, const Task& t);
    void show_tasks()  const;  // prints the task queue in priority order

    void work_all()    const;  // calls work() on every robot
    void charge_all()  const;  // calls charge() on every robot

    std::size_t size()  const;
    bool        empty() const;

    Fleet& operator+=(std::shared_ptr<Robot> robot);
    Fleet& operator-=(const std::string& id);
    friend std::ostream& operator<<(std::ostream& os, const Fleet& f);

private:
    // Choose the right containers and justify each choice in a comment.
    // Hint: robots need fast lookup by id.
    // Hint: tasks must always come out highest priority first.
};
