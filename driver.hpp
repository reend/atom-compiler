#pragma once

#include <string>
#include <vector>

struct OutputFiles
{
    std::string assembly;
    std::string object;
    std::string executable;
};

class CompilerDriver
{
public:
    void assemble(const std::string& input,
                  const std::string& output);

    void link(const std::string& input,
              const std::string& output);

    OutputFiles createOutputFiles(const std::string& source);

    void cleanup(const OutputFiles& files);

private:
    void run(const std::vector<std::string>& arguments);
};