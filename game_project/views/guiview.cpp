#include "guiview.h"

void GUIView::draw()
{
    std::cout << "Undef behavior" << std::endl;
    return;
}

winsize GUIView::getWinSizw()
{
    struct winsize sz{};
    return sz;
}