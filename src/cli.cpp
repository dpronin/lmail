#include "cli.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <ranges>
#include <sstream>
#include <utility>

#include "cli_sm.hpp"

#include "states/main_state.hpp"

#include "readline.hpp"
#include "storage.hpp"
#include "types.hpp"

using namespace lmail;

Cli::Cli(Application::Conf conf)
    : conf_(std::move(conf))
{
}

void Cli::run()
{
    using namespace boost::sml;
    sm::CliCtx ctx{Readline::instance()};
    sm::Cli fsm{ctx};
    fsm.process_event(sm::ev::run{std::make_shared<MainState>(fsm, std::make_unique<Storage>(conf_.db_path))});
    for (user_input_t user_input; !fsm.is(X) && ctx(user_input);) {
        args_t args;
        std::istringstream iss{user_input};
        std::ranges::remove_copy_if(std::views::istream<arg_t>(iss), std::back_inserter(args), &arg_t::empty);
        if (auto cmd = ctx.parse(std::move(args)))
            cmd->exec();
    }
}
