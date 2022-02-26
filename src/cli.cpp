#include "cli.hpp"

#include <algorithm>
#include <exception>
#include <iterator>
#include <memory>
#include <ranges>
#include <sstream>
#include <utility>

#include "sm/cli.hpp"

#include "cmds/cmd_invoker.hpp"

#include "color.hpp"
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
    auto const to_args = [](user_input_t const& user_input) {
        args_t args;
        std::istringstream iss{user_input};
        std::ranges::copy(std::views::istream<arg_t>(iss), std::back_inserter(args));
        return args;
    };
    sm::CliCtx ctx{Readline::instance()};
    for (CmdInvoker invoker{ctx, std::make_unique<Storage>(conf_.db_path)}; invoker;) {
        try {
            invoker(ctx.cmd_parser().parse(to_args(ctx())));
        } catch (std::exception const& ex) {
            std::cerr << cred(ex.what()) << '\n';
        } catch (...) {
            std::cerr << cred("unknown error occurred") << '\n';
            break;
        }
    }
}
