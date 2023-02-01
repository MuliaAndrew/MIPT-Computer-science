#include "text.h"
#include "dict.h"


int main(int argc, char** argv)
{
    text_t text;

    if (argc > 2)
        text = make_text_from_cmd(argv);

    else
        text = make_text_from_file(argv[1]);

    dict_t dict;
    dict_init(&dict);

    
}