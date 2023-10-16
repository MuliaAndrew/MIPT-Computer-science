#pragma once

#include "common.h"
#include <vector>
#include <unordered_set>

class ASTNodeBase {
    protected:
        ASTNodeBase* parent;
        std::vector<ASTNodeBase*> child;
        
    public:
        ASTNodeBase() = delete;

        virtual ~ASTNodeBase();
        
        ASTNodeBase* getParent();
        std::vector<ASTNodeBase*>& getChildArr();
        virtual ASTNodeType getType() = 0;
        virtual void codegen() = 0;
};

class ASTNodeST : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::ST; } 
};

class ASTNodeAsSt : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::ASST; }
};

class ASTNodeOp : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::OP; }
};

class ASTNodeVar : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::VAR; }
};

class ASTNodeVal : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::VAL; }
};

class ASTNodeCall : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::CALL; }
};

class ASTNodeScope : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::SCOPE; }
};

class ASTNodeFuncDecl : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::FDECL; }
};

class ASTNodeVarDecl : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::VARDECL; }
};

class ASTNodeIfSt : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::IFST; }
};

class ASTNodeWhileSt : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::WHILEST; }
};

class ASTNodeRetSt : public ASTNodeBase {
    public:
        ASTNodeType getType() const { return ASTNodeType::RETST; }
};
