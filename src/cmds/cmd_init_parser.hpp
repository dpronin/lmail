#pragma once

#include <stdexcept>

#include "cmd_parser_interface.hpp"

namespace lmail
{

class CmdInitParser : public ICmdParser
{
public:
    CmdInitParser()           = default;
    ~CmdInitParser() override = default;

    CmdInitParser(CmdInitParser const&) = default;
    CmdInitParser& operator=(CmdInitParser const&) = default;

    CmdInitParser(CmdInitParser&&) noexcept = default;
    CmdInitParser& operator=(CmdInitParser&&) noexcept = default;

    [[nodiscard]] std::unique_ptr<ICmd> parse(args_t /*args*/) override { throw std::logic_error("invalid state to parse command"); }
};

} // namespace lmail
