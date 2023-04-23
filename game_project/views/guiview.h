#pragma once

#include <SFML/Graphics.hpp>

#include "view.h"

class GUIView : public View
{
        const unsigned int PIXELS_IN_CELL = 45;

        static sf::RenderWindow window;
        static std::list<sf::Sprite> rabbits;
        static std::vector<std::vector<sf::Sprite>> snakes; 
        static sf::Texture rabbit_texture;
        static std::vector<sf::Texture> snake_textures;
        static sf::Texture background_texture;

        int getCtrlNum();
        void drawSnakes();

    public:
        GUIView();
        ~GUIView() {};

        void draw();
        winsize getWinSize();
};