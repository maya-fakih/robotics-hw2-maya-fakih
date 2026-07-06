# Answers

## 1. Why `shared_ptr` in Fleet and not `unique_ptr`? What would break if you switched?

`Fleet::find(id)` returns a pointer to the caller, but the fleet itself
also keeps its own copy in `robots_`. That means the same robot needs to
be owned by two places at once: the map, and whoever called `find()`.

`shared_ptr` allows this — it counts how many owners point to the same
object and only deletes it when the count hits zero. `unique_ptr` does
not allow this — it only ever has one owner and cannot be copied, only
moved.

If `robots_` were `unordered_map<string, unique_ptr<Robot>>`, this exact
line in `find()` would fail to compile:
```cpp
return it->second;
```
because returning it means making a copy of the pointer, and `unique_ptr`
can't be copied — only moved. Moving it would also be wrong here, since
that would strip ownership away from the map, leaving `robots_` holding
a null pointer for that robot.

## 2. What container did you use for `robots_`? Why — what is its lookup cost?

`std::unordered_map<std::string, std::shared_ptr<Robot>>`, keyed by robot
id. Lookup is O(1) on average (hash table), versus O(n) for a `vector`
where you'd have to loop through every robot comparing ids. Since
`find()`, `assign_task()`, and menu actions all look robots up by id
constantly, this matters.

## 3. What container did you use for the task queue? What data structure is it built on, and why is it the right fit?

`std::priority_queue<Task>`. It's built on a binary heap (usually backed
by a `vector` internally, but that's hidden from us). `top()` always
gives the "biggest" element by `operator<` in O(1), and `push`/`pop` are
O(log n). Since `Task::operator<` compares by `priority` in the natural
direction (`priority < other.priority`), the heap's notion of "biggest"
lines up with "highest priority", so `top()` always gives the
highest-priority task first — exactly what we need.

An `unordered_map` wouldn't work here because it has no ordering at all —
it's built for looking things up by key, not for repeatedly pulling out
the "next most important" item.

## 4. Why must `operator<<` be a friend, not a member?

`std::cout << robot` really means `operator<<(std::cout, robot)`. The
left-hand argument is `std::cout` (an `ostream`), not our `Robot`. Member
operator functions always take the object before the dot as the implicit
first argument (`this`), so if `operator<<` were a member of `Robot`,
we'd have to write `robot << std::cout`, which is backwards from normal
usage.

So it has to be a free function living outside the class. But as a free
function it can't normally see `Robot`'s protected members (`id_`,
`name_`, `battery_`, `status_`). Declaring it `friend` inside `Robot`
gives this one specific function permission to reach in anyway, without
making it an actual member.

## 5. What does `std::find_if` with a lambda communicate compared to a hand-written loop?

It communicates intent directly: "I'm searching for the first element
matching this condition," rather than making the reader trace a manual
loop with an index/iterator, an if-check, and a break, to figure out
what it's doing. It also removes an entire category of off-by-one/loop
bugs since the STL implementation is already correct and tested.

## 6. `work()` throws if battery is too low. Why an exception rather than returning false / printing an error?

Because `work()`'s return type is `void` — there's no return value to
check, so a caller could easily forget to check anything and just
assume it worked. An exception forces the problem to be handled (or it
crashes the program loudly), instead of silently continuing with a
robot that didn't actually do its job. It also lets the error message
carry specific details (`name_`, current battery) up to whichever layer
actually knows how to react — the `Fleet`, or `main.cpp`'s menu — without
every intermediate function needing to manually check and forward a
bool/error code.

## 7. Is the fleet's state consistent after `assign_task` throws for an unknown robot?

Yes. `assign_task` calls `find(robot_id)` first, and `find` throws
*before* anything else happens — the task is only pushed onto `tasks_`
after `find` has already succeeded:
```cpp
void Fleet::assign_task(const std::string& robot_id, const Task& t) {
    auto robot = find(robot_id);   // throws here if not found
    tasks_.push(t);                // never reached if it throws
}
```
So if the robot doesn't exist, the function exits via the exception
before `tasks_.push(t)` ever runs — no partial task gets added, no
robot state changes. The fleet is left exactly as it was before the call.

## 8. What did you do with `stop_` and `worker_`, and why?

`worker_` holds the actual background `std::thread` that `start_work()`
launches. It runs a loop calling `work()` once per second for the given
number of seconds, printing the robot's status after each call.

`stop_` is an `std::atomic<bool>` flag checked inside that loop
(`while (i < seconds && !stop_)`). It's atomic because it's read from
the background thread and could, in principle, be written from another
thread too — a plain `bool` shared across threads without synchronization
is a data race (undefined behavior), so `atomic<bool>` makes reading and
writing it safe. Before starting a new background run, I also call
`worker_.join()` if it's still joinable, so a previous run is always
cleaned up before starting a new one on the same robot.

## 9. Diamond inheritance

```cpp
class Robot { public: std::string name_ = "base"; };
class MobileRobot      : public Robot {};
class CookingRobot     : public Robot {};
class MaintenanceRobot : public MobileRobot, public CookingRobot {};

int main() {
    MaintenanceRobot m;
    std::cout << m.name_;   // which name_?
}
```

This doesn't compile because `m.name_` is ambiguous. `MaintenanceRobot`
inherits from both `MobileRobot` and `CookingRobot`, and each of those
independently inherits its own full copy of `Robot` (since plain
`public Robot` doesn't share the base). So `MaintenanceRobot` actually
contains **two separate `Robot` sub-objects** — one via the
`MobileRobot` path, one via the `CookingRobot` path — and therefore two
separate `name_` members. Writing `m.name_` doesn't tell the compiler
which one you mean, so it refuses to compile.

C++ solves this with **virtual inheritance**:

```cpp
class Robot { public: std::string name_ = "base"; };
class MobileRobot      : public virtual Robot {};
class CookingRobot     : public virtual Robot {};
class MaintenanceRobot : public MobileRobot, public CookingRobot {};

int main() {
    MaintenanceRobot m;
    std::cout << m.name_;   // works: only one Robot sub-object now
}
```

Adding `virtual` to `public virtual Robot` in both `MobileRobot` and
`CookingRobot` tells the compiler: "if something ends up inheriting from
both of us, only keep **one shared copy** of `Robot`, not one per path."
So `MaintenanceRobot` now contains exactly one `Robot` sub-object, shared
between both parent paths, and `m.name_` unambiguously refers to that
single copy.