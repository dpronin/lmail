#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include "cli.hpp"
#include "cli_states.hpp"
#include "storage.hpp"
#include "utility.hpp"

using namespace lmail;

Cli::Cli(Application::Conf const &conf) : Fsm(std::make_shared<InitState>(*this)), conf_(conf)
{
}

int Cli::run() noexcept
{
    std::cout << "Welcome to lmail!" << std::endl;
    change_state(std::make_shared<MainState>(*this, std::make_unique<Storage>(conf_.db_path)));
    for (user_input_t user_input; !is_in_state<InitState>() && uread(user_input, cstate_->prompt());)
    {
        args_t args;
        boost::algorithm::split(args, user_input, boost::is_any_of(" "));
        boost::remove_erase_if(args, [](auto &&arg) { return arg.empty(); });
        if (!args.empty())
            cstate_->process(std::move(args));
    }
    std::cout << "Quitting lmail. Bye!" << std::endl;
    return EXIT_SUCCESS;
}
