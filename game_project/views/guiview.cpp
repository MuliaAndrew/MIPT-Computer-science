#include "guiview.h"

sf::RenderWindow GUIView::window{};
std::list<sf::Sprite> GUIView::rabbits{};
std::vector<std::vector<sf::Sprite>> GUIView::snakes;
sf::Texture GUIView::rabbit_texture{};
std::vector<sf::Texture> GUIView::snake_textures{};
sf::Texture GUIView::background_texture{};

std::unordered_map<Models::Color ,sf::Color> color_map{
    {Models::Color::blue, sf::Color::Blue},
    {Models::Color::green, sf::Color::Green},
    {Models::Color::ocean, sf::Color::Cyan},
    {Models::Color::purple, sf::Color::Magenta},
    {Models::Color::red, sf::Color::Red},
    {Models::Color::white, sf::Color::White},
    {Models::Color::yellow, sf::Color::Yellow}       
};

void GUIView::draw()
{
    sf::Sprite background(background_texture, sf::IntRect(0, 0, 1425, 892));
    background.scale(1.021, 1.021);

    for (int i = 0; i < 10; i++)
        model->createRabbitRandom();

    while (window.isOpen())
    {
        int ch = getCtrlNum();

        int isOnPause;

        for (auto& humanFonKey : HumanfonKeys)
            isOnPause = humanFonKey(ch);

        AI::update();
        
        for (auto AiFonKey : AIfonKeys)
            isOnPause = AiFonKey(ch);
        
        if (ch & Models::CLOSE_BIT)
            break;

        window.clear();

        window.draw(background);

        for (auto r = model->rabbits_begin(), r_end = model->rabbits_end(); r != r_end; r++)
        {
            auto r_ = sf::Sprite(rabbit_texture, sf::IntRect(0, 0, 1200, 1200));
            r_.setPosition((r->coords().second - 2) * PIXELS_IN_CELL, (r->coords().first - 2) * PIXELS_IN_CELL);
            r_.scale(0.04, 0.04);
            rabbits.push_back(r_);
        }

        drawSnakes();

        for (auto& rabbit : rabbits)
            window.draw(rabbit);
        
        for (auto& snake : snakes)
        {
            for (auto& snake_part : snake)
                window.draw(snake_part);
        }
        
        window.display();

        rabbits.clear();
        snakes.clear();
    } 
}

GUIView::GUIView()
{
    window.create(sf::VideoMode(1440, 900), "Snake!");
    window.setFramerateLimit(10);
    window.setMouseCursorVisible(false);

    snake_textures = std::vector<sf::Texture>(4, sf::Texture{});

    rabbit_texture.loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/rabbit.png");
    background_texture.loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/grass_background.png");
    snake_textures[0].loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/snake_head.png");
    snake_textures[1].loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/snake_body.png");
    snake_textures[2].loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/snake_turn.png");
    snake_textures[3].loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/snake_tail.png");
}

winsize GUIView::getWinSize()
{
    winsize sz;
    auto win_sz = window.getSize();
    sz.ws_row = win_sz.y / PIXELS_IN_CELL + 2;
    sz.ws_col = win_sz.x / PIXELS_IN_CELL + 3;
    return sz;
}

/* 
int ctrl_num: 
    first bit replying on Pause mode 
    second bit replying closed window
    4-8 bits - up, down, left, right for wasd controller
    9-12 bits - up, sown, left, right for arrows controller
*/

// Converts user input to spetial control number which represents user commands

int GUIView::getCtrlNum()
{
    int ctrl_num = 0;

    sf::Event event;

    while(window.pollEvent(event))
    {
        if (event.type == sf::Event::EventType::Closed)
            return ctrl_num |= Models::CLOSE_BIT; 
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            ctrl_num |= Models::ARROW_LEFT;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        ctrl_num |= Models::ARROW_RIGHT;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        ctrl_num |= Models::ARROW_UP;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        ctrl_num |= Models::ARROW_DOWN;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        ctrl_num |= Models::WASD_LEFT;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        ctrl_num |= Models::WASD_RIGHT;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        ctrl_num |= Models::WASD_UP;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        ctrl_num |= Models::WASD_DOWN;

    return ctrl_num;
}

void GUIView::drawSnakes()
{
    for (auto s = model->snakes_begin(), s_end = model->snakes_end(); s != s_end; s++)
    {
        sf::Color snake_color = color_map[s->getColor()];
        if (s->cutIfBump(model))
            s->status = Models::DEAD;
        
        if (s->status == Models::DEAD)
            snake_color = sf::Color::Black;

        snakes.push_back({});
        auto& snake = snakes.back();

        auto c = s->coords_begin();
        auto c_dir = s->getHeadDir();

        sf::Sprite c_ = sf::Sprite(snake_textures[0], sf::IntRect(0, 0, 45, 45));

        c_.setOrigin(22.5, 22.5);
        c_.setColor(snake_color);
        c_.setPosition((c->second - 2)* PIXELS_IN_CELL + 22.5, (c->first - 2) * PIXELS_IN_CELL + 22.5);

        switch (c_dir)
        {
            case Models::Dir::up:
                c_.rotate(90);
                break;
            case Models::Dir::down:
                c_.rotate(270);
                break;
            case Models::Dir::right:
                c_.rotate(180);
                break;
        }

        snake.push_back(c_);

        c++;
        auto c_end = s->coords_end();
        c_end--;

        for (; c != c_end; c++)
        {
            auto body_dir = s->getBodyDir(c, model);
            auto& forward = body_dir.first;
            auto& backward = body_dir.second;

            if (forward == -backward)
            {
                c_ = sf::Sprite(snake_textures[1], sf::IntRect(0, 0, 45, 45));
                c_.setOrigin(22.5, 22.5);

                if (forward == Models::Dir::up || forward == Models::Dir::down)
                    c_.rotate(90);
            }
            else
            {
                c_ = sf::Sprite(snake_textures[2], sf::IntRect(0, 0, 45, 45));
                c_.setOrigin(22.5, 22.5);

                if (
                    forward == Models::Dir::up && backward == Models::Dir::left ||
                    forward == Models::Dir::left && backward == Models::Dir::up
                )
                    c_.rotate(90);
                
                else if (
                    forward == Models::Dir::up && backward == Models::Dir::right ||
                    forward == Models::Dir::right && backward == Models::Dir::up
                )
                    c_.rotate(180);
                
                else if (
                    forward == Models::Dir::down && backward == Models::Dir::right ||
                    forward == Models::Dir::right && backward == Models::Dir::down
                )
                    c_.rotate(270);
            }

            c_.setPosition((c->second - 2) * PIXELS_IN_CELL + 22.5, (c->first - 2) * PIXELS_IN_CELL + 22.5);
            c_.setColor(snake_color);
            snake.push_back(c_);
        }

        c_ = sf::Sprite(snake_textures[3], sf::IntRect(0, 0, 45, 45));
        c_.setOrigin(22.5, 22.5);

        c_dir = s->getTailDir(model);

        switch (c_dir)
        {
            case Models::Dir::up:
                c_.rotate(90);
                break;
            case Models::Dir::down:
                c_.rotate(270);
                break;
            case Models::Dir::right:
                c_.rotate(180);
                break;
        }

        c_.setPosition((c->second - 2) * PIXELS_IN_CELL + 22.5, (c->first - 2) * PIXELS_IN_CELL + 22.5);
        c_.setColor(snake_color);
        snake.push_back(c_);
    }
}