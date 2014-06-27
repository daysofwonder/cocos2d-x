//
//  LambdaAnimation.cpp
//
//  Created by Gérald Guyomard on 18/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "animations/LambdaAnimation.h"
#include "DOWFoundation/Exception.h"
#include <cassert>

NS_CC_BEGIN

CCObjectPtr<LambdaAnimation>
LambdaAnimation::createInstantAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda)
{
    CCObjectPtr<LambdaAnimation> a;
    a << new LambdaAnimation(iStartLambda);
    return a;
}

CCObjectPtr<LambdaAnimation>
LambdaAnimation::createLastingAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled)
{
    CCObjectPtr<LambdaAnimation> a;
    a << new LambdaAnimation(iStartLambda, iStopLambda, iTouchesEnabled);
    return a;
}
    
LambdaAnimation::LambdaAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda)
: fStartLambda(iStartLambda), fStopLambda(nullptr), fTouchedEnabled(false)
{}

LambdaAnimation::LambdaAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled)
: fStartLambda(iStartLambda), fStopLambda(iStopLambda), fTouchedEnabled(iTouchesEnabled)
{}

LambdaAnimation::~LambdaAnimation()
{
}
    
bool
LambdaAnimation::touchesEnabled() const
{
    return fTouchedEnabled;
}
    
void
LambdaAnimation::start()
{
    assert(fStartLambda);
    
    AnimationPtr self = this;
    
    auto startLambda = fStartLambda;
    startLambda(self); // Used a copy of start lambda, because in some cases, start lambda call LambdaAnimation::stop() that deletes fStartLambda being run!
    
    fStartLambda = nullptr;
    
    if (fStopLambda == nullptr)
    {
        finish();
    }
}
    
void
LambdaAnimation::stop()
{
    if (fStopLambda != nullptr)
    {
        fStartLambda = nullptr;
        
        auto stopLambda = fStopLambda;
        fStopLambda = nullptr;
        stopLambda(this);
    }
    else
    {
        throw DOW::Exception("Stop not implemented in lambda animation");
    }
}
    
NS_CC_END



