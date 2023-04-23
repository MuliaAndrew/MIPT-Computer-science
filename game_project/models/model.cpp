#include "model.h"

using namespace Models;

Rabbit* Model::createRabbit(Coord c)
{
    auto r = new Rabbit(c);
    rabbits.push_back(*r);
    return &rabbits.back();
}

Snake* Model::createSnake(Coord c)
{
    auto s = new Snake(c);
    snakes.push_back(*s);
    return &snakes.back();
}

void Model::removeRabbit(std::list<Models::Rabbit>::iterator & r_iter)
{
    rabbits.erase(r_iter);
}

void Model::removeSnake(std::list<Models::Snake>::iterator & s_iter)
{
    snakes.erase(s_iter);
}

void Snake::move(winsize sz, Model* model, Dir d)
{   
    if (d == def)
        d = prev_d;
    
    if (d == -prev_d)
        d = prev_d;
    
    auto head = c.front();
    auto new_head = head;
    prev_d = d;

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
        if (new_head.second < 2)
            new_head.second = 2;
    }
    
    c.push_front(new_head);

    for (auto r = model->rabbits_begin(); r != model->rabbits_end(); r++)
    {
        if (isSnakeOnRabbit(*r))
        {
            model->removeRabbit(r);
            model->createRabbitRandom();
            return;
        }
    }

    c.pop_back();
    return;
}

Models::Dir Snake::getHeadDir()
{
    return prev_d;
}

void Snake::setColor(Color color_)
{
    color = color_;
}

Color Snake::getColor()
{
    return color;
}

int Snake::getLenght()
{
    return c.size();
}

std::__cxx11::list<Models::Coord>::iterator Snake::coords_begin()
{
    return c.begin();
}

std::__cxx11::list<Models::Coord>::iterator Snake::coords_end()
{
    return c.end();
}


Coord& Rabbit::coords()
{
    return c;
}

bool Snake::isSnakeOnRabbit(Rabbit& rabbit)
{
    if (*c.begin() == rabbit.coords())
        return true;
    
    return false;
}

bool Snake::cutIfBump(Model* model)
{
    for (auto snake = model->snakes_begin(); snake != model->snakes_end(); snake++)
    {
        auto head = c.begin();
        
        if (&*snake == this)
        {
            auto coord = c.begin();
            coord++;
            for (; coord != c.end(); coord++)
            {
                if (*head == *coord)
                {
                    c.erase(coord, c.end());
                    break;
                }
            }
        }
        else
        {
            for (auto coord = snake->coords_begin(); coord != snake->coords_end(); coord++)
            {
                if (*head == *coord)
                    return 1;
            }
        }
    }
    return 0;
}

Models::Dir Snake::getTailDir(Model* model)
{
    auto tail = c.end(); tail--;
    auto tail_prev = c.end(); tail_prev--; tail_prev--;

    auto sz = model->getWinSZ();
    auto N = sz.ws_row - 3;
    auto M = sz.ws_col - 3;

    if (tail_prev->first - tail->first == 1 || tail->first - tail_prev->first == N)
        return Dir::down;
    else if (tail->first - tail_prev->first == 1 || tail_prev->first - tail->first == N)
        return Dir::up;
    
    if (tail_prev->second - tail->second == 1 || tail->second - tail_prev->second == M)
        return Dir::right;
    else if (tail->second - tail_prev->second == 1 || tail_prev->second - tail->second == M)
        return Dir::left;
    
    return Dir::def;
}

std::pair<Dir, Dir> Snake::getBodyDir(std::__cxx11::list<Coord>::iterator c, Model* model)
{
    auto c_next = c; c_next++;
    auto c_prev = c; c_prev--;

    Dir forward = Dir::def;
    Dir backward = Dir::def;

    auto sz = model->getWinSZ();
    auto N = sz.ws_row - 3;
    auto M = sz.ws_col - 3;

    if (c_next->first - c->first == 1 || c->first - c_next->first == N)
        forward = Dir::down;
    else if (c->first - c_next->first == 1 || c_next->first - c->first == N)
        forward = Dir::up;
    
    if (c_next->second - c->second == 1 || c->second - c_next->second == M)
        forward = Dir::right;
    else if (c->second - c_next->second == 1 || c_next->second - c->second == M)
        forward = Dir::left;
    
    if (c_prev->first - c->first == 1 || c->first - c_prev->first == N)
        backward = Dir::down;
    else if (c->first - c_prev->first == 1 || c_prev->first - c->first == N)
        backward = Dir::up;
    
    if (c_prev->second - c->second == 1 || c->second - c_prev->second == M)
        backward = Dir::right;
    else if (c->second - c_prev->second == 1 || c_prev->second - c->second == M)
        backward = Dir::left;
    
    return {forward, backward};
}

std::list<Models::Snake>::iterator Model::snakes_begin()
{
    return snakes.begin();
}

std::list<Models::Snake>::iterator Model::snakes_end()
{
    return snakes.end();
}

std::list<Models::Rabbit>::iterator Model::rabbits_begin()
{
    return rabbits.begin();
}

std::list<Models::Rabbit>::iterator Model::rabbits_end()
{
    return rabbits.end();
}

void Model::update()
{
    for (auto snake = snakes.begin(); snake != snakes.end(); snake++)
    {
        if (snake->cutIfBump(this))
            snakes.erase(snake);
    }
}

void Model::setLoopPeriod(useconds_t t)
{
    loop_period = t;
}

useconds_t Model::getLoopPeriod()
{
    return loop_period;
}

void Model::setWinSZ(winsize sz_)
{
    sz = sz_;
}

void Model::createRabbitRandom()
{
    std::vector<Coord> busy{};
    
    for (auto snake = snakes.begin(); snake != snakes.end(); snake++)
    {
        for (auto coord = snake->coords_begin(); coord != snake->coords_end(); coord++)
            busy.push_back(*coord);
    }

    for (auto rabbit = rabbits.begin(); rabbit != rabbits.end(); rabbit++)
        busy.push_back(rabbit->coords());
    
    bool isbusy = true;
    Coord new_coord;

    while(isbusy)
    {
        isbusy = false;
        new_coord.first = std::rand() % (sz.ws_row - 3) + 2;
        new_coord.second = std::rand() % (sz.ws_col - 3) + 2;

        for (int i = 0; i < busy.size(); i++)
        {
            if (new_coord == busy[i])
                isbusy = true;
        }
    }

    rabbits.push_back(Rabbit(new_coord));
}

winsize Model::getWinSZ()
{
    return sz;
}