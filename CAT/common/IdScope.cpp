#include "common.h"

bool IdScope::idAccess(IdScope* cur, IdScope* id_native_scope)
{
    if (cur != id_native_scope)
    {
        if (cur->parent != nullptr)
            idAccess(cur->parent, id_native_scope);
        else
            return false;
    }
    else
        return true;
}