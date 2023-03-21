#include "model.h"

using namespace Models;

Rabit* Model::createRabit(Coord c)
{
    auto r = new Rabit(c);
    rabits.push_back(*r);
    return &rabits.back();
}

Snake* Model::createSnake(Coord c)
{
    auto s = new Snake(c);
    snakes.push_back(*s);
    return &snakes.back();
}

void Model::removeRabit(std::list<Models::Rabit>::iterator & r_iter)
{
    rabits.erase(r_iter);
}

void Model::removeSnake(std::list<Models::Snake>::iterator & s_iter)
{
    snakes.erase(s_iter);
}

void Snake::move(winsize sz, Dir d, bool grow)
{   
    if (d == def)
        d = prev_d;
    
    if (d == -prev_d)
        return;
    
    auto head = c.front();
    auto new_head = head;

    if (d == down)
    {
        new_head.first = (new_head.first + 1) % sz.ws_row;
        if (new_head.first < 2)
            new_head.first = 2;
    }
    else if (d == up)
    {
        new_head.first = (new_head.first - 1) % sz.ws_row;
        if (new_head.first < 2)
            new_head.first = sz.ws_row - 1;
    }
    else if (d == left)
    {
        new_head.second = (new_head.second - 1) % sz.ws_col;
        if (new_head.second < 2)
            new_head.second = sz.ws_col - 1;
    }
    else if (d == right)
    {
        new_head.second = (new_head.second + 1) % sz.ws_col;
        new_head.second = 2;
    }
    
    c.push_front(new_head);

    if (!grow)
        c.pop_back();
}

std::__cxx11::list<Models::Coord>::iterator Snake::coords_begin()
{
    return c.begin();
}

std::__cxx11::list<Models::Coord>::iterator Snake::coords_end()
{
    return c.end();
}


Coord& Rabit::coords()
{
    return c;
}

bool Model::isSnakeOnRabit(Snake& snake, Rabit& rabit)
{
    auto equal_c = std::find(snake.coords_begin(), snake.coords_end(), rabit.coords());

    if (equal_c == snake.coords_end())
    {
        if (*equal_c == rabit.coords())
            return true;

        return false;
    }

    return true;
}

std::list<Models::Snake>::iterator Model::snakes_begin()
{
    return snakes.begin();
}

std::list<Models::Snake>::iterator Model::snakes_end()
{
    return snakes.end();
}

std::list<Models::Rabit>::iterator Model::rabits_begin()
{
    return rabits.begin();
}

std::list<Models::Rabit>::iterator Model::rabits_end()
{
    return rabits.end();
}

void Model::update()
{
    ///
}

void Model::setLoopPeriod(useconds_t t)
{
    loop_period = t;
}

useconds_t Model::getLoopPeriod()
{
    return loop_period;
}