#ifndef PALEOSIMCOMMANDLINE_HH
#define PALEOSIMCOMMANDLINE_HH

#include <string>
#include <unordered_map>

// Simple command line parser
class PaleoSimCommandLine {
public:
    static std::unordered_map<std::string, std::string> Parse(int argc, char** argv);
};

#endif

