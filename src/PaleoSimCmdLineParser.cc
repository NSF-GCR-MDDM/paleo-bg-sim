#include "PaleoSimCmdLineParser.hh"

std::unordered_map<std::string, std::string> PaleoSimCommandLine::Parse(int argc, char** argv) {

    std::unordered_map<std::string, std::string> parsedArgs;

    // Skip argv[0] (the executable name)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (i==1) {
          parsedArgs["macro"] = arg;
        }
        else if (arg.rfind("--", 0) == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') { // next arg isn't another flag
                parsedArgs[arg] = argv[i+1];
                i++;
            } 
        }
    }

    return parsedArgs;
}

