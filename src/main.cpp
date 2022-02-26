#include <cstdlib>

#include <iostream>
#include <stdexcept>

#include "application.hpp"
#include "cli.hpp"

using namespace lmail;

int main(int /*argc*/, char const* /*argv*/[])
try {
    Cli(Application::parse_conf()).run();
    return EXIT_SUCCESS;
} catch (std::exception const& ex) {
    std::cerr << "error occurred: " << ex.what() << '\n';
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "unknown exception\n";
    return EXIT_FAILURE;
}
