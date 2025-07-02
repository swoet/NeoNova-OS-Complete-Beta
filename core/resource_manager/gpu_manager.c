#include "gpu_manager.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#endif

// Query real GPU usage using DXGI and D3D11 (Windows 10+)
float gpu_manager_get_usage(void) {
#ifdef _WIN32
    IDXGIFactory1* pFactory = NULL;
    if (FAILED(CreateDXGIFactory1(&IID_IDXGIFactory1, (void**)&pFactory))) {
        printf("[GPUManager] Failed to create DXGIFactory1.\n");
        return 0.0f;
    }
    IDXGIAdapter1* pAdapter = NULL;
    float usage = 0.0f;
    for (UINT i = 0; pFactory->lpVtbl->EnumAdapters1(pFactory, i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 desc;
        pAdapter->lpVtbl->GetDesc1(pAdapter, &desc);
        // Query D3D11 device for usage
        ID3D11Device* pDevice = NULL;
        ID3D11DeviceContext* pContext = NULL;
        D3D_FEATURE_LEVEL fl;
        if (D3D11CreateDevice((IDXGIAdapter*)pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pDevice, &fl, &pContext) == S_OK) {
            // Query for DXGI 1.4+ (Win10+) for real usage
            IDXGIDevice3* pDXGIDevice3 = NULL;
            if (SUCCEEDED(pDevice->lpVtbl->QueryInterface(pDevice, &IID_IDXGIDevice3, (void**)&pDXGIDevice3))) {
                DXGI_QUERY_VIDEO_MEMORY_INFO memInfo = {0};
                if (SUCCEEDED(pDXGIDevice3->lpVtbl->QueryVideoMemoryInfo(pDXGIDevice3, 0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo))) {
                    if (memInfo.Budget > 0)
                        usage = (float)memInfo.CurrentUsage / (float)memInfo.Budget;
                }
                pDXGIDevice3->lpVtbl->Release(pDXGIDevice3);
            }
            pContext->lpVtbl->Release(pContext);
            pDevice->lpVtbl->Release(pDevice);
        }
        pAdapter->lpVtbl->Release(pAdapter);
        if (usage > 0.0f) break;
    }
    pFactory->lpVtbl->Release(pFactory);
    printf("[GPUManager] Real GPU usage: %.2f\n", usage);
    return usage;
#else
    printf("[GPUManager] GPU usage not implemented on this platform.\n");
    return 0.0f;
#endif
}

// Example scaling: throttle GPU-intensive tasks if usage > 90%
void gpu_manager_scale(void) {
    float usage = gpu_manager_get_usage();
    if (usage > 0.9f) {
        // Integrate with process/task manager to deprioritize GPU tasks
        // Example: Lower priority of GPU-intensive processes (requires process manager integration)
    }
}

// Example prioritization: prioritize critical GPU tasks
void gpu_manager_prioritize(void) {
    // Integrate with scheduler to boost priority of critical GPU tasks
    // Example: Raise priority of critical GPU tasks (requires scheduler integration)
}

// Example power adjustment: reduce GPU power on battery
void gpu_manager_power_adjust(void) {
#ifdef _WIN32
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && ps.ACLineStatus == 0) {
        // Use vendor APIs to reduce GPU clocks/power (NVIDIA, AMD, Intel)
        // Example: Call NVAPI, ADL, or Intel Power Gadget if available
    }
#endif
}

void gpu_manager_update(void) {
    float u = gpu_manager_get_usage();
    printf("[GPUManager] GPU usage: %.2f\n", u);
} 