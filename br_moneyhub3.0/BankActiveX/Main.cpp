#include "stdafx.h"
#include <initguid.h>
#include "BankCube.h"
#include "Interface/BankActiveX_i.c"


class CTuoModule : public CComModule
{
	HRESULT AddCommonRGSReplacements(IRegistrarBase *pRegistrar) throw()
	{
		TCHAR szModule[MAX_PATH];
		::GetModuleFileName(_Module.GetModuleInstance(), szModule, _countof(szModule));

		TCHAR szGUID[256];
		::StringFromGUID2(__uuidof(BankCube), szGUID, _countof(szGUID));
		pRegistrar->AddReplacement(L"CLSID_CLASS", szGUID);
		::StringFromGUID2(LIBID_BankCubeActiveXLib, szGUID, _countof(szGUID));
		pRegistrar->AddReplacement(L"TYPELIB_GUID", szGUID);
		pRegistrar->AddReplacement(L"MODULE", szModule);
		pRegistrar->AddReplacement(L"APPID", GetAppId());
		return S_OK;
	}
} _Module;


BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_BankCube, CBankCube)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hInstance, &LIBID_BankCubeActiveXLib);
		::DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
		_Module.Term();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

STDAPI DllCanUnloadNow()
{
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer()
{
	return _Module.RegisterServer(TRUE);
}

STDAPI DllUnregisterServer()
{
	return _Module.UnregisterServer(TRUE);
}
