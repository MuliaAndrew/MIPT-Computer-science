#include "guiview.h"

void GUIView::draw()
{
    std::cout << "Undef behavior" << std::endl;
    return;
}

winsize GUIView::getWinSize()
{
    struct winsize sz{};
    return sz;
}