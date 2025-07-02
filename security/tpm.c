// TPM 2.0 integration

#include <windows.h>
#include <bcrypt.h>
#include <tbs.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <ncrypt.h>
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "tbs.lib")
#pragma comment(lib, "ncrypt.lib")

// TPM key storage and attestation using Windows TBS and CNG
// In production, use TBS APIs for real TPM-backed key storage

// Store key in TPM (real, using NCrypt)
bool tpm_store_key(const char* key_id, const void* key, size_t key_len) {
    NCRYPT_PROV_HANDLE hProv = 0;
    NCRYPT_KEY_HANDLE hKey = 0;
    SECURITY_STATUS status = NCryptOpenStorageProvider(&hProv, MS_PLATFORM_CRYPTO_PROVIDER, 0);
    if (status != ERROR_SUCCESS) return false;
    status = NCryptCreatePersistedKey(hProv, &hKey, NCRYPT_RSA_ALGORITHM, key_id, 0, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hProv); return false; }
    status = NCryptSetProperty(hKey, NCRYPT_KEY_DATA_PROPERTY, (PBYTE)key, (DWORD)key_len, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptFinalizeKey(hKey, 0);
    NCryptFreeObject(hKey);
    NCryptFreeObject(hProv);
    return status == ERROR_SUCCESS;
}

// Load key from TPM (real, using NCrypt)
bool tpm_load_key(const char* key_id, void* key_out, size_t* key_len) {
    NCRYPT_PROV_HANDLE hProv = 0;
    NCRYPT_KEY_HANDLE hKey = 0;
    SECURITY_STATUS status = NCryptOpenStorageProvider(&hProv, MS_PLATFORM_CRYPTO_PROVIDER, 0);
    if (status != ERROR_SUCCESS) return false;
    status = NCryptOpenKey(hProv, &hKey, key_id, 0, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hProv); return false; }
    DWORD len = 0;
    status = NCryptGetProperty(hKey, NCRYPT_KEY_DATA_PROPERTY, NULL, 0, &len, 0);
    if (status != ERROR_SUCCESS || len > *key_len) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptGetProperty(hKey, NCRYPT_KEY_DATA_PROPERTY, (PBYTE)key_out, len, &len, 0);
    if (status == ERROR_SUCCESS) *key_len = len;
    NCryptFreeObject(hKey);
    NCryptFreeObject(hProv);
    return status == ERROR_SUCCESS;
}

// TPM attestation (real, using NCryptSignHash)
bool tpm_attest(const void* data, size_t len, void* signature_out, size_t* sig_len) {
    NCRYPT_PROV_HANDLE hProv = 0;
    NCRYPT_KEY_HANDLE hKey = 0;
    SECURITY_STATUS status = NCryptOpenStorageProvider(&hProv, MS_PLATFORM_CRYPTO_PROVIDER, 0);
    if (status != ERROR_SUCCESS) return false;
    status = NCryptOpenKey(hProv, &hKey, L"TPMKey", 0, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hProv); return false; }
    DWORD cbResult = 0;
    status = NCryptSignHash(hKey, NULL, (PBYTE)data, (DWORD)len, (PBYTE)signature_out, (DWORD)*sig_len, &cbResult, 0);
    if (status == ERROR_SUCCESS) *sig_len = cbResult;
    NCryptFreeObject(hKey);
    NCryptFreeObject(hProv);
    return status == ERROR_SUCCESS;
}
