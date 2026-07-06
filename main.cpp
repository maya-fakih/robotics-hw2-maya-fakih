#include <iostream>
#include <string>
#include <limits>
#include "fleet.hpp"
#include "mobile_robot.hpp"
#include "cleaning_robot.hpp"
#include "cooking_robot.hpp"

void clear_screen()
{
    std::system("clear");
}

void wait_for_enter()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void print_menu()
{
    std::cout << "\n=== Robot Fleet Manager ===\n"
              << "1. Add robot\n2. Remove robot\n3. Show all robots\n"
              << "4. Work (single robot)\n5. Work all\n6. Charge all\n"
              << "7. Assign task to robot\n8. Show task queue\n"
              << "9. Start timed work on a robot\n0. Exit\nChoice: ";
}

int read_int()
{
    int x;
    while (!(std::cin >> x)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid number, try again: ";
    }
    return x;
}

void handle_add(Fleet& fleet)
{
    std::cout << "Robot type - 1: Mobile, 2: Cleaning, 3: Cooking\n> ";
    int t = read_int();

    std::cout << "Robot id: ";
    std::string id; std::cin >> id;

    std::cout << "Robot name: ";
    std::string name; std::cin >> name;

    std::cout << "Battery (0-100): ";
    int battery = read_int();

    double speed = 1.0;
    if (t == 1 || t == 2) {
        std::cout << "Speed: ";
        std::cin >> speed;
    }

    try {
        if (t == 1) fleet.add(std::make_shared<MobileRobot>(id, name, battery, speed));
        else if (t == 2) fleet.add(std::make_shared<CleaningRobot>(id, name, battery, speed));
        else if (t == 3) fleet.add(std::make_shared<CookingRobot>(id, name, battery));
        else std::cout << "Unknown type.\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handle_remove(Fleet& fleet)
{
    std::cout << "id to remove: ";
    std::string id; std::cin >> id;
    fleet.remove(id);
}

void handle_work_one(Fleet& fleet)
{
    std::cout << "id to work: ";
    std::string id; std::cin >> id;
    try {
        auto r = fleet.find(id);
        r->work();
        std::cout << *r << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handle_work_all(Fleet& fleet)
{
    try { fleet.work_all(); }
    catch (const std::exception& e) { std::cout << "Error: " << e.what() << "\n"; }
}

void handle_assign_task(Fleet& fleet)
{
    std::cout << "robot id, task name, priority: ";
    std::string id, tname; int pri;
    std::cin >> id >> tname >> pri;
    try {
        fleet.assign_task(id, Task{tname, pri, id});
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void handle_timed_work(Fleet& fleet)
{
    std::cout << "id, seconds: ";
    std::string id; int secs;
    std::cin >> id >> secs;
    if (secs < 0) { std::cout << "Seconds cannot be negative.\n"; return; }

    try {
        auto r = fleet.find(id);
        auto mobile = std::dynamic_pointer_cast<MobileRobot>(r);
        if (mobile) mobile->start_work(secs);
        else std::cout << "Not a mobile-type robot.\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void run_choice(int choice, Fleet& fleet, bool& running)
{
    switch (choice) {
        case 1: handle_add(fleet); break;
        case 2: handle_remove(fleet); break;
        case 3: std::cout << fleet; break;
        case 4: handle_work_one(fleet); break;
        case 5: handle_work_all(fleet); break;
        case 6: fleet.charge_all(); break;
        case 7: handle_assign_task(fleet); break;
        case 8: fleet.show_tasks(); break;
        case 9: handle_timed_work(fleet); break;
        case 0: running = false; break;
        default: std::cout << "Unknown choice.\n";
    }
}

int main()
{
    Fleet fleet;
    bool running = true;
    while (running) {
        clear_screen();
        print_menu();
        run_choice(read_int(), fleet, running);
        if (running) wait_for_enter();
    }
    std::cout << "Goodbye.\n";
    return 0;
}