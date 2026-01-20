#include <chrono>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;

enum class Arg { Days, Weeks, Months, Years };

// Allows us to shorthand the function pointer type
using ApplyFunc = void(*)(chrono::time_point<chrono::system_clock>&, int); // & -> pass by reference, edit the original time_point

/*
    Using a constexpr array to map Arg enum to corresponding functions

    Benefits of constexpr array:
    1. Compile-time initialization: The mapping is established at compile time, leading to potential performance improvements.
    2. Simplicity: The array structure is straightforward and easy to understand.
    3. No runtime overhead: Unlike unordered_map, there is no runtime overhead for lookups since the mapping is fixed and known at compile time.

    Alternatives:
    1. std::unordered_map: While it provides flexibility and dynamic mapping, it incurs runtime overhead for lookups and requires more complex initialization.
    2. std::map: Similar to unordered_map but with ordered keys, it also has runtime overhead and is more complex to set up.
    3. Switch-case statements: While simple, they can become cumbersome and less maintainable as the number of cases increases.

    Beware:
    1. Maintainability: Adding new arguments requires updating the array, which is less flexible than dynamic structures.
    2. Limited size: The array size is fixed at compile time, so it cannot accommodate dynamic additions.
*/
constexpr ApplyFunc ArgFuncs[] = {
    // Days
    [](auto& tp, int value) { tp += chrono::hours(24 * value); }, 
    // Weeks
    [](auto& tp, int value) { tp += chrono::hours(24 * 7 * value); },
    // Months
    [](auto& tp, int value) { 
        auto dt = chrono::system_clock::to_time_t(tp);
        tm* timeinfo = localtime(&dt);
        timeinfo->tm_mon += value;
        dt = mktime(timeinfo);
        tp = chrono::system_clock::from_time_t(dt);
    },
    // Years
    [](auto& tp, int value) { 
        auto dt = chrono::system_clock::to_time_t(tp);
        tm* timeinfo = localtime(&dt);
        timeinfo->tm_year += value;
        dt = mktime(timeinfo);
        tp = chrono::system_clock::from_time_t(dt);
    },
};

// Map argument names to Arg enum values
const unordered_map<string, Arg> ArgNameMap = {
    {"--days", Arg::Days},
    {"--weeks", Arg::Weeks},
    {"--months", Arg::Months},
    {"--years", Arg::Years},
};

constexpr int argToIndex(Arg arg) {
    return static_cast<int>(arg); // Assuming Arg enum values are sequential starting from 0, will return 0 for Days, 1 for Weeks, etc.
};

unordered_map<Arg, int> parseArgs(int argc, char* argv[]) {
    unordered_map<Arg, int> inputs;

    // start at 1 to skip the program name
    for (int i = 1; i < argc; i++) {

        // check to see if the argument is in the ArgNameMap
        string argName = argv[i];
        auto it = ArgNameMap.find(argName);
        
        if (it == ArgNameMap.end()) {
            throw invalid_argument("Unknown argument: " + argName);
        }

        Arg arg = it->second; 
        int value = stoi(argv[++i]); // convert string to int
        
        inputs[arg] = value;
    }

    return inputs;
}

int main(int argc, char* argv[]) {
    chrono::time_point<chrono::system_clock> today = chrono::system_clock::now();

    try
    {
        auto input = parseArgs(argc, argv);

        for (const auto& [arg, value] : input) {
            int index = argToIndex(arg);
            ArgFuncs[index](today, value); // Call the corresponding function
        }

        time_t result_time = chrono::system_clock::to_time_t(today);
        cout << "Resulting date: " << ctime(&result_time);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    

    return 0;
}