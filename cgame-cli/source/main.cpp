#include <cgame/assets/pak.hpp>
#include <cgame/project/project.hpp>

#include <filesystem>
#include <iostream>
#include <optional>
#include <span>
#include <string>
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
            << "  help              Show this message\n"
            << "  version           Show the version\n"
            << "  pack <target>     Pack assets for a target (client or server)\n"
            << "\n"
            << "Options:\n"
            << "  -h, --help       Show this message\n"
            << "  -v, --version    Show the version\n";
    }

    void printPackUsage(std::ostream& out)
    {
        out << "Usage:\n"
            << "  cgame-cli pack <target>\n"
            << "\n"
            << "Targets:\n"
            << "  client       Pack assets for client\n"
            << "  server       Pack assets for server\n";
    }

    std::optional<cgame::project::pack_mode> parsePackMode(std::string_view target)
    {
        if (target == "client")
            return cgame::project::pack_mode::client;

        if (target == "server")
            return cgame::project::pack_mode::server;

        return std::nullopt;
    }

    std::string_view packModeName(cgame::project::pack_mode mode)
    {
        return mode == cgame::project::pack_mode::client ? "client" : "server";
    }

    int runPack(cgame::project::pack_mode mode)
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

        const auto assetsToPack = project.getAssetsForPacking(mode);

        cgame::assets::pak_builder pakBuilder;

        for (const auto& asset : assetsToPack)
        {
            std::cout << "[ASSETS] registering "
                      << std::filesystem::relative(asset.realFile,
                                                   std::filesystem::current_path())
                      << "\n";
            pakBuilder.add(asset.virtualPath, asset.realFile);
        }

        const std::string pakFile = std::string{packModeName(mode)} + ".cgpak";

        std::cout << "[ASSETS] building \"" << pakFile << "\"..." << "\n";
        pakBuilder.build(pakFile);

        return 0;
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

    if (command == "pack")
    {
        if (args.size() < 3)
        {
            std::cerr << "cgame-cli: pack requires a target\n\n";
            printPackUsage(std::cerr);
            return 1;
        }

        const std::string_view target{args[2]};

        if (target == "help" || target == "-h" || target == "--help")
        {
            printPackUsage(std::cout);
            return 0;
        }

        const auto maybeMode = parsePackMode(target);

        if (!maybeMode.has_value())
        {
            std::cerr << "cgame-cli: unknown pack target '" << target << "'\n\n";
            printPackUsage(std::cerr);
            return 1;
        }

        return runPack(maybeMode.value());
    }

    std::cerr << "cgame-cli: unknown command '" << command << "'\n\n";
    printUsage(std::cerr);
    return 1;
}
