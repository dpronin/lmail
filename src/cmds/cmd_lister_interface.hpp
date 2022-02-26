#pragma once

#include "types.hpp"

namespace lmail
{

class ICmdLister
{
public:
    ICmdLister()          = default;
    virtual ~ICmdLister() = default;

    ICmdLister(ICmdLister const&) = default;
    ICmdLister& operator=(ICmdLister const&) = default;

    ICmdLister(ICmdLister&&) noexcept = default;
    ICmdLister& operator=(ICmdLister&&) noexcept = default;

    [[nodiscard]] virtual cmds_names_t cmds() const = 0;
};

} // namespace lmail
