#include "common.h"

inline bool IdType::isFunc() {
    return is_func;
}

inline Type IdType::retType() {
    return return_type;
}

inline std::vector<Type>& IdType::argsTypes() {
    return args_types;
}
