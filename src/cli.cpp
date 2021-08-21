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

#include "readline/history.h"
#include "readline/readline.h"

#include "states/init_state.hpp"
#include "states/main_state.hpp"

#include "color.hpp"
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

class Readline
{
public:
    bool operator()(std::string &user_input, std::string_view prompt = {})
    {
        if (auto *input = readline(prompt.data()))
        {
            user_input = input;
            free(input);
            if (!user_input.empty())
                add_history(user_input.c_str());
            return true;
        }
        return false;
    }

    void init() { rl_attempted_completion_function = completer; }

    void        set_cmds(cmds_t cmds) { cmds_ = std::move(cmds); }
    auto const &cmds() const noexcept { return cmds_; }

private:
    cmds_t cmds_;
} g_reader;

char *completion_generator(const char *text, int state)
{
    static cmds_t matches;
    static size_t match_idx = 0;
    if (0 == state)
    {
        matches.clear();
        match_idx                = 0;
        std::string_view textstr = text;
        boost::remove_copy_if(g_reader.cmds(), std::back_inserter(matches), [&textstr](auto const &pattern) {
            return pattern.size() < textstr.size() || pattern.compare(0, textstr.size(), textstr) != 0;
        });
    }
    return match_idx < matches.size() ? strdup(matches[match_idx++].c_str()) : nullptr;
}
} // anonymous namespace

Cli::Cli(Application::Conf const &conf) : Fsm(std::make_shared<InitState>()), conf_(conf)
{
    g_reader.init();
}

void Cli::on_state_changed()
{
    g_reader.set_cmds(cstate_->commands());
}

void Cli::run()
{
    std::cout << "Welcome to " << cbrown("lmail") << '!' << std::endl;
    change_state(std::make_shared<MainState>(*this, std::make_shared<Storage>(conf_.db_path)));
    for (user_input_t user_input; !is_in_state<InitState>() && g_reader(user_input, cstate_->prompt());)
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
            cstate_->process(std::move(args));
    }
    std::cout << "Quitting " << cbrown("lmail") << ". Bye!" << std::endl;
}
