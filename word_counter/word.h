#ifndef WORD_H
#define WORD_H

struct methods_t {
    void    alloc_;
    void    free_;
    void*   parse;
}


struct word_t {
    char*       ptr;
    int         len;
    methods_t*  methods;
}


#undef // WORD_H