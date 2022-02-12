#include "readline.hpp"

#include <cstdlib>
#include <cstring>

#include <iterator>
#include <string_view>

#include "boost/range/algorithm/remove_copy_if.hpp"

#include "readline/history.h"
#include "readline/readline.h"

#include "types.hpp"

using namespace lmail;

namespace
{

char* completion_generator(const char* text, int state)
{
    static cmds_names_t matches;
    static size_t match_idx = 0;
    if (0 == state) {
        matches.clear();
        match_idx                = 0;
        std::string_view textstr = text;
        // clang-format off
        boost::remove_copy_if(Readline::instance().cmds(), std::back_inserter(matches), [&textstr](auto const &pattern) {
            return pattern.size() < textstr.size() || pattern.compare(0, textstr.size(), textstr) != 0;
        });
        // clang-format on
    }
    return match_idx < matches.size() ? strdup(matches[match_idx++].c_str()) : nullptr;
}

char** completer(const char* text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, completion_generator);
}

} // namespace

Readline& Readline::instance()
{
    static Readline rl{completer};
    return rl;
}

bool Readline::operator()(std::string& user_input, std::string_view prompt /* = {}*/)
{
    if (auto* input = readline(prompt.data())) {
        user_input = input;
        std::free(input);
        if (!user_input.empty())
            add_history(user_input.c_str());
        return true;
    }
    return false;
}
