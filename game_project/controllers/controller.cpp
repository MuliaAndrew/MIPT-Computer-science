#include "controller.h"

Models::Model* Controller::model = nullptr;

Controller* Controller::get(const std::string& s)
{
    Controller* obj = nullptr;
    
    if (s == "AI")
        auto obj = new AI;
    
    else if (s == "Human")
        auto obj = new Human;
    
    return obj;
}

void Controller::setModel(Models::Model& _m)
{
    model = &_m;
}

int Human::fonKey(char ch)
{
    switch (ch)
    {
        case 'W':
        case 'w':
            snake->move(model->getWinSZ(), model, Models::Dir::up);
            return 0;
        case 'A':
        case 'a':
            snake->move(model->getWinSZ(), model, Models::Dir::left);
            return 0;
        case 'S':
        case 's':
            snake->move(model->getWinSZ(), model, Models::Dir::down);
            return 0;
        case 'D':
        case 'd':
            snake->move(model->getWinSZ(), model, Models::Dir::right);
            return 0;
        case '\e':
            return 1;
    }

    if (!model->isOnPause)
    {
        snake->move(model->getWinSZ(), model, Models::Dir::def);
        return 0;
    }
    return 1;
}

int AI::fonKey(char ch)
{
    return ch;
}

void Controller::setSnake(Models::Snake* s_)
{
    snake = s_;
}