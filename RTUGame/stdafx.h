#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <d3d11_3.h>
#include <d3d12.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <string>
#include <codecvt>
#include <wrl.h>
#include <shellapi.h>
#include <memory>
#include <mutex>
#include <future>
#include <queue>
#include <map>
#include <set>

#include "src/Graphics/DirectX12/d3dx12.h"
#include "src/Common/RTUCommon.h"
#include "src/Compression/lz4.h"
#include "src/Compression/lz4hc.h"