#include "thermal.h"
#include <windows.h>
#include <stdio.h>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

void thermal_init(void) {
    printf("[Thermal] Initialized.\n");
}

void thermal_check(void) {
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) { printf("[Thermal] COM init failed\n"); return; }
    hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hres)) { CoUninitialize(); printf("[Thermal] COM security failed\n"); return; }
    IWbemLocator *pLoc = NULL;
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
    if (FAILED(hres)) { CoUninitialize(); printf("[Thermal] WbemLocator failed\n"); return; }
    IWbemServices *pSvc = NULL;
    hres = pLoc->ConnectServer(L"ROOT\\WMI", NULL, NULL, 0, NULL, 0, 0, &pSvc);
    if (FAILED(hres)) { pLoc->Release(); CoUninitialize(); printf("[Thermal] ConnectServer failed\n"); return; }
    hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hres)) { pSvc->Release(); pLoc->Release(); CoUninitialize(); printf("[Thermal] ProxyBlanket failed\n"); return; }
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(L"WQL", L"SELECT CurrentTemperature FROM MSAcpi_ThermalZoneTemperature", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    if (SUCCEEDED(hres)) {
        IWbemClassObject *pObj = NULL;
        ULONG uReturn = 0;
        while (pEnumerator && pEnumerator->Next(WBEM_INFINITE, 1, &pObj, &uReturn) == S_OK) {
            VARIANT vtProp;
            VariantInit(&vtProp);
            if (SUCCEEDED(pObj->Get(L"CurrentTemperature", 0, &vtProp, 0, 0))) {
                double tempC = ((double)vtProp.uintVal / 10.0) - 273.15;
                printf("[Thermal] CPU Temp: %.1f C\n", tempC);
            }
            VariantClear(&vtProp);
            pObj->Release();
        }
        pEnumerator->Release();
    } else {
        printf("[Thermal] WMI query failed or not supported\n");
    }
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
} 