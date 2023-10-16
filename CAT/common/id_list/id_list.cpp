#include "id_list.h"

inline bool IdList::contains(const IdNode & id_node) {
    return buffer.count(id_node);
}

inline bool IdList::contains(IdNode * id_node) {
    return buffer.count(*id_node);
}

inline bool IdList::insert(const IdNode & id_node) {
    return buffer.insert(id_node).second;
}

inline bool IdList::insert(IdNode * id_node) {
    return buffer.insert(*id_node).second;
}

IdNode IdList::get_by_name(const std::string & name) {
    for (auto id = buffer.begin(); id != buffer.end(); id++) {
        if (id->idName() == name) {
            return (*id);
        }
    }
}
