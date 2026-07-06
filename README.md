# Robotics HW2 — Robot Fleet Manager

A small C++ console app that manages a fleet of robots (mobile, cleaning,
cooking), assigns tasks with priority, and can run timed background work
using threads.

## Class structure

```
Robot (abstract base)
├── MobileRobot
│   └── CleaningRobot
└── CookingRobot

Fleet   -- manages robots (unordered_map) and a task queue (priority_queue)
Task    -- simple struct describing a job, ordered by priority
```

## How to compile

```bash
./compile.sh
```

This runs:
```bash
g++ -std=c++17 -Wall -Wextra \
    src/robot.cpp src/task.cpp src/fleet.cpp \
    src/mobile_robot.cpp src/cleaning_robot.cpp src/cooking_robot.cpp \
    main.cpp -I include -lpthread -o fleet_app
```

## How to run

```bash
./fleet_app
```

You'll get an interactive menu:

```
=== Robot Fleet Manager ===
1. Add robot
2. Remove robot
3. Show all robots
4. Work (single robot)
5. Work all
6. Charge all
7. Assign task to robot
8. Show task queue
9. Start timed work on a robot
0. Exit
```

## Problems I ran into and how I solved them

- **Reading input with brackets/wrong types silently broke `std::cin`.**
  When a `>>` read fails (e.g. typing text where a number is expected),
  `cin` doesn't throw — it just sets a fail flag and leaves the variable
  unchanged, and every future read fails too until you `cin.clear()` and
  `cin.ignore(...)`. I fixed this by validating input right after reading
  it instead of assuming it worked.

- **`work()` only checked `battery_ == 0`, not whether there was *enough*
  battery for the job.** A robot with battery `1` could still "successfully"
  run `work()` (which costs 20%), silently go negative, and get clamped to
  0 with no error. Fixed by checking `battery_ < cost` *before* subtracting,
  so it throws instead of quietly finishing with too little charge.

- **`Fleet::add` silently overwrote a robot if you reused an existing id**,
  since `unordered_map::operator[]` just replaces the value at that key
  with no warning. Fixed by checking `robots_.count(id)` first and throwing
  if the id is already taken.

- **Menu output disappeared before I could read it**, since the screen
  cleared and reprinted immediately after each action. Fixed by adding a
  "Press Enter to continue..." pause after every menu action, before
  clearing the screen for the next loop.

## Design choices

- **`shared_ptr<Robot>` in `Fleet`, not `unique_ptr`** — `Fleet::find()`
  needs to return a copy of the stored pointer to the caller while the
  fleet keeps its own copy too. `unique_ptr` can't be copied, only moved,
  so `find()` wouldn't compile with it.
- **`unordered_map<string, shared_ptr<Robot>>` for `robots_`** — O(1)
  average lookup by id, versus O(n) for a `vector` you'd have to scan.
- **`priority_queue<Task>` for the task queue** — built on a binary heap,
  `top()` always gives the highest-priority task in O(1), `pop()`/`push()`
  are O(log n). `Task::operator<` compares by `priority`, so the natural
  ordering already puts the highest priority on top.
- **No generic `set_battery()` setter** — battery is only ever changed
  through meaningful actions (construction, `charge()`, `work()`), each of
  which knows exactly how to clamp/validate it. A public setter would let
  anyone assign an invalid value directly.