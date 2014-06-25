//
//  MemberVariableAssigner.h
//
//  Created by Gérald Guyomard on 28/09/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __MemberVariableAssigner__
#define __MemberVariableAssigner__

#include "CCBReader/CCBMemberVariableAssigner.h"
#include "cocoa/CCObjectPtr.h"

#include <map>
#include <memory>

NS_CC_EXT_BEGIN

    class MemberVariableAssigner : public CCBMemberVariableAssigner
    {
    public:
        MemberVariableAssigner();
        ~MemberVariableAssigner();
        
        virtual bool onAssignCCBMemberVariable(CCObject * pTarget, const char* pMemberVariableName, CCNode * pNode) override;
        
        void addMember(const std::string& iName, CCObjectPtr<CCNode>& oNode);
        template <class TNode> void addMember(const std::string& iName, CCObjectPtr<TNode>& oNode);

        // Contruct name as iName_%iIndex
        void addMember(const std::string& iPrefixName, size_t iIndex, CCObjectPtr<CCNode>& oNode);
        template <class TNode> void addMember(const std::string& iPrefixName, size_t iIndex, CCObjectPtr<TNode>& oNode);

        void addMember(const std::string& iName, std::vector<CCObjectPtr<CCNode>>& oArrayOfNodes);
        template <class TNode> void addMember(const std::string& iName, std::vector<CCObjectPtr<TNode>>& oArrayOfNodes);
      
    public:
        class Member;

    private:
        typedef std::unique_ptr<Member> MemberPtr;

        typedef std::map<std::string, MemberPtr> TNameToMember;
        TNameToMember fNameToMember;
    };
    
    // Inlined implementations
    template <class TNode> inline void
    MemberVariableAssigner::addMember(const std::string& iName, CCObjectPtr<TNode>& oNode)
    {        
        addMember(iName, (CCObjectPtr<CCNode>&) oNode);
    }

    template <class TNode> inline void
    MemberVariableAssigner::addMember(const std::string& iPrefixName, size_t iIndex, CCObjectPtr<TNode>& oNode)
    {
        addMember(iPrefixName, iIndex, (CCObjectPtr<CCNode>&) oNode);
    }
    
    template <class TNode>
    inline void
    MemberVariableAssigner::addMember(const std::string& iName, std::vector<CCObjectPtr<TNode>>& oArrayOfNodes)
    {
        addMember(iName, (std::vector<CCObjectPtr<CCNode>>&) oArrayOfNodes);
    }

NS_CC_EXT_END

#endif /* defined(__MemberVariableAssigner__) */
