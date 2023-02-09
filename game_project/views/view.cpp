#include "view.h"
#include "textview.h"
#include "guiview.h"

View *View::obj = nullptr;

View* View::get(std::string &s)
{
    if (obj == nullptr)
    {
        if (s == "gui")
            obj = new GUIView;

        else if (s == "text")
            obj = new TextView;
        
        return obj;
    }

    else
    {
        if (s.empty())
            return obj;
        
        else return nullptr;
    }
}