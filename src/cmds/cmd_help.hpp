#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <ranges>
#include <stdexcept>
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
    cmds_t cmds_;
    boost::format fmt_;

public:
    explicit CmdHelp(std::ostream& out, cmds_t cmds)
        : out_(out)
        , cmds_(std::move(cmds))
    {
        std::ranges::sort(cmds_, std::less<>{}, [](auto const& cmd) { return std::get<0>(cmd); });
        auto getlen_cmd = [](auto const& cmd) { return std::size(std::get<0>(cmd)); };
        auto getlen_arg = [](auto const& cmd) {
            auto const& args       = std::get<1>(cmd);
            auto const whilespaces = std::max(1zu, std::size(args)) - 1;
            return boost::accumulate(args, 0, [](auto sum, auto const& args) { return sum + std::size(args); }) + whilespaces;
        };
        auto const cmds_lens         = cmds_ | std::views::transform(getlen_cmd);
        size_t const cmds_align_size = *std::ranges::max_element(cmds_lens) + cgreen("").size();
        auto const args_lens         = cmds_ | std::views::transform(getlen_arg);
        size_t const args_align_size = *std::ranges::max_element(args_lens) + cblue("").size();
        fmt_                         = boost::format{"%-" + std::to_string(cmds_align_size) + "s %-" + std::to_string(args_align_size) + "s %s"};
    }

    void exec() override
    {
        /* clang-format off */
        for (auto const& cmd : cmds_)
            out_ << (fmt_ % (cgreen(std::get<0>(cmd)))
                    % (cblue(boost::algorithm::join(std::get<1>(cmd), " ")))
                    % std::get<2>(cmd))
                 << std::endl;
        /* clang-format on */
    }
};

inline cmds_t cmds_with_help(cmds_t cmds)
{
    cmds.push_back({"help", {}, "Shows this help page", {}});
    std::get<3>(cmds.back()) = [cmds](args_t&&) { return std::make_unique<CmdHelp>(std::cout, cmds); };
    return cmds;
}

} // namespace lmail
