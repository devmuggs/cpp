# From Today CPP

This C++ program calculates a future date based on user input provided via Command Line Arguments. It utilises the `<chrono>` library for date and time manipulation.

## Example Usage

```bash
g++ from-today/mainw.cpp -o from-today.out # Compile the program
./from-today.out --days 10 --weeks 2 --months 1 --years 1 # Run the program with arguments
# Resulting date: Tue Mar 16 20:12:04 2027
```

the arguments are designed to be used partially, so you could just do `./from-today.out --days 5` or `./from-today.out --years 2 --months 3`.

## Points of Learning

- **Chrono Library**: The program demonstrates how to use the `<chrono>` library to handle time points and durations.

- **TS type= Equivalent**: By using `using` to define `ApplyFunc`, the code shows how to create type aliases for function pointers, enhancing code readability. Similar to TypeScript `type=`.

- **enum class**: The use of `enum class` for defining argument types provides strong type safety and scoped enumerations.

- **Lambda Functions**: The program employs lambda functions to define operations for adding days, weeks, months, and years to a date. Broken into the following elements:
    - **Capture List**: The empty capture list `[]` indicates that the lambda does not capture any variables from its surrounding scope.
    - **Parameters**: The parameters `(auto& tp, int value)` specify that the lambda takes a reference to a `time_point` and an integer value.
    - **Function Body**: The body of the lambda contains the logic for modifying the `time_point` based on the specified duration (days, weeks, months, years).

- **ConstExpr Arrays**: By using `constexpr` for the `ArgFuncs` array, the program ensures that the array is evaluated at compile time, we're able to associate each enum value with its corresponding function efficiently. However, this approach has limitations. This I'm least familiar with, so not going to pretend to be an expert.

- **Unordered Maps Aren't O(1)**: I was shocked to learn that `unordered_map` doesn't guarantee O(1) lookups in c++ unlike TypeScript's `Map` or Python's `dict`. This is because C++'s `unordered_map` can degrade to O(n) in the worst-case scenario due to hash collisions.
