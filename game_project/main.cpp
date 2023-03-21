#include <iostream>
#include "views/view.h"
#include "views/textview.h"
#include "views/guiview.h"
#include "models/model.h"
#include "controllers/controller.h"

int main(int argc, char** argv)
{
    std::string view_arg;
    View *view;

    if (argc > 1)
    {
        view_arg = std::string(argv[1]);
        view = View::get(view_arg);
        
        Models::Model model{};
        View::setModel(model);

        model.setLoopPeriod(1000000);

        view->draw();
    }
    else
        return 1;
    
    return 0;
}