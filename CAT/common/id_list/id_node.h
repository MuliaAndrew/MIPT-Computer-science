#pragma once

#include <string>
#include "../common.h"
#include "../ast_node.h"

class IdNode
{
        std::string name;
        IdType _type;
        IdScope* native_scope;

    public:
        IdNode() = delete;
        IdNode(std::string& name_, bool is_var_, IdType type_, IdScope* native_scope_) : 
            name(name_), _type(type_), native_scope(native_scope_) {};
        inline IdType type() const; 
        inline std::string idName() const;
        inline bool assertIdScope(IdScope *);
};