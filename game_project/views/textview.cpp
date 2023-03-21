#include "textview.h"

const char CSI[] = "\e[";

struct sigaction TextView::sig = {};

TextView::TextView()
{
    setWinSZSig();
}

void TextView::draw()
{
    auto snake1 = model->createSnake({3, 3});
    model->createRabit({5, 5});
    model->createRabit({15, 20});
    
    while(true)
    {
        clean();

        auto sz = getWinSize();

        hline(0, sz.ws_col);
        hline(sz.ws_row, sz.ws_col);

        vline(0, sz.ws_row);
        vline(sz.ws_col, sz.ws_row);

        snake1->move(sz, Models::down);

        putXY(sz.ws_row / 2, sz.ws_col / 2);
        printf("x, y: %u, %u", snake1->coords_begin()->first, snake1->coords_begin()->second);

        for (auto snake = model->snakes_begin(); snake != model->snakes_end(); snake++)
        {
            for (auto coord = snake->coords_begin(); coord != snake->coords_end(); coord++)
            {
                putXY(coord->first, coord->second);
                putSymb('@');
            }
        }

        for (auto rabit = model->rabits_begin(); rabit != model->rabits_end(); rabit++)
        {
            auto coord = rabit->coords();
            putXY(coord.first, coord.second);
            putSymb('$');
        }

        fflush(stdout);

        usleep(model->getLoopPeriod());
    }
}

winsize TextView::getWinSize()
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
    // std::string s = "";
    // auto view = View::get(s);

    // view->draw();
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

void TextView::putSymb(char symb)
{
    printf("%c", symb);
}

void TextView::line(unsigned int x, unsigned int y, LineDir dir, unsigned int length, char symb)
{
    if (dir >= 2)
        return;
    
    printf("%s%u;%uH", CSI, y, x);

    if (dir == h)
    {
        for (int i = 0; i < length; i++)
            putchar(symb);
    }
    else
    {
        for (int i = 0; i <= length; i++)
        {
            printf("%s%u;%uH", CSI, y + i, x);
            putchar(symb);
        }
    }
}

void TextView::vline(unsigned int x, unsigned int length)
{
    line(x, 0, LineDir::v, length, '#');
}

void TextView::hline(unsigned int y, unsigned int length)
{
    line(0, y, LineDir::h, length, '#');
}

