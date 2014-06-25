//
//  NodeLoadingFacilities.h
//
//  Created by Gérald Guyomard on 28/09/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __NodeLoadingFacilities__
#define __NodeLoadingFacilities__

#include <cocos2d.h>

#include "SelectorResolver.h"
#include "MemberVariableAssigner.h"
#include "cocoa/CCObjectPtr.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

    class NodeLoadingFacilities
    {
    public:
        virtual ~NodeLoadingFacilities();
        
        virtual CCObject* owner() = 0;
        
        template <class TNode> static CCObjectPtr<TNode> createFromResource(const std::string& iResourceName);
        
        class Resolvers
        {
        public:
            SelectorResolver& selectorResolver();
            SelectorResolver* selectorResolverIfAny() { return fSelectorResolver.get(); }
            
            MemberVariableAssigner& memberVariableAssigner();
            MemberVariableAssigner* memberVariableAssignerIfAny() { return fMemberVariableAssigner.get(); }
            
        private:
            std::unique_ptr<SelectorResolver> fSelectorResolver;
            std::unique_ptr<MemberVariableAssigner> fMemberVariableAssigner;
        };

    protected:
        NodeLoadingFacilities();
        
        virtual void _fillResolvers(Resolvers& iResolvers);
        virtual void _preLoad();

    protected:
        void _load(const std::string& iResourceName);

        virtual void _postLoad(CCNode* iLoadedRootNode);
    };
    
    // Inlined implementation
    template <class TNode> inline CCObjectPtr<TNode>
    NodeLoadingFacilities::createFromResource(const std::string& iResourceName)
    {
        CCObjectPtr<TNode> node;
        node << new TNode;
        
        node->_load(iResourceName);

        return node;
    }

NS_CC_EXT_END

#endif /* defined(__NodeLoadingFacilities__) */
