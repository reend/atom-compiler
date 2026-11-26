#include "driver.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <cerrno>
#include <filesystem>

void CompilerDriver::run(
    const std::vector<std::string>& arguments)
{
    std::vector<char*> argv;

    for (const auto& argument : arguments)
    {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }

    argv.push_back(nullptr);

    pid_t pid = fork();

    if (pid < 0)
    {
        throw std::runtime_error("Cannot create process");
    }

    if (pid == 0)
    {
        execvp(argv[0], argv.data());
        _exit(127);
    }

    int status;
    pid_t result;

    do
    {
        result = waitpid(pid, &status, 0);
    }
    while (result == -1 && errno == EINTR);

    if (result == -1)
    {
        throw std::runtime_error("Cannot wait for process");
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
    {
        throw std::runtime_error(
            arguments[0] + " failed");
    }
}

void CompilerDriver::assemble(
    const std::string& input,
    const std::string& output)
{
    run({"as", "--64", input, "-o", output});
}

void CompilerDriver::link(
    const std::string& input,
    const std::string& output)
{
    run({"ld", input, "-o", output});
}

OutputFiles CompilerDriver::createOutputFiles(
    const std::string& source)
{
    std::filesystem::create_directories("bin");

    std::filesystem::path path(source);

    std::string name = "bin/" + path.stem().string();

    return OutputFiles
    {
        name + ".s",
        name + ".o",
        name
    };
}

void CompilerDriver::cleanup(const OutputFiles& files)
{
    std::filesystem::remove(files.assembly);
    std::filesystem::remove(files.object);
}