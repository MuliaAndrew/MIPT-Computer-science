#pragma once

#include "../models/model.h"
#include "../controllers/controller.h"

#include <iostream>
#include <string>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <functional>

using HumanFonKeyBindType = std::_Bind<int (Human::*(Human *, std::_Placeholder<1>))(int)>;
using AIFonKeyBindType = std::_Bind<int (AI::*(AI *, std::_Placeholder<1>))(int)>;
class View
{
    static View *obj;
    static termios default_term_attrs;

protected:
    static std::vector<HumanFonKeyBindType> HumanfonKeys;
    static std::vector<AIFonKeyBindType> AIfonKeys;
    static Models::Model *model;

public:
    virtual ~View();

    static View *get(const std::string &);
    static void setModel(Models::Model &);
    virtual void draw() = 0;
    virtual winsize getWinSize() = 0;
    void setFonKeyHuman(HumanFonKeyBindType);
    void setFonKeyAI(AIFonKeyBindType);
};
