#include "ast_node.h"

ASTNodeBase* ASTNodeBase::getParent() {
    return parent;
}

std::vector<ASTNodeBase*> & ASTNodeBase::getChildArr() {
    return child;
}