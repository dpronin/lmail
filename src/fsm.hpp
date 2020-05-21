#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

namespace lmail
{
template <typename StateT>
class Fsm
{
protected:
    explicit Fsm(std::shared_ptr<StateT> init_state) : cstate_(std::move(init_state))
    {
        if (!cstate_)
            throw std::invalid_argument("cstate_ provided cannot be empty");
    }

public:
    ~Fsm() = default;

    void change_state(std::shared_ptr<StateT> new_state)
    {
        auto pstate = std::move(cstate_);
        pstate->OnExit();
        cstate_ = std::move(new_state);
        cstate_->OnEnter();
        pstate_ = std::move(pstate);
        on_state_changed();
    }

    template <typename T>
    std::enable_if_t<std::is_base_of_v<StateT, T>, bool>
    is_in_state() const noexcept { return dynamic_cast<T const*>(cstate_.get()); }

protected:
    virtual void on_state_changed() { }

protected:
    std::shared_ptr<StateT> pstate_;
    std::shared_ptr<StateT> cstate_;
};

} // namespace lmail
