#pragma once

#include <iostream>
#include <utility>

#include "boost/algorithm/string/join.hpp"
#include "boost/format.hpp"
#include "boost/function_output_iterator.hpp"
#include "boost/range/algorithm/copy.hpp"
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

        boost::sort(help_cmds_, [](auto const &item1, auto const &item2) { return std::get<0>(item1) < std::get<0>(item2); });
        auto getsize = [](auto const &item) {
            return std::size(std::get<0>(item)) + boost::accumulate(std::get<1>(item), 0, [](auto sum, auto const &item) { return sum + item.size(); }) + std::size(std::get<1>(item)) - 1;
        };

        size_t align_size = 0;
        for (auto const &item : help_cmds_)
        {
            if (auto const len = getsize(item); len > align_size)
                align_size = len;
        }

        fmt_ = boost::format("%-" + std::to_string(align_size + 1) + "s %s");
    }

    void operator()()
    {
        boost::copy(help_cmds_, boost::make_function_output_iterator([this](auto const &item) {
                        std::cout << (fmt_ % (std::get<0>(item) + ' ' + boost::algorithm::join(std::get<1>(item), " ")) % std::get<2>(item)) << std::endl;
                    }));
    }

    auto const &help_cmds() const noexcept { return help_cmds_; }

private:
    help_cmds_t   help_cmds_;
    boost::format fmt_;
};

} // namespace lmail
