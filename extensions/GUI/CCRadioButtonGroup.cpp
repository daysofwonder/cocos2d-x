//
//  CCRadioButtonGroup.cpp
//
//  Created by Gérald Guyomard on 19/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/CCRadioButtonGroup.h"

NS_CC_EXT_BEGIN

    CCRadioButtonGroup*
    CCRadioButtonGroup::create()
    {
        CCRadioButtonGroup* pRet = new CCRadioButtonGroup();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pRet);
        }
        return pRet;        
    }
    
    CCControl*
    CCRadioButtonGroup::_controlFromChild(CCObject* iChild) const
    {
        return dynamic_cast<CCControl*>(iChild);
    }
    
    void
    CCRadioButtonGroup::addChild(CCNode* child, int zOrder, int tag)
    {
        SuperClass::addChild(child, zOrder, tag);
        
        CCControl* control = _controlFromChild(child);
        if (control != nullptr)
        {
            control->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_onControlClicked, CCControlEventTouchUpInside);
         
            // Transfer background in proper states
            CCControlButton* button = dynamic_cast<CCControlButton*>(control);
            if (button != nullptr)
            {
                button->setupToggle();
            }
                        
            if (control->isEnabled() && (selectedControl() == nullptr))
            {
                control->setSelected(true);
            }
            else
            {
                control->setSelected(false);
            }
        }
    }
    
    void
    CCRadioButtonGroup::_onControlClicked(CCObject* iObject, CCControlEvent)
    {
        CCControl* control = _controlFromChild(iObject);
        assert(control != nullptr);
        
        const bool changed = select(control);
        
        if (fSelectCallback != nullptr)
        {
            fSelectCallback(this, control, changed);
        }
    }
    
    bool
    CCRadioButtonGroup::select(CCControl* iControl)
    {
        if (iControl == nullptr)
        {
            return false;
        }
        
        bool changed = false;
        
        CCObject* child;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCControl* control = _controlFromChild(child);
            if (control != nullptr)
            {
                const bool selected = (control == iControl);
                if (selected)
                {
                    changed = !control->isSelected();
                }
                
                if (fCustomSelectionHandler != nullptr)
                {
                    fCustomSelectionHandler(this, control, selected);
                }
                else if (control->isEnabled())
                {
                    control->setSelected(selected);
                }
            }
        }
        
        return changed;
    }
    
    void
    CCRadioButtonGroup::enableControl(CCControl* iControl, bool iEnabled)
    {
        if (iEnabled != iControl->isEnabled())
        {
            if (!iEnabled)
            {
                iControl->setSelected(false);
            }
            
            iControl->setEnabled(iEnabled);
        }
        
        _selectFirstPossibleControlIfNoSelection();
    }
    
    void
    CCRadioButtonGroup::setCustomSelectionHandler(const TCustomSelectionHandler& iHandler)
    {
        fCustomSelectionHandler = iHandler;
        
        select(selectedControl());
    }
    
    CCControl*
    CCRadioButtonGroup::controlWithTag(int iTag)
    {
        CCObject* child;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCControl* control = _controlFromChild(child);
            if ((control != nullptr) && (control->getTag() == iTag))
            {
                return control;
            }
        }
        
        return nullptr;
    }
    
    bool
    CCRadioButtonGroup::selectControlWithTag(int iTag)
    {
        CCControl* controlToSelect = nullptr;
        
        CCObject* child;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCControl* control = _controlFromChild(child);
            if ((control != nullptr) && (control->getTag() == iTag))
            {
                controlToSelect = control;
                break;
            }
        }
        
        return select(controlToSelect);
    }
    
    void
    CCRadioButtonGroup::removeControlWithTag(int iTag)
    {
        auto* control = controlWithTag(iTag);
        if (control != nullptr)
        {
            removeChild(control, true);
        }
    }
    
    CCControl*
    CCRadioButtonGroup::selectedControl()
    {
        CCObject* child;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCControl* control = _controlFromChild(child);
            if ((control != nullptr) && control->isEnabled() && control->isSelected())
            {
                return control;
            }
        }
        
        return nullptr;
    }
    
    bool
    CCRadioButtonGroup::iterateControls(const TIterator& iIterator)
    {
        CCObject* child;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCControl* control = _controlFromChild(child);
            if (control != nullptr)
            {
                if (iIterator(control))
                {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    void
    CCRadioButtonGroup::removeChild(CCNode* iChild, bool cleanup)
    {
        CCControl* control = _controlFromChild(iChild);
        if (control != nullptr)
        {
            control->removeTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_onControlClicked, CCControlEventTouchUpInside);
        }
        
        SuperClass::removeChild(iChild, cleanup);
        
        _selectFirstPossibleControlIfNoSelection();
    }
                    
    void
    CCRadioButtonGroup::_selectFirstPossibleControlIfNoSelection()
    {
        if (selectedControl() == nullptr)
        {
            CCObject* child;
            CCARRAY_FOREACH(getChildren(), child)
            {
                CCControl* control = _controlFromChild(child);
                if ((control != nullptr) && control->isEnabled())
                {
                    select(control);
                    break;
                }
            }
        }
    }

NS_CC_EXT_END
