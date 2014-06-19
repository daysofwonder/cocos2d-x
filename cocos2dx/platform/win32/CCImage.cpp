/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#define __CC_PLATFORM_IMAGE_CPP__
#include "platform/CCImageCommon_cpp.h"
#include "ccMacros.h"
#include "CCDirector.h"
#include "Dwrite.h"
#include "d2d1.h"
#include "atlbase.h"
#include "Wincodec.h"

NS_CC_BEGIN

wchar_t * utf8ToUtf16(const std::string& nString)
{
	wchar_t * pwszBuffer = NULL;
	do
	{
		if (nString.size() < 0)
		{
			break;
		}
		// utf-8 to utf-16
		int nLen = nString.size();
		int nBufLen = nLen + 1;
		pwszBuffer = new wchar_t[nBufLen];
		CC_BREAK_IF(!pwszBuffer);
		memset(pwszBuffer, 0, nBufLen);
		nLen = MultiByteToWideChar(CP_UTF8, 0, nString.c_str(), nLen, pwszBuffer, nBufLen);
		pwszBuffer[nLen] = '\0';
	} while (0);
	return pwszBuffer;

}

std::wstring utf8ToUtf16String(const std::string& nString)
{
	wchar_t* s = utf8ToUtf16(nString);
	std::wstring utf16Str = s;
	delete[] s;

	return utf16Str;
}

class DirectWriteManager
{
public:
	static DirectWriteManager* instance();

	void calculateStringSize(const wchar_t* pText,
		const char *    pFontName,
		int             nFontSize,

		CCSize&         oComputedSize,
		int&            oAdjustedFontSize,

		int            nMinFontSize = 10,
		int             nWidth = 0,
		int             nHeight = 0,
		float scale = CC_CONTENT_SCALE_FACTOR()
		);

	bool renderText
	(
		CCImage*		iDstImage,
		const char *    pText,
		int             nWidth/* = 0*/,
		int             nHeight/* = 0*/,
		CCImage::ETextAlign      eAlignMask/* = kAlignCenter*/,
		const char *    pFontName/* = nil*/,
		int             nSize/* = 0*/);

	CCSize dipToPixel(const CCSize& iDip) const;

private:
	DirectWriteManager(IDWriteFactory* idwriteFactory, ID2D1Factory* id2dFactory, IWICImagingFactory* iWICFactory);
	~DirectWriteManager();

	FLOAT ConvertPointSizeToDIP(FLOAT points)
	{
		return (points / 72.0f)*96.0f;
	}

	CComPtr<IDWriteTextLayout> _createLayout(const wchar_t* pText, const char* pFontName, int fontSize, int maxWidth, int maxHeight, float iScale, DWRITE_TEXT_RANGE* oRange = NULL);

	CComPtr<IDWriteFactory> fDwriteFactory;
	CComPtr<ID2D1Factory> fD2dFactory;
	CComPtr<IWICImagingFactory> fWICFactory;

	CComPtr<ID2D1HwndRenderTarget> fRenderTarget;

	FLOAT fDPiRatioX = 0;
	FLOAT fDPiRatioY = 0;
};

// Direct Write Support
DirectWriteManager* DirectWriteManager::instance()
{
	if (!CCImage::isDirectWriteTextRenderingEnabled())
	{
		return NULL;
	}

	static DirectWriteManager* s_Instance = NULL;
	static bool s_InstanceInited = false;
	if (!s_InstanceInited)
	{
		assert(s_Instance == NULL);
		s_InstanceInited = true;

		CComPtr<IDWriteFactory> dwriteFactory;
		CComPtr<ID2D1Factory> d2dFactory;

		// DirectWrite
		HMODULE dwriteModule = LoadLibrary(L"Dwrite.dll");
		if (dwriteModule != NULL)
		{
			/* EXTERN_C HRESULT DWRITE_EXPORT DWriteCreateFactory(
			__in DWRITE_FACTORY_TYPE factoryType,
				__in REFIID iid,
				__out IUnknown **factory
				);*/

			typedef HRESULT(WINAPI *TCreateFactory)(DWRITE_FACTORY_TYPE factoryType, _In_   REFIID iid, _Out_  IUnknown **factory);
			TCreateFactory createFactory = (TCreateFactory)GetProcAddress(dwriteModule, "DWriteCreateFactory");
			if (createFactory != NULL)
			{
				CComPtr<IUnknown> baseInterface;
				HRESULT hr = createFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &baseInterface);
				if ((hr == S_OK) && (baseInterface != NULL))
				{
					baseInterface->QueryInterface(__uuidof(IDWriteFactory), (void**)&dwriteFactory);
				}
			}
		}

		// Direct2D
		HMODULE d2dModule = LoadLibrary(L"D2d1.dll");
		if (d2dModule != NULL)
		{
			/*     HRESULT WINAPI
			D2D1CreateFactory(
				__in D2D1_FACTORY_TYPE factoryType,
				__in REFIID riid,
				__in_opt CONST D2D1_FACTORY_OPTIONS *pFactoryOptions,
				__out void **ppIFactory
				);*/

			typedef HRESULT(WINAPI *TCreateFactory)(__in D2D1_FACTORY_TYPE factoryType,
				__in REFIID riid,
				__in_opt CONST D2D1_FACTORY_OPTIONS *pFactoryOptions,
				__out void **ppIFactory);

			TCreateFactory createFactory = (TCreateFactory)GetProcAddress(d2dModule, "D2D1CreateFactory");
			if (createFactory != NULL)
			{
				CComPtr<IUnknown> baseInterface;

				D2D1_FACTORY_OPTIONS options;
				options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

				HRESULT hr = createFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &options, reinterpret_cast<void **> (&baseInterface));
				if ((hr == S_OK) && (baseInterface != NULL))
				{
					baseInterface->QueryInterface(__uuidof(ID2D1Factory), (void**)&d2dFactory);
				}
			}
		}

		// WIC Factory
		CComPtr<IWICImagingFactory> wicFactory;
		HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

		if ((dwriteFactory != NULL) && (d2dFactory != NULL) && (wicFactory != NULL))
		{
			s_Instance = new DirectWriteManager(dwriteFactory, d2dFactory, wicFactory);
		}
	}

	return s_Instance;
}

DirectWriteManager::DirectWriteManager(IDWriteFactory* iDwriteFactory, ID2D1Factory* id2dFactory, IWICImagingFactory* iWICFactory)
: fDwriteFactory(iDwriteFactory), fD2dFactory(id2dFactory), fWICFactory(iWICFactory)
{
	fD2dFactory->GetDesktopDpi(&fDPiRatioX, &fDPiRatioY);
	fDPiRatioX /= 96;
	fDPiRatioY /= 96;
}

DirectWriteManager::~DirectWriteManager()
{
}


CCSize
DirectWriteManager::dipToPixel(const CCSize& iDip) const
{
	CCSize s;
	s.width = iDip.width * fDPiRatioX;
	s.height = iDip.height * fDPiRatioY;

	return s;
}

CComPtr<IDWriteTextLayout>
DirectWriteManager::_createLayout(const wchar_t* pText, const char* pFontName, int nFontSize, int nWidth, int nHeight, float scale, DWRITE_TEXT_RANGE* oRange)
{
	int fontSize = nFontSize * scale;
	const int maxWidth = nWidth * scale;
	const int maxHeight = nHeight * scale;

	CComPtr<IDWriteTextFormat> format;
	HRESULT hr = fDwriteFactory->CreateTextFormat(utf8ToUtf16String(pFontName).c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		ConvertPointSizeToDIP(fontSize), L"en-US", &format);

	if (SUCCEEDED(hr))
	{
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		format->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
	}

	DWRITE_TEXT_RANGE range;
	range.startPosition = 0;
	range.length = wcslen(pText);

	CComPtr<IDWriteTextLayout> layout;

	const FLOAT kMaxWidthOrInfinite = (maxWidth > 0) ? maxWidth : 1e6f;
	const FLOAT kMaxHeightOrInfinite = (maxHeight > 0) ? maxHeight : 1e6f;

	hr = fDwriteFactory->CreateTextLayout(pText, range.length, format, kMaxWidthOrInfinite, kMaxHeightOrInfinite, &layout);
	if (FAILED(hr))
	{
		return NULL;
	}

	if (oRange != NULL)
	{
		*oRange = range;
	}

	return layout;
}

void
DirectWriteManager::calculateStringSize(const wchar_t* pText,
										const char *    pFontName,
										int             nFontSize,

										CCSize&         oComputedSize,
										int&            oAdjustedFontSize,

										int            nMinFontSize,
										int             nWidth,
										int             nHeight,
										float scale
)
{
	DWRITE_TEXT_RANGE range;
	CComPtr<IDWriteTextLayout> layout = _createLayout(pText, pFontName, nFontSize, nWidth, nHeight, scale, &range);
	if (layout == NULL)
	{
		return;
	}

	range.startPosition = 0;
	range.length = wcslen(pText);

	int fontSize = nFontSize * scale;
	const int minFontSize = nMinFontSize * scale;
	const int maxWidth = nWidth * scale;
	const int maxHeight = nHeight * scale;

	while (fontSize >= minFontSize)
	{
		HRESULT hr = layout->SetFontSize(ConvertPointSizeToDIP(fontSize), range);
		if (FAILED(hr))
		{
			break;
		}

		DWRITE_TEXT_METRICS metrics;
		hr = layout->GetMetrics(&metrics);
		if (SUCCEEDED(hr))
		{
			oComputedSize.width = metrics.width;
			oComputedSize.height = metrics.height;

			oComputedSize = dipToPixel(oComputedSize);
		}

		if ((maxWidth != 0 && oComputedSize.width > maxWidth) || (maxHeight != 0 && oComputedSize.height > maxHeight)) {
			fontSize--;
		}
		else {
			break;
		}
	}

	// ... But at the end, we must convert back to original scale the output parameters.
	oComputedSize.width /= scale;
	oComputedSize.height /= scale;

	oAdjustedFontSize = fontSize / scale;
}

bool
DirectWriteManager::renderText
						(
						CCImage*		iDstImage,
						const char *    pText,
						int             nWidth/* = 0*/,
						int             nHeight/* = 0*/,
						CCImage::ETextAlign  eAlignMask/* = kAlignCenter*/,
						const char *    pFontName/* = nil*/,
						int             fontSize/* = 0*/)
{
	if ((nWidth == 0) || (nHeight == 0))
	{
		CCSize imageSize;
		int adjustedFontSize = 0;
		calculateStringSize(utf8ToUtf16String(pText).c_str(), pFontName, fontSize, imageSize, adjustedFontSize, fontSize, nWidth, nHeight, 1.f);

		nWidth = imageSize.width;
		nHeight = imageSize.height;
	}

	if ((nWidth == 0) || (nHeight == 0))
	{
		// Still empty ? Render nothing
		return true;
	}

	CComPtr<IWICBitmap> bitmap;
	if (FAILED(fWICFactory->CreateBitmap(nWidth, nHeight, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &bitmap)))
	{
		return false;
	}

	D2D1_RENDER_TARGET_PROPERTIES props;
	props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	
	props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

	props.dpiX = props.dpiY = 0;

	props.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

	CComPtr<ID2D1RenderTarget> renderTarget;
	HRESULT hr = fD2dFactory->CreateWicBitmapRenderTarget(bitmap, props, &renderTarget);
	if (FAILED(hr))
	{
		return false;
	}

	renderTarget->BeginDraw();
	{
		renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		renderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

		CComPtr<IDWriteTextLayout> layout = _createLayout(utf8ToUtf16String(pText).c_str(), pFontName, fontSize, nWidth, nHeight, 1.f);
		D2D_POINT_2F origin;
		origin.x = origin.y = 0;

		D2D1_COLOR_F brushColor;
		brushColor.r = brushColor.g = brushColor.b = 1;
		brushColor.a = 1;

		CComPtr<ID2D1SolidColorBrush> brush;
		hr = renderTarget->CreateSolidColorBrush(brushColor, &brush);
		if (FAILED(hr))
		{
			return false;
		}

		renderTarget->DrawTextLayout(origin, layout, brush);
	}
	hr = renderTarget->EndDraw();
	if (FAILED(hr))
	{
		return false;
	}

	// Now extract the data from the bitmap
	UINT bitmapWidth, bitmapHeight;
	hr = bitmap->GetSize(&bitmapWidth, &bitmapHeight);
	if (FAILED(hr))
	{
		return false;
	}

	WICRect rect;
	rect.X = rect.Y = 0;
	rect.Width = bitmapWidth;
	rect.Height = bitmapHeight;

	CComPtr<IWICBitmapLock> lock;
	hr = bitmap->Lock(&rect, WICBitmapLockRead, &lock);
	if (FAILED(hr))
	{
		return false;
	}

	UINT cbStride = 0;
	hr = lock->GetStride(&cbStride);
	if (FAILED(hr))
	{
		return false;
	}

	UINT bufferSize = 0;
	BYTE* buffer = NULL;

	hr = lock->GetDataPointer(&bufferSize, &buffer);
	if (FAILED(hr))
	{
		return false;
	}

	if (!iDstImage->initWithImageData(buffer, bufferSize, CCImage::kFmtRawData, bitmapWidth, bitmapHeight, 8))
	{
		return false;
	}

	iDstImage->setIsPremultipliedAlpha(true);

	return true;
}

/**
@brief    A memory DC which uses to draw text on bitmap.
*/
class BitmapDC
{
public:
    BitmapDC(HWND hWnd = NULL)
    : m_hDC(NULL)
    , m_hBmp(NULL)
    , m_hFont((HFONT)GetStockObject(DEFAULT_GUI_FONT))
    , m_hWnd(NULL)
    {
        m_hWnd = hWnd;
        HDC hdc = GetDC(hWnd);
        m_hDC   = CreateCompatibleDC(hdc);
        ReleaseDC(hWnd, hdc);
    }

    ~BitmapDC()
    {
        prepareBitmap(0, 0);
        if (m_hDC)
        {
            DeleteDC(m_hDC);
        }
        HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (hDefFont != m_hFont)
        {
            DeleteObject(m_hFont);
            m_hFont = hDefFont;
        }
		// release temp font resource	
		if (m_curFontPath.size() > 0)
		{
			wchar_t * pwszBuffer = utf8ToUtf16(m_curFontPath);
			if (pwszBuffer)
			{
				RemoveFontResource(pwszBuffer);
				SendMessage( m_hWnd, WM_FONTCHANGE, 0, 0);
				delete [] pwszBuffer;
				pwszBuffer = NULL;
			}
		}
    }

    bool setFont(const char * pFontName = NULL, int nSize = 0)
    {
        bool bRet = false;
        do 
        {
            std::string fontName = pFontName;
            std::string fontPath;
            HFONT       hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            LOGFONTA    tNewFont = {0};
            LOGFONTA    tOldFont = {0};
            GetObjectA(hDefFont, sizeof(tNewFont), &tNewFont);
            if (fontName.c_str())
            {    
                // create font from ttf file
                int nFindttf = fontName.find(".ttf");
                int nFindTTF = fontName.find(".TTF");
                if (nFindttf >= 0 || nFindTTF >= 0)
                {
                    fontPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(fontName.c_str());
                    int nFindPos = fontName.rfind("/");
                    fontName = &fontName[nFindPos+1];
                    nFindPos = fontName.rfind(".");
                    fontName = fontName.substr(0,nFindPos);                
                }
                tNewFont.lfCharSet = DEFAULT_CHARSET;
                strcpy_s(tNewFont.lfFaceName, LF_FACESIZE, fontName.c_str());
            }
            if (nSize)
            {
                tNewFont.lfHeight = -nSize;
            }
            GetObjectA(m_hFont,  sizeof(tOldFont), &tOldFont);

            if (tOldFont.lfHeight == tNewFont.lfHeight
                && 0 == strcmp(tOldFont.lfFaceName, tNewFont.lfFaceName))
            {
                // already has the font 
                bRet = true;
                break;
            }

            // delete old font
            if (m_hFont != hDefFont)
            {
                DeleteObject(m_hFont);
				// release old font register
				if (m_curFontPath.size() > 0)
				{
					wchar_t * pwszBuffer = utf8ToUtf16(m_curFontPath);
					if (pwszBuffer)
					{
						if(RemoveFontResource(pwszBuffer))
						{
							SendMessage( m_hWnd, WM_FONTCHANGE, 0, 0);
						}						
						delete [] pwszBuffer;
						pwszBuffer = NULL;
					}
				}
				if (fontPath.size() > 0)
					m_curFontPath = fontPath;
				else
					m_curFontPath.clear();
				// register temp font
				if (m_curFontPath.size() > 0)
				{
					wchar_t * pwszBuffer = utf8ToUtf16(m_curFontPath);
					if (pwszBuffer)
					{
						if(AddFontResource(pwszBuffer))
						{
							SendMessage( m_hWnd, WM_FONTCHANGE, 0, 0);
						}						
						delete [] pwszBuffer;
						pwszBuffer = NULL;
					}
				}
            }
            m_hFont = NULL;

            // enable Cleartype
			tNewFont.lfQuality = CLEARTYPE_QUALITY;

            // create new font
            m_hFont = CreateFontIndirectA(&tNewFont);
            if (! m_hFont)
            {
                // create failed, use default font
                m_hFont = hDefFont;
                break;
            }
            
            bRet = true;
        } while (0);
        return bRet;
    }

    SIZE sizeWithText(const wchar_t * pszText, int nLen, DWORD dwFmt, LONG nWidthLimit)
    {
        SIZE tRet = {0};
        do 
        {
            CC_BREAK_IF(! pszText || nLen <= 0);

            RECT rc = {0, 0, 0, 0};
            DWORD dwCalcFmt = DT_CALCRECT;

            if (nWidthLimit > 0)
            {
                rc.right = nWidthLimit;
                dwCalcFmt |= DT_WORDBREAK
                    | (dwFmt & DT_CENTER)
                    | (dwFmt & DT_RIGHT);
            }
            // use current font to measure text extent
            HGDIOBJ hOld = SelectObject(m_hDC, m_hFont);

            // measure text size
            const int height = DrawTextW(m_hDC, pszText, nLen, &rc, dwCalcFmt);
            SelectObject(m_hDC, hOld);

            tRet.cx = rc.right;
            tRet.cy = rc.bottom;
        } while (0);

        return tRet;
    }

    bool prepareBitmap(int nWidth, int nHeight)
    {
        // release bitmap
        if (m_hBmp)
        {
            DeleteObject(m_hBmp);
            m_hBmp = NULL;
        }
        if (nWidth > 0 && nHeight > 0)
        {
            m_hBmp = CreateBitmap(nWidth, nHeight, 1, 32, NULL);
            if (! m_hBmp)
            {
                return false;
            }
        }
        return true;
    }

    int drawText(const char * pszText, SIZE& tSize, CCImage::ETextAlign eAlign)
    {
        int nRet = 0;
        wchar_t * pwszBuffer = 0;
        do 
        {
            CC_BREAK_IF(! pszText);

            DWORD dwFmt = DT_WORDBREAK;
            DWORD dwHoriFlag = eAlign & 0x0f;
            DWORD dwVertFlag = (eAlign & 0xf0) >> 4;

            switch (dwHoriFlag)
            {
            case 1: // left
                dwFmt |= DT_LEFT;
                break;
            case 2: // right
                dwFmt |= DT_RIGHT;
                break;
            case 3: // center
                dwFmt |= DT_CENTER;
                break;
            }

            int nLen = strlen(pszText);
            // utf-8 to utf-16
            int nBufLen  = nLen + 1;
            pwszBuffer = new wchar_t[nBufLen];
            CC_BREAK_IF(! pwszBuffer);

            memset(pwszBuffer, 0, sizeof(wchar_t)*nBufLen);
            nLen = MultiByteToWideChar(CP_UTF8, 0, pszText, nLen, pwszBuffer, nBufLen);

            SIZE newSize = sizeWithText(pwszBuffer, nLen, dwFmt, tSize.cx);

            RECT rcText = {0};
            // if content width is 0, use text size as content size
            if (tSize.cx <= 0)
            {
                tSize = newSize;
                rcText.right  = newSize.cx;
                rcText.bottom = newSize.cy;
            }
            else
            {

                LONG offsetX = 0;
                LONG offsetY = 0;
                rcText.right = newSize.cx; // store the text width to rectangle

                // calculate text horizontal offset
                if (1 != dwHoriFlag          // and text isn't align to left
                    && newSize.cx < tSize.cx)   // and text's width less then content width,
                {                               // then need adjust offset of X.
                    offsetX = (2 == dwHoriFlag) ? tSize.cx - newSize.cx     // align to right
                        : (tSize.cx - newSize.cx) / 2;                      // align to center
                }

                // if content height is 0, use text height as content height
                // else if content height less than text height, use content height to draw text
                if (tSize.cy <= 0)
                {
                    tSize.cy = newSize.cy;
                    dwFmt   |= DT_NOCLIP;
                    rcText.bottom = newSize.cy; // store the text height to rectangle
                }
                else if (tSize.cy < newSize.cy)
                {
                    // content height larger than text height need, clip text to rect
                    rcText.bottom = tSize.cy;
                }
                else
                {
                    rcText.bottom = newSize.cy; // store the text height to rectangle

                    // content larger than text, need adjust vertical position
                    dwFmt |= DT_NOCLIP;

                    // calculate text vertical offset
                    offsetY = (2 == dwVertFlag) ? tSize.cy - newSize.cy     // align to bottom
                        : (3 == dwVertFlag) ? (tSize.cy - newSize.cy) / 2   // align to middle
                        : 0;                                                // align to top
                }

                if (offsetX || offsetY)
                {
                    OffsetRect(&rcText, offsetX, offsetY);
                }
            }

            CC_BREAK_IF(! prepareBitmap(tSize.cx, tSize.cy));

            // draw text
            HGDIOBJ hOldFont = SelectObject(m_hDC, m_hFont);
            HGDIOBJ hOldBmp  = SelectObject(m_hDC, m_hBmp);
            
            SetBkMode(m_hDC, TRANSPARENT);
            SetTextColor(m_hDC, RGB(255, 255, 255)); // white color

            // draw text
            nRet = DrawTextW(m_hDC, pwszBuffer, nLen, &rcText, dwFmt);
            //DrawTextA(m_hDC, pszText, nLen, &rcText, dwFmt);

            SelectObject(m_hDC, hOldBmp);
            SelectObject(m_hDC, hOldFont);
        } while (0);
        CC_SAFE_DELETE_ARRAY(pwszBuffer);
        return nRet;
    }

    CC_SYNTHESIZE_READONLY(HDC, m_hDC, DC);
    CC_SYNTHESIZE_READONLY(HBITMAP, m_hBmp, Bitmap);
private:
    friend class CCImage;
    HFONT   m_hFont;
    HWND    m_hWnd;
    std::string m_curFontPath;
};

static BitmapDC& sharedBitmapDC()
{
    static BitmapDC s_BmpDC;
    return s_BmpDC;
}

static bool s_IsDirectWriteTextRenderingEnabled = false;
bool
CCImage::isDirectWriteTextRenderingEnabled()
{
	return s_IsDirectWriteTextRenderingEnabled;
}

void
CCImage::enableDirectWriteTextRendering(bool iEnable)
{
	s_IsDirectWriteTextRenderingEnabled = iEnable;
}

bool CCImage::initWithString(
                               const char *    pText, 
                               int             nWidth/* = 0*/, 
                               int             nHeight/* = 0*/,
                               ETextAlign      eAlignMask/* = kAlignCenter*/,
                               const char *    pFontName/* = nil*/,
                               int             nSize/* = 0*/)
{
	DirectWriteManager* manager = DirectWriteManager::instance();
	if (manager != NULL)
	{
		return manager->renderText(this, pText, nWidth, nHeight, eAlignMask, pFontName, nSize);
	}

    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! pText);       

        BitmapDC& dc = sharedBitmapDC();

        if (! dc.setFont(pFontName, nSize))
        {
            CCLog("Can't found font(%s), use system default", pFontName);
        }

        // draw text
        SIZE size = {nWidth, nHeight};
        CC_BREAK_IF(! dc.drawText(pText, size, eAlignMask));

        m_pData = new unsigned char[size.cx * size.cy * 4];
        CC_BREAK_IF(! m_pData);

        struct
        {
            BITMAPINFOHEADER bmiHeader;
            int mask[4];
        } bi = {0};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        CC_BREAK_IF(! GetDIBits(dc.getDC(), dc.getBitmap(), 0, 0, 
            NULL, (LPBITMAPINFO)&bi, DIB_RGB_COLORS));

        m_nWidth    = (short)size.cx;
        m_nHeight   = (short)size.cy;
        m_bHasAlpha = true;
        m_bPreMulti = true;
        m_nBitsPerComponent = 8;
        // copy pixed data
        bi.bmiHeader.biHeight = (bi.bmiHeader.biHeight > 0)
           ? - bi.bmiHeader.biHeight : bi.bmiHeader.biHeight;
        GetDIBits(dc.getDC(), dc.getBitmap(), 0, m_nHeight, m_pData, 
            (LPBITMAPINFO)&bi, DIB_RGB_COLORS);

        // change pixel's alpha value to 255, when it's RGB != 0
        COLORREF * pPixel = NULL;
        for (int y = 0; y < m_nHeight; ++y)
        {
            pPixel = (COLORREF *)m_pData + y * m_nWidth;
            for (int x = 0; x < m_nWidth; ++x)
            {
                COLORREF& clr = *pPixel;

				if (clr != 0)
				{
					const uint32_t a = GetRValue(clr);

					// Should be premultiplied alpha
					// ABGR
					clr = (a << 24) | (a << 16) | (a << 8) | a;
				}

                ++pPixel;
            }
        }

        bRet = true;
    } while (0);

    return bRet;
}


void CCImage::calculateStringSize(const char* pText,
	const char *    pFontName,
	int             nFontSize,
	cocos2d::CCSize&         oComputedSize,
	int&            oAdjustedFontSize,
	int            nMinFontSize,
	int             nWidth,
	int             nHeight
	)
{
	assert(pText);
	wchar_t * pwszBuffer = 0;
	int nLen = strlen(pText);
	// utf-8 to utf-16
	int nBufLen = nLen + 1;
	pwszBuffer = new wchar_t[nBufLen];
	if (!pwszBuffer) {
		return;
	}
	memset(pwszBuffer, 0, sizeof(wchar_t) *nBufLen);
	nLen = MultiByteToWideChar(CP_UTF8, 0, pText, nLen, pwszBuffer, nBufLen);

	DirectWriteManager* manager = DirectWriteManager::instance();
	if (manager != NULL)
	{
		manager->calculateStringSize(pwszBuffer, pFontName, nFontSize, oComputedSize, oAdjustedFontSize, nMinFontSize, nWidth, nHeight);
		return;
	}

	BitmapDC& dc = sharedBitmapDC();

	// GG: CCImage::calculateStringSize is mainly used to determine the size
	// of a text to be drawn in a texture.
	// On Windows fonts are not necessarily proportional, then we have to perform
	// the computations in real target coordinates...
	const float scale = CC_CONTENT_SCALE_FACTOR();
	int fontSize = nFontSize * scale;
	const int minFontSize = nMinFontSize * scale;
	const int maxWidth = nWidth * scale;
	const int maxHeight = nHeight * scale;

	while (fontSize >= minFontSize) {
		if (!dc.setFont(pFontName, fontSize)) {
			CCLog("Can't found font(%s), use system default", pFontName);
		}
		const SIZE calcSize = dc.sizeWithText(pwszBuffer, nLen, 0, maxWidth);
		oComputedSize.width = calcSize.cx;
		oComputedSize.height = calcSize.cy;
		if ((maxWidth != 0 && calcSize.cx>maxWidth) || (maxHeight != 0 && calcSize.cy>maxHeight)) {
			fontSize--;
		}
		else {
			break;
		}
	}

	// ... But at the end, we must convert back to original scale the output parameters.
	oComputedSize.width /= scale;
	oComputedSize.height /= scale;

	oAdjustedFontSize = fontSize / scale;
}


NS_CC_END
