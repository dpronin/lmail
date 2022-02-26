#pragma once

#include "application.hpp"

namespace lmail
{

class Cli final
{
    Application::Conf conf_;

public:
    explicit Cli(Application::Conf conf);
    void run();
};

} // namespace lmail
