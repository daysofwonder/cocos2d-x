//
//  SpawnAnimation.h
//
//  Created by Gérald Guyomard on 19/11/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __SpawnAnimation__
#define __SpawnAnimation__

#include "animations/Animation.h"
#include <functional>
#include <vector>

NS_CC_BEGIN

    class SpawnAnimation;
    typedef CCObjectPtr<SpawnAnimation> SpawnAnimationPtr;
    
    typedef std::function<void (const SpawnAnimationPtr& iAnimation)> TPrepareStartLambda;
    
    class SpawnAnimation : public Animation, protected AnimationDelegate
    {
    public:
        static SpawnAnimationPtr create(const std::vector<AnimationPtr>& iAnimations);
        static SpawnAnimationPtr create();

        virtual ~SpawnAnimation();
        
        virtual void start() override;
        virtual void stop() override;
        
        void addAnimation(const AnimationPtr& iAnimation);
        void setPrepareStart(const TPrepareStartLambda& iLambda);

    private:
        SpawnAnimation();
        SpawnAnimation(const std::vector<AnimationPtr>& iAnimations);

        virtual void animationDidStop(const AnimationPtr& iAnimation, bool iFinished) override;

        virtual bool _pause() override;
        virtual void _resume() override;
        
        std::vector<AnimationPtr>   fAnimations;
        TPrepareStartLambda         fPrepareStart;
    };

NS_CC_END

#endif /* defined(__SpawnAnimation__) */
