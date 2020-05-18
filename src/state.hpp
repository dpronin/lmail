#pragma once

namespace lmail
{
class State
{
public:
    virtual ~State() = default;
    virtual void OnEnter() {}
    virtual void OnExit() {}
};
} // namespace lmail
