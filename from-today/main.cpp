#include <chrono>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;
using namespace std::chrono;

enum class Arg : size_t
{
    Days = 0,
    Weeks,
    Months,
    Years,
    Count
};

// Allows us to shorthand the function pointer type
using ApplyFunc = void (*)(std::chrono::sys_days &, int); // & -> pass by reference, edit the original time_point
using sys_days = chrono::time_point<chrono::system_clock, chrono::days>;

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
    [](std::chrono::sys_days &d, int v)
    {
        d += std::chrono::days{v};
    },

    // Weeks
    [](std::chrono::sys_days &d, int v)
    {
        d += std::chrono::days{7 * v};
    },

    [](std::chrono::sys_days &d, int v)
    {
        using namespace std::chrono;
        year_month_day ymd{d};
        ymd += months{v};
        d = sys_days{ymd};
    },

    // Years
    [](std::chrono::sys_days &d, int v)
    {
        using namespace std::chrono;
        year_month_day ymd{d};
        ymd += years{v};
        d = sys_days{ymd};
    },
};

constexpr int argToIndex(Arg arg)
{
    return static_cast<int>(arg); // Assuming Arg enum values are sequential starting from 0, will return 0 for Days, 1 for Weeks, etc.
};

struct Args
{
    int days = 0;
    int weeks = 0;
    int months = 0;
    int years = 0;
};

Args parseArgs(int argc, char *argv[])
{
    Args inputs;

    // start at 1 to skip the program name
    for (int i = 1; i < argc; i++)
    {

        std::string_view argStr = argv[i];

        if (i + 1 >= argc)
        {
            throw runtime_error("Missing value for argument: " + string(argStr));
        }

        if (argStr == "--days")
        {
            inputs.days = stoi(argv[++i]);
        }
        else if (argStr == "--weeks")
        {
            inputs.weeks = stoi(argv[++i]);
        }
        else if (argStr == "--months")
        {
            inputs.months = stoi(argv[++i]);
        }
        else if (argStr == "--years")
        {
            inputs.years = stoi(argv[++i]);
        }
        else
        {
            throw runtime_error("Unknown argument: " + string(argStr));
        }
    }

    return inputs;
}

int main(int argc, char *argv[])
{
    sys_days today = floor<chrono::days>(chrono::system_clock::now());

    try
    {
        if (argc == 2 && string(argv[1]) == "--help")
        {
            cout << "Usage: from-today [--days N] [--weeks N] [--months N] [--years N]\n";
            cout << "Example: from-today --days 5 --weeks 2\n";
            return 0;
        }

        auto input = parseArgs(argc, argv);

        ArgFuncs[argToIndex(Arg::Days)](today, input.days);
        ArgFuncs[argToIndex(Arg::Weeks)](today, input.weeks);
        ArgFuncs[argToIndex(Arg::Months)](today, input.months);
        ArgFuncs[argToIndex(Arg::Years)](today, input.years);

        year_month_day out{today};

        cout << int(out.year()) << "-"
             << unsigned(out.month()) << "-"
             << unsigned(out.day()) << "\n";

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}