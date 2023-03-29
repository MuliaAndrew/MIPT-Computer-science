#pragma once

#include <list>
#include <algorithm>
#include <vector>
#include <set>
#include <termios.h>
#include <sys/ioctl.h>
#include <string>


namespace Models
{
    class Model;

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

    class Rabbit
    {
            Coord c;
        
        public:
            Rabbit(unsigned int x = 0, unsigned int y = 0): c(x, y) {}
            Rabbit(Coord c_): c(c_) {}
            ~Rabbit() {}

            Coord& coords();
    };

    class Snake
    {
            std::list<Coord> c {};
            Dir prev_d = right;

            bool isSnakeOnRabbit(Rabbit& rabbit);
        
        public:
            Snake(unsigned int x = 0, unsigned int y = 0) { c.push_front({x, y}); }
            Snake(Coord c_ = {0, 0}) { c.push_front(c_); }
            Snake(): c() {}
            ~Snake() { c.clear(); }

            void move(winsize sz, Model* model, Dir d = def);

            std::__cxx11::list<Models::Coord>::iterator coords_begin();
            std::__cxx11::list<Models::Coord>::iterator coords_end();
    };

    class Model
    {
            std::list<Snake> snakes {};
            std::list<Rabbit> rabbits {};

            useconds_t loop_period = 1000000;
            winsize sz = {};

        public:
            bool isOnPause = false;

            Model(): loop_period(), snakes(), rabbits() {}
            ~Model() { snakes.clear(); rabbits.clear(); }

            void update();
            
            void setWinSZ(winsize);
            winsize getWinSZ();

            void setLoopPeriod(useconds_t);
            useconds_t getLoopPeriod();

            Snake* createSnake(Coord);
            Rabbit* createRabbit(Coord);
            
            void removeSnake(std::list<Models::Snake>::iterator &);
            void removeRabbit(std::list<Models::Rabbit>::iterator &);
            
            std::list<Models::Snake>::iterator snakes_begin();
            std::list<Models::Snake>::iterator snakes_end();

            std::list<Models::Rabbit>::iterator rabbits_begin();
            std::list<Models::Rabbit>::iterator rabbits_end();

            void createRabbitRandom();
    };
}