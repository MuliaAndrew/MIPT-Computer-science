#pragma once

#include <list>
#include <algorithm>
#include <vector>
#include <set>
#include <termios.h>
#include <sys/ioctl.h>
#include <climits>

namespace Models
{
    enum Dir
    {
        def,
        up      = 1,
        down    = -1,
        left    = 2,
        right   = -2
    };

    // first in pair - x, second - y. x is used to define height, y - width
    using Coord = std::pair<unsigned int, unsigned int>;

    class Rabit
    {
            Coord c;
        
        public:
            Rabit(unsigned int x = 0, unsigned int y = 0): c(x, y) {}
            Rabit(Coord c_): c(c_) {}
            ~Rabit() {}

            Coord& coords();
    };

    class Snake
    {
            std::list<Coord> c {};
            Dir prev_d = right;
        
        public:
            Snake(unsigned int x = 0, unsigned int y = 0) { c.push_front({x, y}); }
            Snake(Coord c_ = {0, 0}) { c.push_front(c_); }
            Snake(): c() {}
            ~Snake() { c.clear(); }

            void move(winsize sz, Dir d = def, bool grow = false);

            std::__cxx11::list<Models::Coord>::iterator coords_begin();
            std::__cxx11::list<Models::Coord>::iterator coords_end();
    };

    class Model
    {
            std::list<Snake> snakes {};
            std::list<Rabit> rabits {};

            useconds_t loop_period = 1000000;

            bool isSnakeOnRabit(Snake& snake, Rabit& rabit);

        public:
            Model(): loop_period(), snakes(), rabits() {}
            ~Model() { snakes.clear(); rabits.clear(); }

            void update();

            void setLoopPeriod(useconds_t);
            useconds_t getLoopPeriod();

            Snake* createSnake(Coord);
            Rabit* createRabit(Coord);
            
            void removeSnake(std::list<Models::Snake>::iterator &);
            void removeRabit(std::list<Models::Rabit>::iterator &);
            
            std::list<Models::Snake>::iterator snakes_begin();
            std::list<Models::Snake>::iterator snakes_end();

            std::list<Models::Rabit>::iterator rabits_begin();
            std::list<Models::Rabit>::iterator rabits_end();
    };
}