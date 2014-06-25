//
//  NodeLoadingFacilities.cpp
//
//  Created by Gérald Guyomard on 28/09/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeLoadingFacilities.h"

#include "DOWFoundation/Exception.h"
#include "cocoa/CCObjectPtr.h"
#include "NodeControllers/SelectorResolver.h"
#include "CCBReader/CCNodeLoaderLibrary.h"

NS_CC_EXT_BEGIN

using namespace DOW;

NodeLoadingFacilities::~NodeLoadingFacilities()
{}
    
SelectorResolver&
NodeLoadingFacilities::Resolvers::selectorResolver()
{
    if (!fSelectorResolver)
    {
        fSelectorResolver.reset(new SelectorResolver);
    }
    
    return *fSelectorResolver;
}

MemberVariableAssigner&
NodeLoadingFacilities::Resolvers::memberVariableAssigner()
{
    if (!fMemberVariableAssigner)
    {
        fMemberVariableAssigner.reset(new MemberVariableAssigner);
    }
    
    return *fMemberVariableAssigner;
}

NodeLoadingFacilities::NodeLoadingFacilities()
{
}

void
NodeLoadingFacilities::_preLoad()
{
}
    
void
NodeLoadingFacilities::_fillResolvers(Resolvers& iResolvers)
{
}

void
NodeLoadingFacilities::_load(const std::string& iResourceName)
{
    _preLoad();

    Resolvers resolvers;
    _fillResolvers(resolvers);
    
    CCNodeLoaderLibrary* lib = CCNodeLoaderLibrary::sharedCCNodeLoaderLibrary();
    
    CCObjectPtr<CCBReader> reader;
    reader << new CCBReader(lib, resolvers.memberVariableAssignerIfAny(), resolvers.selectorResolverIfAny());
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    std::string fileName = iResourceName + ".ccbi";
    CCNode* node  = reader->readNodeGraphFromFile(fileName.c_str(), owner(), size);
    if (node == nullptr)
    {
        throw Exception("Error when loading %s", fileName.c_str());
    }
    
    _postLoad(node);
}

void
NodeLoadingFacilities::_postLoad(CCNode* iLoadedRootNode)
{
    
}
    
NS_CC_EXT_END
