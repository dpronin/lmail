#include <iostream>
#include <stdexcept>

#include "application.hpp"
#include "cli.hpp"

using namespace lmail;

int main(int argc, char const *argv[])
try
{
    Cli(Application::parse_conf()).run();
    return 0;
}
catch (std::exception const &ex)
{
    std::cerr << "error occurred: " << ex.what() << '\n';
}
catch (...)
{
    std::cerr << "unknown exception\n";
}
