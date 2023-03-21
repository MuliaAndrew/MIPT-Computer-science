#pragma once

#include "../models/model.h"

class Human
{
    public:
        enum KeySet { wasd, arrows };

    protected:
        static Models::Model* model;
        KeySet keyset;
    
    public:
        Human(KeySet) {}
        ~Human() {}

        static void setModel(Models::Model&);
        int f_onkey(int);
};