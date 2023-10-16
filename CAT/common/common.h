#pragma once

#include <vector>
#include <string>

enum Type {
    INT,
    BOOL,
    FLOAT,
    CHAR
};

enum ASTNodeType {
    OP,
    VAR,
    VAL,
    CALL,
    SCOPE,
    FDECL,
    VARDECL,
    ST,
    ASST,
    RETST,
    IFST,
    WHILEST
};

class Token final {
    public:
        enum Type {
            ID,
            INTNUM,
            FLOATNUM
        };
    
    private:    
        Token::Type type;
        std::string val;
    
    public:
        Token(Token::Type type_, std::string &val_) : type(type_), val(val_) {};
        Token::Type getType() const {
            return type;
        };
        std::string & getVal() { return val; }
};

class IdType {
        /*  
        if identifier is variable or constant it have only return type which, if not 
        identifier is function that have signature (return type + array of args types)
        */ 
        bool is_func;
        Type return_type;
        std::vector<Type> args_types;
    public:
        inline bool isFunc();
        inline Type retType();
        inline std::vector<Type>& argsTypes();
};

class IdScope {
        IdScope* parent;
    public:
        IdScope(IdScope* p_) : parent(p_) {};
        static bool idAccess(IdScope*, IdScope*);
};
