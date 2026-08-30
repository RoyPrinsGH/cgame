#include <cstring>
#include <iostream>
#include <span>
#include <string_view>

namespace
{
    constexpr std::string_view VERSION = "0.1.0";

    void printUsage(std::ostream& out)
    {
        out << "cgame-cli " << VERSION << "\n"
            << "\n"
            << "Usage:\n"
            << "  cgame-cli <command> [options]\n"
            << "\n"
            << "Commands:\n"
            << "  help       Show this message\n"
            << "  version    Show the version\n"
            << "\n"
            << "Options:\n"
            << "  -h, --help       Show this message\n"
            << "  -v, --version    Show the version\n";
    }
}

int main(int argc, char** argv)
{
    const std::span<char*> args{argv, static_cast<std::size_t>(argc)};

    if (args.size() < 2)
    {
        printUsage(std::cout);
        return 0;
    }

    const std::string_view command{args[1]};

    if (command == "help" || command == "-h" || command == "--help")
    {
        printUsage(std::cout);
        return 0;
    }

    if (command == "version" || command == "-v" || command == "--version")
    {
        std::cout << VERSION << "\n";
        return 0;
    }

    std::cerr << "cgame-cli: unknown command '" << command << "'\n\n";
    printUsage(std::cerr);
    return 1;
}
