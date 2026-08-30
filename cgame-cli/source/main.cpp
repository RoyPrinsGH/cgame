#include <cgame/assets/pak.hpp>
#include <cgame/project/project.hpp>

#include <cstring>
#include <iostream>
#include <optional>
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
            << "  help         Show this message\n"
            << "  version      Show the version\n"
            << "  pack-client  Pack assets for client\n"
            << "  pack-server  Pack assets for server\n"
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

    if (command == "pack-client")
    {
        const cgame::project::project project{std::filesystem::current_path()};

        const auto maybeProjectDefinition = project.readProjectDefinition();

        if (!maybeProjectDefinition.has_value())
        {
            std::cerr << "project.yaml does not exist in this directory" << std::endl;
            return 2;
        }

        const auto projectDefinition = maybeProjectDefinition.value();

        std::cout << "[META] project name = " << projectDefinition.name << "\n"
                  << "[META] project type = "
                  << (projectDefinition.buildMultiplayer ? "NETWORKED" : "OFFLINE")
                  << "\n";

        const auto assetsToPack =
            project.getAssetsForPacking(cgame::project::pack_mode::client);

        cgame::assets::pak_builder pakBuilder;

        for (const auto& asset : assetsToPack)
        {
            std::cout << "[ASSETS] registering "
                      << std::filesystem::relative(asset.realFile,
                                                   std::filesystem::current_path())
                      << "\n";
            pakBuilder.add(asset.virtualPath, asset.realFile);
        };

        std::cout << "[ASSETS] building \"client.cgpak\"..." << "\n";
        pakBuilder.build("client.cgpak");

        return 0;
    }

    std::cerr << "cgame-cli: unknown command '" << command << "'\n\n";
    printUsage(std::cerr);
    return 1;
}
