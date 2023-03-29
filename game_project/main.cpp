#include <iostream>

#include "views/view.h"
#include "views/textview.h"
#include "views/guiview.h"
#include "models/model.h"
#include "controllers/controller.h"

int main(int argc, char** argv)
{
    const std::string HUMAN = "Human";
    const std::string AI = "AI";

    std::string view_arg;
    View *view;

    if (argc > 1)
    {
        view_arg = std::string(argv[1]);
        view = View::get(view_arg);
        
        Models::Model model{};
        View::setModel(model);
        model.setWinSZ(view->getWinSize());
        Controller::setModel(model);

        model.setLoopPeriod(500000);

        auto snake = model.createSnake({3, 3});

        Human human2{Human::wasd};
        human2.setSnake(snake);
        auto human2_fonkey = std::bind(&Human::fonKey, &human2, std::placeholders::_1);

        view->setFonKeyHuman(human2_fonkey);

        view->draw();
    }
    else
        return 1;
    
    return 0;
}