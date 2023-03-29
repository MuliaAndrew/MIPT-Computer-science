#include "guiview.h"

sf::RenderWindow GUIView::window{};
std::list<sf::Sprite> GUIView::rabbits{};
std::vector<std::vector<sf::Sprite>> GUIView::snakes;
sf::Texture GUIView::rabbit_texture{};
std::vector<sf::Texture> GUIView::snake_textures{};

void GUIView::draw()
{
    GUIView::InitGraphics();
    
    auto rabbit = sf::Sprite(rabbit_texture, sf::IntRect(0, 0, 1200, 1200));
    rabbit.scale(0.1, 0.1);
    rabbits.push_back(rabbit);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (auto rabbit : rabbits)
            window.draw(rabbit);
        
        window.display();
    } 
}

void GUIView::InitGraphics()
{
    window.create(sf::VideoMode(1200, 1200), "Snake!");

    rabbit_texture.loadFromFile("/home/muliaandrew/MIPT/MIPT-Computer-science/game_project/textures/rabbit.png");
}

winsize GUIView::getWinSize()
{
    winsize sz{};
    return sz;
}