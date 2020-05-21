#include <cstdlib>
#include <cstring>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <algorithm>
#include <sstream>
#include <iterator>

#include <boost/range/algorithm/remove_copy_if.hpp>

#include "cli.hpp"
#include "cli_states.hpp"
#include "storage.hpp"
#include "utility.hpp"
#include "color.hpp"
#include "types.hpp"

using namespace lmail;

namespace
{
    cmds_t g_completion_cmds;

    char* completion_generator(const char* text, int state)
    {
        static cmds_t matches;
        static size_t match_idx = 0;
        if (0 == state)
        {
            matches.clear();
            match_idx = 0;
            std::string textstr = text;
            boost::remove_copy_if(g_completion_cmds, std::back_inserter(matches), [&textstr](auto const &pattern){
                return pattern.size() < textstr.size() || pattern.compare(0, textstr.size(), textstr) != 0;
            });
        }
        return match_idx < matches.size() ? strdup(matches[match_idx++].c_str()) : nullptr;
    }

    char** completer(const char* text, int start, int end)
    {
        rl_attempted_completion_over = 1;
        return rl_completion_matches(text, completion_generator);
    }

    void init_completer() { rl_attempted_completion_function = completer; }

    bool ureadline(std::string &user_input, std::string_view prompt = {})
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
} // anonymous namespace

Cli::Cli(Application::Conf const &conf) : Fsm(std::make_shared<InitState>(*this)), conf_(conf)
{
    init_completer();
}

void Cli::on_state_changed()
{
    g_completion_cmds = cstate_->commands();
}

void Cli::run()
{
    std::cout << "Welcome to " << colored("lmail", color_e::brown) << '!' << std::endl;
    change_state(std::make_shared<MainState>(*this, std::make_unique<Storage>(conf_.db_path)));
    for (user_input_t user_input; !is_in_state<InitState>() && ureadline(user_input, cstate_->prompt());)
    {
        args_t args;
        std::istringstream iss{std::move(user_input)};
        std::remove_copy_if(std::istream_iterator<std::string>(iss),
                            std::istream_iterator<std::string>(),
                            std::back_inserter(args),
                            [](auto const &arg){ return arg.empty(); });
        if (!args.empty())
            cstate_->process(std::move(args));
    }
    std::cout << "Quitting " << colored("lmail", color_e::brown) << ". Bye!" << std::endl;
}
