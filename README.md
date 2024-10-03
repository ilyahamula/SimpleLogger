
# Logger


This small project demonstrates the functioning of a custom logger.
It includes the logger itself, a demonstration program, and unit tests.

### Rationale Behind Implementation:

1. The class uses static members to ensure only one instance of timers, outputter, etc., exists throughout the program.
2. It provides flexibility in adjusting settings and printing log messages in various formats.
3. The presence of a timestamp allows measuring the execution time of code segments and recording it in the log. 
4. Templated methods allow logging of different types of objects and formatted messages with ease.

### Possible Shortcomings and Known Issues:

1. Lack of detailed error handling: If file operations fail or settings are incorrectly adjusted, there's no explicit error handling mechanism.
2. Potential thread-safety issues: Although a mutex is used for thread safety, the implementation might not be robust enough for all scenarios. Further testing is needed, especially in multi-threaded environments.
3. Dependency on external typeToString function: As C++ currently lacks a reflection mechanism, end-users still need to perform additional work. The printObject method assumes the existence of a typeToString function for object representation, which should be written.

### Ideas for further development:

1. Expanding the logger with functionality to measure virtual memory usage would be beneficial.
2. It would also be beneficial to provide users with more flexible options for configuring the output text format, such as choosing their own tag, color, etc.
3. Allowing the log to display the depth of function calls using indentation is a good idea. However, in a multithreaded program, this could lead to some confusion.
4. Having the ability to create a separate log file for each individual thread would be great, although it might be considered an excessive feature)
5. Utilizing the latest C++ standards can make the code more readable and platform-independent.


## Dependencies ⚙️

For unit tests, gtest is used. However, there is no need to install it as the build process will handle everything automatically.

## Build  🛠️

1. Navigate to the root directory of the repository.

2. Create a build directory and navigate to it:

    ```console
    mkdir build && cd build
    ```

3. Run CMake to generate the build files:

    ```console
    cmake ..
    ```

4. Build the project:

    ```console
    cmake --build . --target Release
    ```

6. The built executable should now be located in the `build` directory.


## Using

After building, you will have two executable files: one for the testing `LoggerUnitTests` and the other for the short demonstration `LoggerExample`.

`LoggerExample` is a simple console application. It can take zero or one input parameter, which is the path to the log file. \
By default, application will send log messages to the standard output (cout). \
If a log file path is provided, the log output will be written to both the specified file and the standard output, as the Logger supports a combination of multiple outputs.