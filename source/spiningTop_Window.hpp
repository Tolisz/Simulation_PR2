#pragma once

#include "glfwWindowWrap.hpp"

class spiningTop_Window: public glfwWindowWrap
{

private:

    /* virtual */ void RunInit() override;
    /* virtual */ void RunRenderTick() override;
    /* virtual */ void RunClear() override;

};