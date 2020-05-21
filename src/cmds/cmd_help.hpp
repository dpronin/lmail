#pragma once

#include <iostream>
#include <utility>

#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

#include "types.hpp"

namespace lmail
{

class CmdHelp
{
public:
    explicit CmdHelp(help_cmds_t help_cmds) : help_cmds_(std::move(help_cmds))
    {
        boost::sort(help_cmds_, [](auto const &h1, auto const &h2){ return std::get<0>(h1) < std::get<0>(h2); });
    }

    void operator()()
    {
        boost::copy(help_cmds_, boost::make_function_output_iterator([](auto const &item){
            std::cout << (boost::format("%-20s %s")
                                        % (std::get<0>(item) + ' ' + boost::algorithm::join(std::get<1>(item), " "))
                                        % std::get<2>(item)) << std::endl;
        }));
    }

private:
    help_cmds_t help_cmds_;
};

} // namespace lmail
