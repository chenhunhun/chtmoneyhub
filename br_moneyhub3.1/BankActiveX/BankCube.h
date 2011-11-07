#pragma once
#include "Resource/Resource.h"
#include "Interface/BankActiveX_h.h"


class ATL_NO_VTABLE CBankCube :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CBankCube, IBankCube, &IID_IBankCube, &LIBID_BankCubeActiveXLib>,
	public CComControl<CBankCube>,
	public IPersistStreamInitImpl<CBankCube>,
	public IPersistPropertyBagImpl<CBankCube>,
	public IObjectSafetyImpl<CBankCube, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public IOleControlImpl<CBankCube>,
	public IOleObjectImpl<CBankCube>,
	public IOleInPlaceActiveObjectImpl<CBankCube>,
	public IViewObjectExImpl<CBankCube>,
	public IOleInPlaceObjectWindowlessImpl<CBankCube>,
	public IConnectionPointContainerImpl<CBankCube>,
	public IPersistStorageImpl<CBankCube>,
	public ISpecifyPropertyPagesImpl<CBankCube>,
	public IQuickActivateImpl<CBankCube>,
	public IDataObjectImpl<CBankCube>,
	public IProvideClassInfo2Impl<&CLSID_BankCube, &DIID__IBankCubeEvents, &LIBID_BankCubeActiveXLib>,
	public IPropertyNotifySinkCP<CBankCube>,
	public CComCoClass<CBankCube, &CLSID_BankCube>
{

public:

	DECLARE_REGISTRY_RESOURCEID(IDR_BANKACTIVEX)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CBankCube)
		COM_INTERFACE_ENTRY(IBankCube)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY(IViewObject2)
		COM_INTERFACE_ENTRY(IViewObject)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceObject)
		COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
		COM_INTERFACE_ENTRY(IPersistStreamInit)
		COM_INTERFACE_ENTRY(IPersistPropertyBag)
		COM_INTERFACE_ENTRY(IObjectSafety)
		COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
		COM_INTERFACE_ENTRY(IQuickActivate)
		COM_INTERFACE_ENTRY(IPersistStorage)
		COM_INTERFACE_ENTRY(IDataObject)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
	END_COM_MAP()

	BEGIN_PROP_MAP(CBankCube)
	END_PROP_MAP()

	BEGIN_CONNECTION_POINT_MAP(CBankCube)
		CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	END_CONNECTION_POINT_MAP()


	HRESULT STDMETHODCALLTYPE SendData(BSTR strURL, BSTR strData);
};
