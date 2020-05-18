#pragma once

#include <iostream>
#include <utility>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/algorithm/sort.hpp>

#include "types.hpp"

namespace lmail
{

class CmdHelp
{
public:
    explicit CmdHelp(std::vector<cmd_t> cmds) : cmds_(std::move(cmds)) { boost::sort(cmds_); }
    void operator()() { std::cout << "commands: " << boost::algorithm::join(cmds_, " ") << std::endl; }

private:
    std::vector<cmd_t> cmds_;
};

} // namespace lmail
