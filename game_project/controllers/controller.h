#pragma once

#include <iostream>
#include <unistd.h>

#include "../models/model.h"

template<typename T>
class Matrix
{
        size_t m;
        size_t n;
        std::vector<T> buf;

    public:
        Matrix(size_t m_ = 0, size_t n_ = 0) : m(m_), n(n_)
        {
            buf = std::vector<T>(m * n, 0);
        }

        size_t getM() const { return m; }
        size_t getN() const { return n; }

        T& operator()(size_t x, size_t y)
        {
            return buf[x * n + y];
        }

        void clear()
        {
            buf.clear();
        }

        T& up_to(size_t x, size_t y);
        T& down_to(size_t x, size_t y);
        T& left_to(size_t x, size_t y);
        T& right_to(size_t x, size_t y);
};

class Controller
{
    protected:
        static Models::Model* model;
        Models::Snake* snake;

    public:
        virtual ~Controller() {}

        static Controller* get(const std::string &);
        static void setModel(Models::Model &);
        void setSnake(Models::Snake* s_);

        virtual int fonKey(int) = 0;
};

class Human final : public Controller
{
    public:
        enum KeySet { wasd, arrows };

    private:
        KeySet keyset = wasd;
    
    public:
        Human(KeySet k) : keyset(k) {};
        Human() {}
        ~Human() {}

        int fonKey(int);
};

class AI final : public Controller
{
    private:
        static Matrix<int> field;
        Models::Dir calcNextStep();
    
    public:
        AI() {}
        ~AI() {}

        static void update();
        static void printField();

        int fonKey(int);
};
