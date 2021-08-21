#pragma once

#include <iostream>
#include <utility>

#include "boost/algorithm/string/join.hpp"
#include "boost/format.hpp"
#include "boost/function_output_iterator.hpp"
#include "boost/range/adaptor/transformed.hpp"
#include "boost/range/algorithm/copy.hpp"
#include "boost/range/algorithm/max_element.hpp"
#include "boost/range/algorithm/sort.hpp"
#include "boost/range/numeric.hpp"

#include "types.hpp"

namespace lmail
{

class CmdHelp final
{
public:
    explicit CmdHelp(help_cmds_t help_cmds = {}) : help_cmds_(std::move(help_cmds))
    {
        if (help_cmds_.empty())
            return;
        // clang-format off
        boost::sort(help_cmds_);
        auto getsize = [](auto const &cmd)
        {
            return std::size(std::get<0>(cmd))
                + boost::accumulate(std::get<1>(cmd), 0, [](auto sum, auto const &cmd) { return sum + cmd.size(); })
                + std::size(std::get<1>(cmd)) - 1;
        };
        // clang-format on
        auto const   help_cmds_lens = help_cmds_ | boost::adaptors::transformed(getsize);
        size_t const align_size     = *boost::range::max_element(help_cmds_lens);
        fmt_                        = boost::format("%-" + std::to_string(align_size + 1) + "s %s");
    }

    void operator()()
    {
        // clang-format off
        boost::copy(help_cmds_, boost::make_function_output_iterator([this](auto const &item) {
            std::cout << (fmt_ % (std::get<0>(item) + ' ' + boost::algorithm::join(std::get<1>(item), " ")) % std::get<2>(item)) << std::endl;
        }));
        // clang-format on
    }

    auto const &help_cmds() const noexcept { return help_cmds_; }

private:
    help_cmds_t   help_cmds_;
    boost::format fmt_;
};

} // namespace lmail
