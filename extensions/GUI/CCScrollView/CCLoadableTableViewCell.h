//
//  CCLoadableTableViewCell.h
//
//  Created by Gérald Guyomard on 21/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCLoadableTableViewCell__
#define __CCLoadableTableViewCell__

#include "GUI/CCScrollView/CCTableViewCell.h"
#include "NodeControllers/NodeLoadingFacilities.h"
#include "cocoa/CCObjectPtr.h"

NS_CC_EXT_BEGIN

    class CCLoadableTableViewCell : public CCTableViewCell, public NodeLoadingFacilities
    {
    public:
        
        virtual CCObject* owner() override;
        
        CCNode* rootNode() const { return fRootNode; }
        
    protected:
        CCLoadableTableViewCell();
        
        virtual void _didLoad();
        
    private:
        virtual void _postLoad(CCNode* iRoot) override;
        
        CCObjectPtr<CCNode> fRootNode;
    };

NS_CC_EXT_END

#endif /* defined(__CCLoadableTableViewCell__) */
