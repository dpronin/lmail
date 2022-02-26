#pragma once

namespace lmail
{

class ICmd
{
public:
    ICmd()          = default;
    virtual ~ICmd() = default;

    ICmd(ICmd const&)            = default;
    ICmd& operator=(ICmd const&) = default;

    ICmd(ICmd&&) noexcept            = default;
    ICmd& operator=(ICmd&&) noexcept = default;

    virtual void exec() = 0;
};

} // namespace lmail
