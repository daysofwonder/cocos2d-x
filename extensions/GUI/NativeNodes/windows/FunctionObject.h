//

#include <functional>

template <class T> class CFunctionObject : public IDispatch
{
	typedef std::function<HRESULT(DISPID id, DISPPARAMS* pDispParams,VARIANT* pVarResult)> Callback;

public:
	CFunctionObject() { m_cRef = 0; }

	~CFunctionObject() {}

	HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject)
	{
		*ppvObject = NULL;

		if (IsEqualGUID(riid, IID_IUnknown))
			*ppvObject = reinterpret_cast<void **> (this);

		if (IsEqualGUID(riid, IID_IDispatch))
			*ppvObject = reinterpret_cast<void **> (this);

		if (*ppvObject)
		{
			((IUnknown*) *ppvObject)->AddRef();
			return S_OK;
		}
		else return E_NOINTERFACE;
	}

	DWORD __stdcall AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	DWORD __stdcall Release()
	{
		if (InterlockedDecrement(&m_cRef) == 0)
		{
			delete this;
			return 0;
		}
		return m_cRef;
	}

	STDMETHOD(GetTypeInfoCount)(unsigned int FAR* pctinfo)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetTypeInfo)(unsigned int iTInfo, LCID  lcid,
		ITypeInfo FAR* FAR*  ppTInfo)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames,
		unsigned int cNames, LCID lcid, DISPID FAR* rgDispId)
	{
		return S_OK;
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid,
		WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult,
		EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		if (IID_NULL != riid) {
			return DISP_E_UNKNOWNINTERFACE;
		}
		return fCallback(dispIdMember, pDispParams, pVarResult);
	}

public:
	static CComPtr<IDispatch>  CreateEventFunctionObject(const Callback& pFunc)
	{
		CComPtr<CFunctionObject> pFO = new CFunctionObject<T>;
		pFO->fCallback = pFunc;
		return pFO;
	}

protected:
	Callback fCallback;
	long m_cRef;
};
