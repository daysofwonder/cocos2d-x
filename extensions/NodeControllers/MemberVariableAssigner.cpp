//
//  MemberVariableAssigner.cpp
//
//  Created by Gérald Guyomard on 28/09/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "MemberVariableAssigner.h"
#include "DOWFoundation/StringUtils.h"

NS_CC_EXT_BEGIN

    class MemberVariableAssigner::Member
    {
    public:
        virtual ~Member() {}
        
        virtual void assign(CCNode* iNode) = 0;
        
    };
    
    class CCNodePtrMember : public MemberVariableAssigner::Member
    {
    public:
        CCNodePtrMember(CCObjectPtr<CCNode>& oNodePtr)
        : fNodePtr(oNodePtr)
        {}
        
        virtual void assign(CCNode* iNode) override
        {
            fNodePtr = iNode;
        }
        
    private:
        CCObjectPtr<CCNode>& fNodePtr;
    };
    
    class ArrayOfCCNodePtrsMember : public MemberVariableAssigner::Member
    {
    public:
        ArrayOfCCNodePtrsMember(std::vector<CCObjectPtr<CCNode>>& oArray)
        : fArray(oArray)
        {}
        
        virtual void assign(CCNode* iNode) override
        {
            fArray.push_back(iNode);
        }
        
    private:
        std::vector<CCObjectPtr<CCNode>>& fArray;
    };


MemberVariableAssigner::MemberVariableAssigner()
{}
    
MemberVariableAssigner::~MemberVariableAssigner()
{}

bool
MemberVariableAssigner::onAssignCCBMemberVariable(CCObject * pTarget, const char* pMemberVariableName, CCNode * pNode)
{
    assert(strlen(pMemberVariableName) != 0);
    
    std::string key(pMemberVariableName);
    const TNameToMember::const_iterator it = fNameToMember.find(key);
    if (it != fNameToMember.end())
    {
        it->second->assign(pNode);
        return true;
    }
    
    return false;
}

void
MemberVariableAssigner::addMember(const std::string& iName, CCObjectPtr<CCNode>& oNode)
{
    std::unique_ptr<CCNodePtrMember> member(new CCNodePtrMember(oNode));
    fNameToMember[iName] = std::move(member);
}

void
MemberVariableAssigner::addMember(const std::string& iPrefixName, size_t iIndex, CCObjectPtr<CCNode>& oNode)
{
    std::string name;
    DOW::StringUtils::format(name, "%s_%d", iPrefixName.c_str(), iIndex);
    
    addMember(name, oNode);
}
    
void
MemberVariableAssigner::addMember(const std::string& iName, std::vector<CCObjectPtr<CCNode>>& oArrayOfNodes)
{
    std::unique_ptr<ArrayOfCCNodePtrsMember> member(new ArrayOfCCNodePtrsMember(oArrayOfNodes));
    fNameToMember[iName] = std::move(member);
}
    
NS_CC_EXT_END
