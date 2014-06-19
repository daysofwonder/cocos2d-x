//
//  CCObjectPtr.h
//
//  Created by Gérald Guyomard on 28/09/12.
//
//

#ifndef __CCObjectPtr__
#define __CCObjectPtr__

#include "CCObject.h"

NS_CC_BEGIN

    template <class TObject> class CCObjectPtr
    {
    public:
        CCObjectPtr();
        CCObjectPtr(TObject* iObject);
        CCObjectPtr(const CCObjectPtr<TObject>& iOther);

        template <class TOther> CCObjectPtr(const CCObjectPtr<TOther>& iOther)
        : fObject(iOther.fObject)
        {
            if (fObject != nullptr)
            {
                fObject->retain();
            }
        }

        template <class TOther> CCObjectPtr(CCObjectPtr<TOther>&& iOther)
        : fObject(iOther.fObject)
        {
            iOther.fObject = nullptr;
        }

        ~CCObjectPtr();

        CCObjectPtr& operator=(TObject* iObject);
        CCObjectPtr& operator=(const CCObjectPtr<TObject>& iOther);

        template <class TOther> CCObjectPtr& operator=(const CCObjectPtr<TOther>& iOther)
        {
            if (fObject != iOther.fObject)
            {
                if (fObject != nullptr)
                {
                    fObject->release();
                }

                fObject = iOther.fObject;

                if (fObject != nullptr)
                {
                    fObject->retain();
                }
            }

            return *this;
        }

        CCObjectPtr& operator=(CCObjectPtr<TObject>&& iOther);

        template <class TOther> CCObjectPtr& operator=(CCObjectPtr<TOther>&& iOther)
        {
            if (fObject != iOther.fObject)
            {
                if (fObject != nullptr)
                {
                    fObject->release();
                }

                fObject = iOther.fObject;
                iOther.fObject = nullptr;
            }

            return *this;
        }

        // This flavor does not retain the incoming object (suitable for newMethods returning objects with +1 as retain count)
        CCObjectPtr& operator<<(TObject* iObject);

        TObject* operator->() const;
        operator TObject*() const;
        TObject* get() const { return fObject; }

        TObject& operator*() const;

    private:
        template <class TOther> friend class CCObjectPtr;

        TObject* fObject;
    };

    // Inlined implementations
    template <class TObject> inline CCObjectPtr<TObject>::CCObjectPtr()
    : fObject(nullptr)
    {
    }

    template <class TObject> inline CCObjectPtr<TObject>::CCObjectPtr(TObject* iObject)
    : fObject(iObject)
    {
        if (fObject != nullptr)
        {
            fObject->retain();
        }
    }

    template <class TObject> inline CCObjectPtr<TObject>::CCObjectPtr(const CCObjectPtr<TObject>& iOther)
    : fObject(iOther.fObject)
    {
        if (fObject != nullptr)
        {
            fObject->retain();
        }
    }

    template <class TObject> inline CCObjectPtr<TObject>::~CCObjectPtr()
    {
        if (fObject != nullptr)
        {
            fObject->release();
        }
    }

    template <class TObject> inline CCObjectPtr<TObject>& CCObjectPtr<TObject>::operator=(TObject* iObject)
    {
        if (fObject != iObject)
        {
            if (fObject != nullptr)
            {
                fObject->release();
            }

            fObject = iObject;
            if (fObject != nullptr)
            {
                fObject->retain();
            }
        }

        return *this;
    }

    template <class TObject> inline CCObjectPtr<TObject>& CCObjectPtr<TObject>::operator=(const CCObjectPtr<TObject>& iOther)
    {
        if (fObject != iOther.fObject)
        {
            if (fObject != nullptr)
            {
                fObject->release();
            }

            fObject = iOther.fObject;
            if (fObject != nullptr)
            {
                fObject->retain();
            }
        }

        return *this;
    }

    template <class TObject> inline CCObjectPtr<TObject>& CCObjectPtr<TObject>::operator=(CCObjectPtr<TObject>&& iOther)
    {
        if (fObject != iOther.fObject)
        {
            if (fObject != nullptr)
            {
                fObject->release();
            }

            fObject = iOther.fObject;
            iOther.fObject = nullptr;
        }

        return *this;
    }

    template <class TObject> inline CCObjectPtr<TObject>& CCObjectPtr<TObject>::operator<<(TObject* iObject)
    {
        if (fObject != iObject)
        {
            if (fObject != nullptr)
            {
                fObject->release();
            }

            fObject = iObject;
        }

        return *this;
    }

    template <class TObject> inline TObject* CCObjectPtr<TObject>::operator->() const
    {
        return fObject;
    }

    template <class TObject> inline CCObjectPtr<TObject>::operator TObject*() const
    {
        return fObject;
    }

    template <class TObject> inline TObject& CCObjectPtr<TObject>::operator*() const
    {
        assert(fObject != nullptr);
        return *fObject;
    }

NS_CC_END

#endif /* defined(__CCObjectPtr__) */
