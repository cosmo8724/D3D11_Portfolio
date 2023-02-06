#pragma once

#define BEGIN(NAMESPACE)  namespace NAMESPACE {
#define END				  }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport) 
#else
#define ENGINE_DLL _declspec(dllimport) 
#endif

#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define FLOAT4COLOR_RGBA(r, g, b, a) \
	_float4(r / 255.f, g / 255.f, b / 255.f, a / 255.f)

#define NULL_CHECK( _ptr)	\
	{if( _ptr == 0){__debugbreak();return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__debugbreak();return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( _ptr == 0){MessageBox(nullptr, _message, L"System Message",MB_OK);__debugbreak();}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( _ptr == 0){MessageBox(nullptr, _message, L"System Message",MB_OK);__debugbreak();return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __debugbreak();return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __debugbreak();return _return;}

#define NO_COPY(CLASSNAME)										\
		private:															\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)											\
		private:															\
		static CLASSNAME*	m_pInstance;								\
		public:																\
		static CLASSNAME*	GetInstance( void );						\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)						\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {								\
				m_pInstance = new CLASSNAME;						\
			}																\
			return m_pInstance;											\
		}																	\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;								\
			if(nullptr != m_pInstance)	{								\
				dwRefCnt = m_pInstance->Release();					\
				if (0 == dwRefCnt)										\
					m_pInstance = nullptr;								\
			}																\
			return dwRefCnt;											\
		}



