#pragma once
#include <sal.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
typedef HRESULT(*D3D11CreateDeviceAndSwapChain_t)(
	_In_opt_        IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE      DriverType,
	HMODULE              Software,
	UINT                 Flags,
	_In_opt_  const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT                 FeatureLevels,
	UINT                 SDKVersion,
	_In_opt_  const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	_Out_opt_       IDXGISwapChain** ppSwapChain,
	_Out_opt_       ID3D11Device** ppDevice,
	_Out_opt_       D3D_FEATURE_LEVEL* pFeatureLevel,
	_Out_opt_       ID3D11DeviceContext** ppImmediateContext
	);

extern D3D11CreateDeviceAndSwapChain_t imp_D3D11CreateDeviceAndSwapChain;