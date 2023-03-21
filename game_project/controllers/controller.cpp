#include "controller.h"

Models::Model *Human::model = nullptr;

int Human::f_onkey(int ch)
{
    switch (ch)
    {
        case 'W':
        case 'w':

            break;
        case 'A':
        case 'a':

            break;
        case 'S':
        case 's':

            break;
        case 'D':
        case 'd':

            break;
        default:
            break;
    }

    return ch;
}

void Human::setModel(Models::Model &m_)
{
    model = &m_;
}