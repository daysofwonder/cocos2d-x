//
//  CCMultilineTextBoxImplAndroid.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/NativeNodes/android/CCMultilineTextBoxImplAndroid.h"
#include "GUI/NativeNodes/CCMultilineTextBox.h"
#include "CCOperationPool.h"
//#include "CCIMEDispatcher.h"

// Native Implementations
 extern "C"
{
	using namespace cocos2d;
	using namespace cocos2d::extension;

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_MultilineTextBoxController_textBoxDidStartEditing(JNIEnv*  env, jobject thiz, jlong iOwner)
    {
    	CCMultilineTextBoxImplAndroid* impl = reinterpret_cast<CCMultilineTextBoxImplAndroid*>(iOwner);
    	impl->textBoxDidStartEditing();
    }

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_MultilineTextBoxController_textBoxDidChange(JNIEnv*  env, jobject thiz, jlong iOwner, jstring iText)
    {
    	CCMultilineTextBoxImplAndroid* impl = reinterpret_cast<CCMultilineTextBoxImplAndroid*>(iOwner);
    	impl->textBoxDidChange(iText);
    }

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_MultilineTextBoxController_textBoxDidEndEditing(JNIEnv*  env, jobject thiz, jlong iOwner)
    {
    	CCMultilineTextBoxImplAndroid* impl = reinterpret_cast<CCMultilineTextBoxImplAndroid*>(iOwner);
    	impl->textBoxDidEndEditing();
    }

    // 	private native static void sendOpenKeyboardNotification();
	// private native static void sendCloseKeyboardNotification();

    static void _fillShowKeyboardIMENotification(CCIMEKeyboardNotificationInfo& oInfo)
    {
    	oInfo.duration = 0.25f;

    	const CCSize& screenSize = CCDirector::sharedDirector()->getWinSize();
    	oInfo.end.size.width = screenSize.width;
    	oInfo.end.size.height = screenSize.height * 0.6f;

    	oInfo.begin = oInfo.end;
    	oInfo.begin.origin.y = -oInfo.begin.size.height;
    }

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_MultilineTextBoxController_sendOpenKeyboardNotification(JNIEnv*  env, jobject thiz)
    {
    	CCIMEKeyboardNotificationInfo notif;
    	_fillShowKeyboardIMENotification(notif);

    	CCIMEDispatcher::sharedDispatcher()->dispatchKeyboardWillShow(notif);
    }

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_MultilineTextBoxController_sendCloseKeyboardNotification(JNIEnv*  env, jobject thiz)
    {
    	CCIMEKeyboardNotificationInfo notif;
    	_fillShowKeyboardIMENotification(notif);

    	std::swap(notif.begin, notif.end);

    	CCIMEDispatcher::sharedDispatcher()->dispatchKeyboardWillHide(notif);
    }
}

NS_CC_EXT_BEGIN

    std::unique_ptr<CCNativeViewImpl>
    CCMultilineTextBox::_createImpl()
    {
        std::unique_ptr<CCNativeViewImpl> impl(new CCMultilineTextBoxImplAndroid);
        return impl;
    }
    
    CCMultilineTextBoxImplAndroid::CCMultilineTextBoxImplAndroid()
    {
    }
    
    CCMultilineTextBoxImplAndroid::~CCMultilineTextBoxImplAndroid()
    {
    }
    
    static const std::string kMultilineTextBoxControllerJavaClassName = "com/daysofwonder/smallworld2/MultilineTextBoxController";

    std::string
    CCMultilineTextBoxImplAndroid::_nativeViewControllerFullName()
    {
        return kMultilineTextBoxControllerJavaClassName;
    }
 
    CCMultilineTextBox*
    CCMultilineTextBoxImplAndroid::textBox() const
    {
        return static_cast<CCMultilineTextBox*>(owner());
    }

    std::string
    CCMultilineTextBoxImplAndroid::getText() const
    {
    	auto controller = nativeViewController();

    	auto& manager = JNIManager::instance();

    	JNIMethodInfo::Ptr getText = manager.getMethodInfo(kMultilineTextBoxControllerJavaClassName, "getText", "()Ljava/lang/String;");
    	assert(getText != nullptr);
    	auto txt = getText->callTypedObjectMethod<jstring>(controller);

    	return (txt != nullptr) ? manager.jstring2string(txt->javaObject()) : "";
    }

    void
    CCMultilineTextBoxImplAndroid::setText(const std::string& iText)
    {
       	auto controller = nativeViewController();

		auto& manager = JNIManager::instance();

		JNIMethodInfo::Ptr setText = manager.getMethodInfo(kMultilineTextBoxControllerJavaClassName, "setText", "(Ljava/lang/String;)V");
		assert(setText != nullptr);

		auto text = manager.string2jstring(iText);
		setText->callVoidMethod(controller, text->javaObject());
    }

    void
    CCMultilineTextBoxImplAndroid::setFont(const std::string& pName, float iSize)
    {
       	auto controller = nativeViewController();

		auto& manager = JNIManager::instance();

		JNIMethodInfo::Ptr setFont = manager.getMethodInfo(kMultilineTextBoxControllerJavaClassName, "setFont", "(Ljava/lang/String;F)V");
		assert(setFont != nullptr);

		auto name = manager.string2jstring(pName);
		setFont->callVoidMethod(controller, name->javaObject(), (jfloat) iSize);
    }

    void
    CCMultilineTextBoxImplAndroid::setBackgroundColor(const ccColor4B& iColor)
    {
       	auto controller = nativeViewController();

		auto& manager = JNIManager::instance();

		JNIMethodInfo::Ptr setBackgroundColor = manager.getMethodInfo(kMultilineTextBoxControllerJavaClassName, "setBackgroundColor", "(BBBB)V");
		assert(setBackgroundColor != nullptr);

		setBackgroundColor->callVoidMethod(controller, (jbyte) iColor.r, (jbyte) iColor.g, (jbyte) iColor.b, (jbyte) iColor.a);
    }

    void
    CCMultilineTextBoxImplAndroid::setTextColor(const ccColor4B& iColor)
    {
      	auto controller = nativeViewController();

		auto& manager = JNIManager::instance();

		JNIMethodInfo::Ptr setTextColor = manager.getMethodInfo(kMultilineTextBoxControllerJavaClassName, "setTextColor", "(BBBB)V");
		assert(setTextColor != nullptr);

		setTextColor->callVoidMethod(controller, (jbyte) iColor.r, (jbyte) iColor.g, (jbyte) iColor.b, (jbyte) iColor.a);
    }

    void
    CCMultilineTextBoxImplAndroid::retainFocus()
    {
    	// Make sure we are visible before asking for focus
    	float rotation = 0;
    	CCRect r;
    	const bool isVisible = _computeTransform(rotation, r);
    	_setVisible(isVisible);

      	auto controller = nativeViewController();

      	JNIMethodInfo::Ptr retainFocus = JNIManager::instance().getMethodInfo(kMultilineTextBoxControllerJavaClassName, "retainFocus", "()V");
		assert(retainFocus != nullptr);

		retainFocus->callVoidMethod(controller);
    }

    void
    CCMultilineTextBoxImplAndroid::releaseFocus()
    {
      	auto controller = nativeViewController();

      	JNIMethodInfo::Ptr releaseFocus = JNIManager::instance().getMethodInfo(kMultilineTextBoxControllerJavaClassName, "releaseFocus", "()V");
		assert(releaseFocus != nullptr);

		releaseFocus->callVoidMethod(controller);
    }

    void
     CCMultilineTextBoxImplAndroid::textBoxDidStartEditing()
     {
       	CCOperationPool::instance()->executeInMainThread(
       			[this]()
       			{
       				CCMultilineTextBox* box = textBox();
       				auto* delegate = box->delegate();
       				if (delegate != nullptr)
       				{
       					delegate->textBoxEditingDidBegin(box);
       				}
       			}
       			);
       }

    void
    CCMultilineTextBoxImplAndroid::textBoxDidChange(jstring iText)
    {
    	std::string text = JNIManager::instance().jstring2string(iText);

    	CCOperationPool::instance()->executeInMainThread(
    			[this, text]()
    			{
    				CCMultilineTextBox* box = textBox();
    				auto* delegate = box->delegate();
    				if (delegate != nullptr)
    				{
    					delegate->textBoxTextChanged(box, text);
    				}
    			}
    			);
    }

    void
    CCMultilineTextBoxImplAndroid::textBoxDidEndEditing()
    {
      	CCOperationPool::instance()->executeInMainThread(
      			[this]()
      			{
      				CCMultilineTextBox* box = textBox();
      				auto* delegate = box->delegate();
      				if (delegate != nullptr)
      				{
      					delegate->textBoxEditingDidEnd(box);
      				}
      			}
      			);
      }

NS_CC_EXT_END
