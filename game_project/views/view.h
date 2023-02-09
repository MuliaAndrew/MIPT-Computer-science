#pragma once

#include <iostream>
#include <string>

class View 
{
        static View* obj;

    public:
        virtual ~View() {};

        static View* get(std::string &);
        virtual void draw() = 0;
};