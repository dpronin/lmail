#include "cli.hpp"

#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "boost/range/algorithm/remove_copy_if.hpp"

#include "states/init_state.hpp"
#include "states/main_state.hpp"

#include "color.hpp"
#include "readline.hpp"
#include "storage.hpp"
#include "types.hpp"

using namespace lmail;

namespace
{

char *completion_generator(const char *text, int state);

char **completer(const char *text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, completion_generator);
}

Readline g_reader;

char *completion_generator(const char *text, int state)
{
    static cmds_t matches;
    static size_t match_idx = 0;
    if (0 == state)
    {
        matches.clear();
        match_idx                = 0;
        std::string_view textstr = text;
        // clang-format off
        boost::remove_copy_if(g_reader.cmds(), std::back_inserter(matches), [&textstr](auto const &pattern) {
            return pattern.size() < textstr.size() || pattern.compare(0, textstr.size(), textstr) != 0;
        });
        // clang-format on
    }
    return match_idx < matches.size() ? strdup(matches[match_idx++].c_str()) : nullptr;
}

} // anonymous namespace

Cli::Cli(Application::Conf const &conf) : conf_(conf), ctx_(g_reader), fsm_(ctx_)
{
    g_reader.init(completer);
}

void Cli::run()
{
    using namespace boost::sml;
    std::cout << "Welcome to " << cbrown("lmail") << '!' << std::endl;
    fsm_.process_event(sm::ev::start{std::make_shared<MainState>(fsm_, std::make_shared<Storage>(conf_.db_path))});
    for (user_input_t user_input; !fsm_.is("idle"_s) && g_reader(user_input, ctx_.prompt());)
    {
        args_t             args;
        std::istringstream iss{std::move(user_input)};
        // clang-format off
        std::remove_copy_if(std::istream_iterator<arg_t>(iss),
                            std::istream_iterator<arg_t>(),
                            std::back_inserter(args),
                            [](auto const &arg) { return arg.empty(); });
        // clang-format on
        if (!args.empty())
            ctx_.process(std::move(args));
    }
    std::cout << "Quitting " << cbrown("lmail") << ". Bye!" << std::endl;
}
