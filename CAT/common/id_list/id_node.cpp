#include "id_node.h"


inline std::string IdNode::idName() const
{
    return name;
}

inline IdType IdNode::type() const
{
    return _type;
}

bool IdNode::assertIdScope(IdScope* cur)
{
    return IdScope::idAccess(cur, native_scope);
}
