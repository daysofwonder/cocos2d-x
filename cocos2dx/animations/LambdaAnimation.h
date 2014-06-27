//
//  LambdaAnimation.h
//
//  Created by Gérald Guyomard on 18/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __LambdaAnimation__
#define __LambdaAnimation__

#include "animations/Animation.h"
#include <functional>

NS_CC_BEGIN

    class LambdaAnimation : public Animation
    {
    public:
        
        static CCObjectPtr<LambdaAnimation> createInstantAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda);
        static CCObjectPtr<LambdaAnimation> createLastingAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled = false);
        virtual ~LambdaAnimation();
        
        virtual bool touchesEnabled() const override;
        virtual void start() override;
        virtual void stop() override;
    
    protected:
        LambdaAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda);
        LambdaAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled);
        
        std::function<void (const AnimationPtr& iAnimation)> fStartLambda;
        
        // If stop provided, this means animations last and can be stopped later
        std::function<void (const AnimationPtr& iAnimation)> fStopLambda;
        
        bool fTouchedEnabled;
    };

NS_CC_END

#endif /* defined(__LambdaAnimation__) */
