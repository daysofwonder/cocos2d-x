//
//  CCLambdaAction.h
//
//  Created by Gérald Guyomard on 18/12/12.
//
//

#ifndef __cocos2d_CCLambdaAction__
#define __cocos2d_CCLambdaAction__

#include <cocos2d.h>

NS_CC_BEGIN

class CC_DLL CCLambdaAction : public CCActionInterval //<NSCopying>
{
public:
    CCLambdaAction();
    
    /** creates the action with the lambda
     */
    static CCLambdaAction* create(float iDuration, const std::function<void(float iTime)>& iLambda);
    
    
	/** initializes the action with the lambda
     */
    virtual bool initWithLambda(float iDuration, const std::function<void(float iTime)>& iLambda);
    
    //super methods
    virtual void update(float time);
    CCObject * copyWithZone(CCZone *pZone);
    
protected:
    std::function<void(float iTime)> fLambda;
};

NS_CC_END


#endif /* defined(cocos2d_) */
