#pragma once

#include "id_node.h"

class IdList {
        std::unordered_set<IdNode> buffer;

    public:
        inline bool contains(const IdNode &);
        inline bool contains(IdNode *);
    
        inline bool insert(IdNode *);
        inline bool insert(const IdNode &);

        IdNode get_by_name(const std::string &);
};