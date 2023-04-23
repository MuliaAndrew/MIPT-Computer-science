#include <iostream>

#include "views/view.h"
#include "views/textview.h"
#include "views/guiview.h"
#include "models/model.h"
#include "controllers/controller.h"

int main(int argc, char** argv)
{
    const std::string HUMAN_STR = "Human";
    const std::string AI_STR = "AI";

    std::string view_arg;
    std::string n_humans;
    std::string n_AIs;
    View *view;
    std::vector<Human> humans;

    if (argc > 3)
    {
        view_arg = std::string(argv[1]);
        view = View::get(view_arg);

        n_humans = std::string(argv[2]);
        n_AIs = std::string(argv[3]);
        
        Models::Model model{};
        View::setModel(model);

        auto sz = view->getWinSize();

        if (sz.ws_col < 20)
        {
            std::cout << "Please, make terminal window size bigger to correct running of game" << std::endl;
            std::cout << "Corrent size: x - " << sz.ws_row << ", y - " << sz.ws_col << std::endl;
            return 0;
        }

        model.setWinSZ(sz);
        Controller::setModel(model);

        model.setLoopPeriod(100000);
        
        humans.resize(std::stoi(n_humans));

        if (std::stoi(n_humans) >= 1)
        {
            auto human = new Human{Human::wasd};
            
            Models::Snake* snake = model.createSnake({2, 2});
            human->setSnake(snake);
            snake->setColor(Models::Color::red);

            HumanFonKeyBindType f = std::bind(&Human::fonKey, &*human, std::placeholders::_1);
            view->setFonKeyHuman(f);
        }

        if (std::stoi(n_humans) == 2)
        {
            auto human2 = new Human{Human::arrows};

            Models::Snake* snake2 = model.createSnake({8, 2});
            human2->setSnake(snake2);
            snake2->setColor(Models::Color::blue);

            auto f2 = std::bind(&Human::fonKey, &*human2, std::placeholders::_1);
            view->setFonKeyHuman(f2);
        }

        if (std::stoi(n_AIs) >= 1)
        {
            AI* AI1 = new AI;

            Models::Snake* snake3 = model.createSnake({13, 2});
            AI1->setSnake(snake3);
            snake3->setColor(Models::Color::ocean);

            auto f3 = std::bind(&AI::fonKey, &*AI1, std::placeholders::_1);
            view->setFonKeyAI(f3);
        }

        if (std::stoi(n_AIs) == 2)
        {
            AI* AI2 = new AI;

            Models::Snake* snake4 = model.createSnake({18, 2});
            AI2->setSnake(snake4);
            snake4->setColor(Models::Color::green);

            auto f4 = std::bind(&AI::fonKey, &*AI2, std::placeholders::_1);
            view->setFonKeyAI(f4);
        }

        view->draw();
    }
    else
        return 1;
    
    return 0;
}
