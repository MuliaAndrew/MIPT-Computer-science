#pragma once

#include <list>
#include <algorithm>
#include <vector>
#include <set>
#include <termios.h>
#include <sys/ioctl.h>
#include <string>

#define Debug_OK(num) \
    printf("[OK] %d\n", num)

namespace Models
{
    const bool DEAD = false;
    const bool ALIVE = true;

    class Model;

    enum Color
    {
        white = 37,
        red = 31,
        green = 32,
        blue = 34,
        purple = 35,
        yellow = 33,
        ocean = 36
    };

    enum Dir
    {
        def,
        up      = 1,
        down    = -1,
        left    = 2,
        right   = -2
    };

    enum CtrlBit : int
    {
        PAUSE_BIT = 1 << 15,
        CLOSE_BIT = 1 << 14,
        WASD_UP = 1 << 11,
        WASD_DOWN = 1 << 10,
        WASD_LEFT = 1 << 9,
        WASD_RIGHT = 1 << 8,
        ARROW_UP = 1 << 7,
        ARROW_DOWN = 1 << 6,
        ARROW_LEFT = 1 << 5,
        ARROW_RIGHT = 1 << 4,
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
            std::list<Coord> c{};
            Dir prev_d = right;

            bool isSnakeOnRabbit(Rabbit& rabbit);

            Color color = white;
        
        public:
            bool status = ALIVE;

            Snake(unsigned int x = 0, unsigned int y = 0) { c.push_front({x, y}); }
            Snake(Coord c_ = {0, 0}) { c.push_front(c_); }
            Snake(): c() {}
            ~Snake() { c.clear(); }

            void move(winsize sz, Model* model, Dir d = def);

            void setColor(Color);
            Color getColor();

            int getLenght();
            bool cutIfBump(Model* model);

            Models::Dir getHeadDir();
            std::pair<Models::Dir, Models::Dir> getBodyDir(std::__cxx11::list<Coord>::iterator c, Model* model);
            Models::Dir getTailDir(Model* model);

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

            Model() {}
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