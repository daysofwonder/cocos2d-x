//
//  CCEnhancedTableView.cpp
//
//  Created by Gérald Guyomard on 5/12/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCEnhancedTableView.h"
#include "misc_nodes/CCTooltip.h"

NS_CC_EXT_BEGIN
    
    bool
    CCEnhancedTableView::canHandleMouseOver()
    {
        return isEnabled();
    }
    
    bool
    CCEnhancedTableView::isMouseOver(const CCPoint& iWorldMouseLocation)
    {
        CCRect worldBox = boundingBox();
        
        CCNode* parent = getParent();
        if (parent != nullptr)
        {
            worldBox = CCRectApplyAffineTransform(worldBox, parent->nodeToWorldTransform());
        }
        
        return worldBox.containsPoint(iWorldMouseLocation);
    }
    
    void
    CCEnhancedTableView::onMouseEnter(const CCPoint& iWorldMouseLocation)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->onMouseEnter(iWorldMouseLocation);
        }
    }
    
    void
    CCEnhancedTableView::onMouseOver(const CCPoint& iWorldMouseLocation)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->onMouseOver(iWorldMouseLocation);
        }
    }
    
    void
    CCEnhancedTableView::onMouseExit(const CCPoint& iWorldMouseLocation)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->onMouseExit(iWorldMouseLocation);
        }
    }
    
    void
    CCEnhancedTableView::setTooltipText(const std::string& iText)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->terminate();
        }
        
        fTooltip = CCTextTooltip::create(iText);
    }
    
    void
    CCEnhancedTableView::setTooltipLocalizedTextKey(const std::string& iKey)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->terminate();
        }
        
        fTooltip = CCTextTooltip::createWithLocalizedTextKey(iKey);
    }

    
    CCEnhancedTableView*
    CCEnhancedTableViewLoader::createCCNode(cocos2d::CCNode * pParent, CCBReader * pCCBReader)
    {
        CCEnhancedTableView* view = new CCEnhancedTableView;
        view->initWithViewSize(CCSizeMake(200, 200), NULL);
        
        view->autorelease();
        return view;
    }
    
    void
    CCEnhancedTableViewLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
    {
        CCEnhancedTableView* tableView = static_cast<CCEnhancedTableView*>(pNode);
        assert(dynamic_cast<CCEnhancedTableView*>(pNode) != nullptr);
        
        if (::strcmp(pPropertyName, "tooltipText") == 0)
        {
            tableView->setTooltipText(pString);
        }
        else if (::strcmp(pPropertyName, "tooltipLocalizedStringKey") == 0)
        {
            tableView->setTooltipLocalizedTextKey(pString);
        }
        else
        {
            SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
        }
    }

NS_CC_EXT_END
