//
//  CCStylesProtocol.cpp
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCStylesProtocol.h"
#include "DOWFoundation/StringUtils.h"
#include "DOWFoundation/Exception.h"

NS_CC_BEGIN

void
CCStylesProtocol::setShadowColorFromPlainText(const std::string& iShadowColor)
{
    ccColor4B color = ccColor4B::decode(iShadowColor);
    setShadowColor(color);
}

void
CCStylesProtocol::setShadowOffsetFromPlainText(const std::string& iShadowOffset)
{
    CCSize offset = CCSize::decode(iShadowOffset);
    setShadowOffset(offset);
}

void
CCStylesProtocol::setOutlineColorFromPlainText(const std::string& iOutlineColor)
{    
    ccColor4B color = ccColor4B::decode(iOutlineColor);
    setOutlineColor(color);
}

NS_CC_END