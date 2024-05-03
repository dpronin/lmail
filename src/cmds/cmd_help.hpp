#pragma once

#include <algorithm>
#include <format>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string_view>
#include <utility>

#include <boost/algorithm/string/join.hpp>

#include "cmd_interface.hpp"
#include "color.hpp"
#include "types.hpp"

namespace lmail
{

class CmdHelp final : public ICmd
{
    std::ostream& out_;
    cmds_t cmds_;
    std::string fmt_;

    static std::string to_green(std::string_view s)
    {
        auto oss{std::ostringstream{}};
        oss << cgreen(s);
        return oss.str();
    }

    static std::string to_blue(std::string_view s)
    {
        auto oss{std::ostringstream{}};
        oss << cblue(s);
        return oss.str();
    }

public:
    explicit CmdHelp(std::ostream& out, cmds_t cmds)
        : out_(out)
        , cmds_(std::move(cmds))
    {

        std::ranges::sort(cmds_, {}, [](auto const& cmd) { return std::get<0>(cmd); });
        auto const getsize       = [](auto const& v) { return std::size(v); };
        auto const proj_cmd_name = [=](auto const& cmd) { return std::get<0>(cmd); };
        auto const proj_cmd_args = [=](auto const& cmd) { return std::get<1>(cmd); };
        auto const getlen_args   = [=](auto const& args) {
            return std::transform_reduce(std::begin(args), std::end(args), std::max(1zu, std::size(args)) - 1, std::plus<>{}, getsize);
        };
        auto const cmds_lens = cmds_ | std::views::transform(proj_cmd_name) | std::views::transform(getsize);

        auto const cmds_align_size = *std::ranges::max_element(cmds_lens) + to_green("").size();
        auto const args_lens       = cmds_ | std::views::transform(proj_cmd_args) | std::views::transform(getlen_args);

        auto os2{std::ostringstream{}};
        os2 << cblue("");

        auto const args_align_size = *std::ranges::max_element(args_lens) + to_blue("").size();

        fmt_ = std::format("{{:<{}}} {{:<{}}} {{}}", cmds_align_size, args_align_size);
    }

    void exec() override
    {
        for (auto const& cmd : cmds_) {
            auto to_const_ref = []<typename T>(T&& x) -> const T& { return x; };
            out_ << std::vformat(
                        fmt_,
                        std::make_format_args(
                            to_const_ref(to_green(std::get<0>(cmd))),
                            to_const_ref(to_blue(boost::algorithm::join(std::get<1>(cmd), " "))),
                            std::get<2>(cmd)))
                 << std::endl;
        }
    }
};

inline cmds_t cmds_with_help(cmds_t cmds)
{
    cmds.push_back({"help", {}, "Shows this help page", {}});
    std::get<3>(cmds.back()) = [cmds](args_t&&) { return std::make_unique<CmdHelp>(std::cout, cmds); };
    return cmds;
}

} // namespace lmail
