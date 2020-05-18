#include <stdexcept>
#include <iostream>

#include "cli.hpp"
#include "application.hpp"

using namespace lmail;

int main(int argc, char const *argv[]) try
{
    return Cli(Application::instance().parse_conf()).run();
}
catch (std::exception const &ex)
{
    std::cerr << "error occurred: " << ex.what() << '\n';
}
catch (...)
{
    std::cerr << "unknown exception\n";
}
