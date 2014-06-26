//
//  RadioButtonGroup.h
//
//  Created by Gérald Guyomard on 19/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCRadioButtonGroup__
#define __CCRadioButtonGroup__

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"
#include "CCScale9Sprite.h"
#include "CCControlButton.h"
#include <map>
#include <functional>
#include "CCBReader/CCSmartLayerLoader.h"

NS_CC_EXT_BEGIN

    class CCRadioButtonGroup : public CCSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCRadioButtonGroup, CCSmartLayer);
        
        static CCRadioButtonGroup* create();
        
        // Only allows CCControlButton as children
        virtual void addChild(CCNode* child, int zOrder, int tag) override;
        virtual void removeChild(CCNode* iChild, bool cleanup) override;
        
        CCControl* controlWithTag(int iTag);
        
        virtual bool select(CCControl* iControl);
        bool selectControlWithTag(int iTag);
        
        CCControl* selectedControl();
        
        void removeControlWithTag(int iTag);
        
        void enableControl(CCControl* iControl, bool iEnabled);
        
        typedef std::function<void (CCRadioButtonGroup* iGroup, CCControl* iControl, bool iValueChanged)> TSelectCallback;
        void setSelectCallback(const TSelectCallback& iCallback) { fSelectCallback = iCallback; }

        typedef std::function<void (CCRadioButtonGroup* iGroup, CCControl* iControl, bool iSelect)> TCustomSelectionHandler;
        void setCustomSelectionHandler(const TCustomSelectionHandler& iHandler);
        
        typedef std::function<bool (CCControl*)> TIterator;
        bool iterateControls(const TIterator&);
        
    protected:
        void _onControlClicked(CCObject*, CCControlEvent);
        void _selectFirstPossibleControlIfNoSelection();
        
        CCControl* _controlFromChild(CCObject* iChild) const;
        
        TSelectCallback             fSelectCallback;
        TCustomSelectionHandler     fCustomSelectionHandler;
    };
    
    class CCRadioButtonGroupLoader : public CCSmartLayerLoader
    {
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCRadioButtonGroupLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCRadioButtonGroup);
    };

NS_CC_EXT_END


#endif /* defined(__CCRadioButtonGroup__) */
