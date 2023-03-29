#pragma once

#include <iostream>
#include <unistd.h>

#include "../models/model.h"

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

        virtual int fonKey(char) = 0;
};

class Human : public Controller
{
    public:
        enum KeySet { wasd, arrows };

    protected:
        KeySet keyset = wasd;
    
    public:
        Human(KeySet) {}
        Human() {}
        ~Human() {}

        int fonKey(char);
};

class AI : public Controller
{
        int calcNextStep();

    public:
        AI() {}
        ~AI() {}

        int fonKey(char);
};