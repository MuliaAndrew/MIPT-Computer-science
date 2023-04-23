#include "textview.h"

const char CSI[] = "\e[";
const char* COLORS_str[] = {"RED", "GREEN", "YELLOW", "BLUE", "PURPLE", "OCEAN", "WHITE"};
const int COLORS_OFFSET = 31;

struct sigaction TextView::sig = {};

TextView::TextView()
{
    setWinSZSig();
    printf("%s?25l", CSI);
}

void TextView::draw()
{
    int isOnPause = 0;

    for (int i = 0; i < 10; i++)
        model->createRabbitRandom();

    int ch = 0;

    while(true)
    {
        auto sz = getWinSize();
        model->setWinSZ(sz);

        ch = getCtrlNum();

        clean();

        AI::update();

        for (auto fonKey : HumanfonKeys)
            isOnPause = fonKey(ch);

        for (auto fonKey : AIfonKeys)
            isOnPause = fonKey(ch);

        if (!isOnPause)
        {
            hline(0, sz.ws_col);
            hline(sz.ws_row, sz.ws_col);

            vline(0, sz.ws_row);
            vline(sz.ws_col, sz.ws_row);

            int snake_n = 0;

            putXY(0, sz.ws_col + 8);
            printf("SCORE:");

            for (auto snake = model->snakes_begin(), snake_end = model->snakes_end(); snake != snake_end; snake++)
            {
                if (snake->cutIfBump(model))
                    snake->status = Models::DEAD;
                
                if (snake->status == Models::DEAD)
                    printf("%s30m", CSI);
                else
                    printf("%s%dm", CSI, snake->getColor());
                putXY(snake_n * 2 + 3, sz.ws_col + 3);
                printf("%s : %d", COLORS_str[snake->getColor() - COLORS_OFFSET], snake->getLenght() - 1);

                for (auto coord = snake->coords_begin(); coord != snake->coords_end(); coord++)
                {
                    putXY(coord->first, coord->second);
                    putSymb('@');
                }

                snake_n++;
            }

            printf("%s39m", CSI);

            for (auto rabbit = model->rabbits_begin(); rabbit != model->rabbits_end(); rabbit++)
            {
                auto coord = rabbit->coords();
                putXY(coord.first, coord.second);
                putSymb('$');
            }

            fflush(stdout);
        }
        else
        {
            if (model->isOnPause)
                return;
            else
                model->isOnPause = true;
        }
        
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

    sz.ws_col = sz.ws_col - 20;

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

TextView::~TextView()
{
    printf("%s?25h", CSI);
}

/* 
int ctrl_num: 
    first bit replying on Pause mode 
    4-8 bits - up, down, left, right for wasd controller
    9-12 bits - up, sown, left, right for arrows controller
*/

// Converts user input to spetial control number witch represents user commands
int getCtrlNum()
{
    std::string buffer;
    buffer.resize(4095);

    int n_readed = read(STDIN_FILENO, const_cast<char*>(buffer.c_str()), 4095);

    int ctrl_num = 0;

    for (int i = 0; i < n_readed; i++)
    {
        if (buffer[i] == '\e')
        {
            if (i + 2 < n_readed && buffer[i + 1] == '[')
            {
                switch (buffer[i + 2])
                {
                    case 'A':
                        ctrl_num &= 65295;
                        ctrl_num |= Models::ARROW_UP;
                        break;
                    case 'B':
                        ctrl_num &= 65295;
                        ctrl_num |= Models::ARROW_DOWN;
                        break;
                    case 'C':
                        ctrl_num &= 65295;
                        ctrl_num |= Models::ARROW_RIGHT;
                        break;
                    case 'D':
                        ctrl_num &= 65295;
                        ctrl_num |= Models::ARROW_LEFT;
                        break;
                    default:
                        break;
                }
                i += 2;
            }
            else
                ctrl_num |= Models::PAUSE_BIT;
        }
        else
        {
            switch(buffer[i])
            {
                case 'A':
                case 'a':
                    ctrl_num &= 61695;
                    ctrl_num |= Models::WASD_LEFT;
                    break;
                case 'S':
                case 's':
                    ctrl_num &= 61695;
                    ctrl_num |= Models::WASD_DOWN;
                    break;
                case 'D':
                case 'd':
                    ctrl_num &= 61695;
                    ctrl_num |= Models::WASD_RIGHT;
                    break;
                case 'W':
                case 'w':
                    ctrl_num &= 61695;
                    ctrl_num |= Models::WASD_UP;
                    break;
                default:
                    break;
            }
        }
    }
    return ctrl_num;
}
