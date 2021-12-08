#include <string>
#include <chrono>

using namespace std::chrono;

class stopwatch{
    std::string text;
    time_point<system_clock> now = system_clock::now();
public:
    stopwatch(std::string&& text) : text(text) {}

    ~stopwatch() {
        fmt::print("Operation {} lasted {} seconds\n", text, duration<double>(system_clock::now() - now).count());
    }
};