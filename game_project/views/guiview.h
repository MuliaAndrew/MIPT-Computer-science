#pragma once

#include <SFML/Graphics.hpp>

#include "view.h"

class GUIView : public View
{
        static sf::RenderWindow window;
        static std::list<sf::Sprite> rabbits;
        static std::vector<std::vector<sf::Sprite>> snakes; 
        static sf::Texture rabbit_texture;
        static std::vector<sf::Texture> snake_textures;

    public:
        GUIView() {};
        ~GUIView() {};

        static void InitGraphics();

        void draw();
        winsize getWinSize();
};