#include "view.h"
#include "textview.h"
#include "guiview.h"

View *View::obj = nullptr;
termios View::default_term_attrs = {};
Models::Model *View::model = nullptr;
std::vector<HumanFonKeyBindType> View::HumanfonKeys{};
std::vector<AIFonKeyBindType> View::AIfonKeys{};

View* View::get(const std::string &s)
{
    if (tcgetattr(STDIN_FILENO, &default_term_attrs))
    {
        perror("in tcgetattr()");
        exit(EXIT_FAILURE);
    }

    termios new_term_attrs = default_term_attrs;
    new_term_attrs.c_lflag &= ~ICANON;
    new_term_attrs.c_lflag &= ~ECHO;
    new_term_attrs.c_cc[VMIN] = 0;
    new_term_attrs.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term_attrs))
    {
        perror("in tcsetattr()");
        exit(EXIT_FAILURE);
    }

    if (obj == nullptr)
    {
        if (s == "gui")
            obj = new GUIView;

        else if (s == "text")
            obj = new TextView;
        
        return obj;
    }

    else
    {
        if (s.empty())
            return obj;
        
        else return nullptr;
    }
}

void View::setModel(Models::Model& m_)
{
    model = &m_;
}

View::~View()
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &default_term_attrs))
    {
        perror("in tcsetattr()");
        exit(EXIT_FAILURE);
    }
}

void View::setFonKeyHuman(HumanFonKeyBindType f)
{
    HumanfonKeys.push_back(f);
}

void View::setFonKeyAI(AIFonKeyBindType f)
{
    AIfonKeys.push_back(f);
}
