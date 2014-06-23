//
//  CCCallLambda.h
//
//  Created by Gérald Guyomard on 16/10/12.
//
//

#ifndef __cocos2d_CCCallLambda__
#define __cocos2d_CCCallLambda__

#include <cocos2d.h>

NS_CC_BEGIN

class CC_DLL CCCallLambda : public CCActionInstant //<NSCopying>
{
public:
    CCCallLambda();
 
    /** creates the action with the lambda
     */
    static CCCallLambda* create(const std::function<void()>& iLambda);
    
    
	/** initializes the action with the lambda
     */
    virtual bool initWithLambda(const std::function<void()>& iLambda);

    //super methods
    virtual void update(float time);
    CCObject * copyWithZone(CCZone *pZone);
    
protected:
    std::function<void()> fLambda;
};

NS_CC_END

#endif /* defined(__cocos2d_CCCallLambda__) */
