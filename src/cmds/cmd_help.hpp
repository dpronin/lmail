#pragma once

#include <ostream>
#include <ranges>
#include <utility>

#include "boost/algorithm/string/join.hpp"
#include "boost/format.hpp"
#include "boost/range/numeric.hpp"

#include "cmd_interface.hpp"
#include "color.hpp"
#include "types.hpp"

namespace lmail
{

class CmdHelp final : public ICmd
{
    std::ostream& out_;
    cmds_t const& cmds_;
    mutable boost::format fmt_;

public:
    explicit CmdHelp(std::ostream& out, cmds_t const& cmds)
        : out_(out)
        , cmds_(cmds)
    {
        // clang-format off
        auto getsize = [](auto const &cmd)
        {
            return std::size(std::get<0>(cmd))
                + boost::accumulate(std::get<1>(cmd), 0, [](auto sum, auto const &cmd) { return sum + cmd.size(); })
                + std::size(std::get<1>(cmd)) - 1;
        };
        // clang-format on
        auto const cmds_lens    = cmds_ | std::views::transform(getsize);
        size_t const align_size = *std::ranges::max_element(cmds_lens) + cgreen("").size() + cblue("").size();
        fmt_                    = boost::format{"%-" + std::to_string(align_size + 1) + "s %s"};
    }

    void exec() override
    {
        for (auto const& cmd : cmds_)
            out_ << (fmt_ % (cgreen(std::get<0>(cmd)) + ' ' + cblue(boost::algorithm::join(std::get<1>(cmd), " "))) % std::get<2>(cmd)) << std::endl;
    }
};

} // namespace lmail
