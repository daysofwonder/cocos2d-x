//
//  CCMultilineTextBox.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCMultilineTextBox.h"
#include "CCMultilineTextBoxImpl.h"
#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"
#include "DOWFoundation/StringUtils.h"

NS_CC_EXT_BEGIN

using namespace DOW;

    CCMultilineTextBox*
    CCMultilineTextBox::create()
    {
        CCMultilineTextBox* pRet = new CCMultilineTextBox();
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
    
    CCMultilineTextBox::~CCMultilineTextBox()
    {
    }
        
    CCMultilineTextBoxImpl*
    CCMultilineTextBox::_multiLineTextBoxImpl() const
    {
        CCMultilineTextBoxImpl* impl = dynamic_cast<CCMultilineTextBoxImpl*>(_impl());
        assert(impl != nullptr);
        return impl;
    }
        
    CCMultilineTextBox::CCMultilineTextBox()
    : fDelegate(nullptr)
    {
    }
    
    std::string
    CCMultilineTextBox::getText() const
    {
        return _multiLineTextBoxImpl()->getText();
    }
    
    void
    CCMultilineTextBox::setText(const std::string& iText)
    {
        _multiLineTextBoxImpl()->setText(iText);
    }
    
    void
    CCMultilineTextBox::setFont(const std::string& pName, float iSize)
    {
        _multiLineTextBoxImpl()->setFont(pName, iSize);
    }

    void
    CCMultilineTextBox::setBackgroundColor(const ccColor4B& iColor)
    {
        _multiLineTextBoxImpl()->setBackgroundColor(iColor);
    }
    
    void
    CCMultilineTextBox::setTextColor(const ccColor4B& iColor)
    {
        _multiLineTextBoxImpl()->setTextColor(iColor);
    }

    void
    CCMultilineTextBox::setBackgroundColorAsString(const std::string& iColorAsString)
    {
        ccColor4B color = ccColor4B::decode(iColorAsString);
        setBackgroundColor(color);
    }
    
    void
    CCMultilineTextBox::setTextColorAsString(const std::string& iColorAsString)
    {
        ccColor4B color = ccColor4B::decode(iColorAsString);
        setTextColor(color);
    }

    void
    CCMultilineTextBox::setFontAsString(const std::string& pFontAsString)
    {
        std::vector<std::string> comps = StringUtils::componentsSeparatedByString(pFontAsString, ",");

        if (!comps.empty())
        {
            float size = 10;
            std::string fontName = comps[0];
            if (comps.size() >= 2)
            {
                size = StringUtils::readFloat(comps[1]);
            }
            
            setFont(fontName, size);
        }
    }
    
    void
    CCMultilineTextBox::retainFocus()
    {
        _multiLineTextBoxImpl()->retainFocus();
    }
    
    void
    CCMultilineTextBox::releaseFocus()
    {
        _multiLineTextBoxImpl()->releaseFocus();
    }

    void
    CCMultilineTextBoxLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
    {
        CCMultilineTextBox* textBox = static_cast<CCMultilineTextBox*>(pNode);
        assert(dynamic_cast<CCMultilineTextBox*>(pNode) != nullptr);

        if (::strcmp(pPropertyName, "text") == 0)
        {
            textBox->setText(pString);
        }
        else if (::strcmp(pPropertyName, "font") == 0)
        {
            textBox->setFontAsString(pString);
        }
        else if (::strcmp(pPropertyName, "backgroundColor") == 0)
        {
            textBox->setBackgroundColorAsString(pString);
        }
        else if (::strcmp(pPropertyName, "textColor") == 0)
        {
            textBox->setTextColorAsString(pString);
        }
        else
        {
            SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
        }
    }

NS_CC_EXT_END
