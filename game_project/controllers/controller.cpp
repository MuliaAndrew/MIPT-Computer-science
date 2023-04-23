#include <map>

#include "controller.h"

Models::Model* Controller::model = nullptr;
Matrix<int> AI::field = {};

template<typename T>
T& Matrix<T>::up_to(size_t x, size_t y)
{
    if (x < 0 || x > m - 1 || y < 0 || y > n - 1)
        return *(new T{});

    if (x != 0)
        return buf[(x - 1) * n + y];
    
    else
        return buf[(m - 1) * n + y];
}

template<typename T>
T& Matrix<T>::down_to(size_t x, size_t y)
{
    if (x < 0 || x > m - 1 || y < 0 || y > n - 1)
        return *(new T{});

    if (x == m - 1)
        return buf[y];
    
    else
        return buf[(x + 1) * n + y];
}

template<typename T>
T& Matrix<T>::left_to(size_t x, size_t y)
{
    if (x < 0 || x > m - 1 || y < 0 || y > n - 1)
        return *(new T{});

    if (y != 0)
        return buf[x * n + y - 1];
    
    else
        return buf[x * n + n - 1];
}

template<typename T>
T& Matrix<T>::right_to(size_t x, size_t y)
{
    if (x < 0 || x > m - 1 || y < 0 || y > n - 1)
        return *(new T{});

    if (y != n - 1)
        return buf[x * n + y + 1];

    else
        return buf[x * n];
}

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

int Human::fonKey(int ctrl_num)
{   
    if (ctrl_num & Models::CtrlBit::PAUSE_BIT)
        return 1;

    if (snake->status == Models::DEAD)
        return 0;

    if (keyset == wasd)
    {
        if (ctrl_num & Models::CtrlBit::WASD_UP)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::up);
            return 0;
        }
        else if (ctrl_num & Models::CtrlBit::WASD_DOWN)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::down);
            return 0;
        }
        else if (ctrl_num & Models::CtrlBit::WASD_LEFT)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::left);
            return 0;
        }
        else if (ctrl_num & Models::CtrlBit::WASD_RIGHT)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::right);
            return 0;
        }
    }
    else
    {
        if (ctrl_num & Models::CtrlBit::ARROW_UP)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::up);
            return 0;
        }

        else if (ctrl_num & Models::CtrlBit::ARROW_DOWN)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::down);
            return 0;
        }

        else if (ctrl_num & Models::CtrlBit::ARROW_LEFT)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::left);
            return 0;
        }

        else if (ctrl_num & Models::CtrlBit::ARROW_RIGHT)
        {
            snake->move(model->getWinSZ(), model, Models::Dir::right);
            return 0;
        }
    }

    snake->move(model->getWinSZ(), model, Models::Dir::def);

    return 0;
}

const int INF = -1;
const int R = 1;
const int S = 2;

int AI::fonKey(int ch)
{
    if (snake->status == Models::DEAD)
        return 0;

    Models::Dir d = calcNextStep();
    snake->move(model->getWinSZ(), model, d);
    return 0;
}

void Controller::setSnake(Models::Snake* s_)
{
    snake = s_;
}

Models::Dir AI::calcNextStep()
{
    using namespace Models;

    if (snake->status == Models::DEAD || model->isOnPause)
        return Models::Dir::def;

    Models::Coord head = *snake->coords_begin();

    Models::Coord nearest_rabbit = {};
    size_t min_dist = -1;

    size_t N = field.getN();
    size_t M = field.getM();

    for (auto r = model->rabbits_begin(), r_end = model->rabbits_end(); r != r_end; r++)
    {
        size_t dist = 0;

        auto r_x = r->coords().first;
        auto r_y = r->coords().second;

        if (r_x > head.first)
        {
            if (r_x - head.first > N / 2)
                dist +=  N - r_x + head.first;
            else
                dist += r_x - head.first;
        }

        else
        {
            if (head.first - r_x > N / 2)
                dist += N - head.first + r_x;
            else
                dist += head.first - r_x;
        }

        if (r_y > head.second)
        {
            if (r_y - head.second > M / 2)
                dist += N - r_y + head.second;
            else
                dist += r_y - head.second;
        }

        else
        {
            if (head.second - r_y > M / 2)
                dist += N - head.second + r_y;
            else
                dist += head.second - r_y;
        }

        if (dist < min_dist)
        {
            min_dist = dist;
            nearest_rabbit = r->coords();
        }
    }

    auto x = nearest_rabbit.first;
    auto y = nearest_rabbit.second;

    std::list<Models::Dir> next_step;

    if (x < head.first)
    {
        if (x - head.first > N / 2)
            next_step.push_back(Dir::up);
        else
            next_step.push_back(Dir::down);
    }
    else
    {

        if (head.first - x > N / 2)
            next_step.push_back(Dir::down);
        else if (head.first != x)
            next_step.push_back(Dir::up);
    }

    if (y < head.second)
    {
        if (y - head.second > M / 2)
            next_step.push_back(Dir::left);
        else
            next_step.push_back(Dir::right);
    }
    else
    {
        if (head.second - y > M / 2)
            next_step.push_back(Dir::right);
        else if (head.second != y)
            next_step.push_back(Dir::left);
    }

    std::map<Dir, bool> dirs_perm = {{Dir::down, 1}, {Dir::up, 1}, {Dir::left, 1}, {Dir::right, 1}};

    if (field.up_to(head.first - 2, head.second - 2) == S)
        dirs_perm[Dir::up] = 0;
    
    if (field.down_to(head.first - 2, head.second - 2) == S)
        dirs_perm[Dir::down] = 0;
    
    if (field.left_to(head.first - 2, head.second - 2) == S)
        dirs_perm[Dir::left] = 0;
    
    if (field.right_to(head.first - 2, head.second - 2) == S)
        dirs_perm[Dir::right] = 0;
    
    // printf("[DIRS_PERM] up : %d, down : %d, left : %d, right : %d\n", dirs_perm[Dir::up], dirs_perm[Dir::down],
    //     dirs_perm[Dir::left], dirs_perm[Dir::right]);
    
    for (auto dir : next_step)
    {
        if (dirs_perm[dir])
            return dir;
    }

    for (auto [dir, perm] : dirs_perm)
    {
        if (perm)
            return dir;
    }

    return Dir::def;
}

void AI::update()
{
    auto sz = model->getWinSZ();

    field = Matrix<int>(sz.ws_row - 1, sz.ws_col - 1);

    for (auto snake = model->snakes_begin(), snake_end = model->snakes_end(); snake != snake_end; snake++)
    {
        for (auto coord = snake->coords_begin(), coord_end = snake->coords_end(); coord != coord_end; coord++)
        {
            field(coord->first - 2, coord->second - 2) = S;
        }
    }

    for (auto rabbit = model->rabbits_begin(), rabbit_end = model->rabbits_end(); rabbit != rabbit_end; rabbit++)
    {
        field(rabbit->coords().first - 2, rabbit->coords().second - 2) = R;
    }
}

void AI::printField()
{
    auto N = field.getN();
    auto M = field.getM();

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%d ", field(i, j));
        
        printf("\n");
    }
}
