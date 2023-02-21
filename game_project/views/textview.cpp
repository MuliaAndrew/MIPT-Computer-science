#include "textview.h"

const char CSI[] = "\e[";

struct sigaction TextView::sig = {};

TextView::TextView()
{
    setWinSZSig();
}

void TextView::draw()
{
    clean();

    auto sz = getWinSizw();

    hline(0, sz.ws_col);
    hline(sz.ws_row, sz.ws_col);

    vline(0, sz.ws_row);
    vline(sz.ws_col, sz.ws_row);

    putXY(sz.ws_row / 2, sz.ws_col / 2);
    printf("%u %u", sz.ws_row, sz.ws_col);

    fflush(stdout);
}

winsize TextView::getWinSizw()
{
    struct winsize sz;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz))
    {
        perror("in TextView::getWinSizw()");
        return winsize{};
    }

    return sz;
}

void TextView::setWinSZSig()
{
    sigaction(SIGWINCH, nullptr, &sig);

    sig.sa_handler = &winHndlr;

    sigaction(SIGWINCH, &sig, nullptr);
}

void winHndlr(int signum)
{
    std::string s = "";
    auto view = View::get(s);

    view->draw();
}

void TextView::clean()
{
    printf("%sH", CSI);
    printf("%sJ", CSI);
}

void TextView::putXY(unsigned int x, unsigned int y)
{
    printf("%s%u;%uH", CSI, x, y);
}

void TextView::line(unsigned int x, unsigned int y, LineDir dir, unsigned int length)
{
    if (dir >= 2)
        return;
    
    printf("%s%u;%uH", CSI, y, x);

    if (dir == h)
    {
        for (int i = 0; i < length; i++)
            putchar('#');
    }
    else
    {
        for (int i = 0; i <= length; i++)
        {
            printf("%s%u;%uH", CSI, y + i, x);
            putchar('#');
        }
    }
}

void TextView::vline(unsigned int x, unsigned int length)
{
    line(x, 0, LineDir::v, length);
}

void TextView::hline(unsigned int y, unsigned int length)
{
    line(0, y, LineDir::h, length);
}