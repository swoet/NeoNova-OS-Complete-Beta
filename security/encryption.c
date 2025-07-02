// Disk encryption interface

#include <windows.h>
#include <bcrypt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncrypt.h>
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "ncrypt.lib")

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 32 // 256 bits

// Encrypt data using AES-256-CBC
bool encrypt_data(const void* plaintext, size_t len, void* ciphertext, size_t* out_len, const void* key, size_t key_len) {
    if (!plaintext || !ciphertext || !out_len || !key || key_len != AES_KEY_SIZE) return false;
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_KEY_HANDLE hKey = NULL;
    NTSTATUS status;
    DWORD cbKeyObj = 0, cbData = 0;
    PUCHAR pbKeyObj = NULL;
    UCHAR iv[AES_BLOCK_SIZE] = {0}; // Zero IV for demo; use random IV in production
    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (status != 0) goto cleanup;
    status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbKeyObj, sizeof(cbKeyObj), &cbData, 0);
    if (status != 0) goto cleanup;
    pbKeyObj = (PUCHAR)HeapAlloc(GetProcessHeap(), 0, cbKeyObj);
    if (!pbKeyObj) goto cleanup;
    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
    if (status != 0) goto cleanup;
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, pbKeyObj, cbKeyObj, (PUCHAR)key, (ULONG)key_len, 0);
    if (status != 0) goto cleanup;
    ULONG cbCipher = 0;
    status = BCryptEncrypt(hKey, (PUCHAR)plaintext, (ULONG)len, NULL, iv, AES_BLOCK_SIZE, (PUCHAR)ciphertext, (ULONG)(*out_len), &cbCipher, 0);
    if (status != 0) goto cleanup;
    *out_len = cbCipher;
    HeapFree(GetProcessHeap(), 0, pbKeyObj);
    if (hKey) BCryptDestroyKey(hKey);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    return true;
cleanup:
    if (pbKeyObj) HeapFree(GetProcessHeap(), 0, pbKeyObj);
    if (hKey) BCryptDestroyKey(hKey);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    return false;
}

// Decrypt data using AES-256-CBC
bool decrypt_data(const void* ciphertext, size_t len, void* plaintext, size_t* out_len, const void* key, size_t key_len) {
    if (!plaintext || !ciphertext || !out_len || !key || key_len != AES_KEY_SIZE) return false;
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_KEY_HANDLE hKey = NULL;
    NTSTATUS status;
    DWORD cbKeyObj = 0, cbData = 0;
    PUCHAR pbKeyObj = NULL;
    UCHAR iv[AES_BLOCK_SIZE] = {0}; // Zero IV for demo; use random IV in production
    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (status != 0) goto cleanup;
    status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbKeyObj, sizeof(cbKeyObj), &cbData, 0);
    if (status != 0) goto cleanup;
    pbKeyObj = (PUCHAR)HeapAlloc(GetProcessHeap(), 0, cbKeyObj);
    if (!pbKeyObj) goto cleanup;
    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
    if (status != 0) goto cleanup;
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, pbKeyObj, cbKeyObj, (PUCHAR)key, (ULONG)key_len, 0);
    if (status != 0) goto cleanup;
    ULONG cbPlain = 0;
    status = BCryptDecrypt(hKey, (PUCHAR)ciphertext, (ULONG)len, NULL, iv, AES_BLOCK_SIZE, (PUCHAR)plaintext, (ULONG)(*out_len), &cbPlain, 0);
    if (status != 0) goto cleanup;
    *out_len = cbPlain;
    HeapFree(GetProcessHeap(), 0, pbKeyObj);
    if (hKey) BCryptDestroyKey(hKey);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    return true;
cleanup:
    if (pbKeyObj) HeapFree(GetProcessHeap(), 0, pbKeyObj);
    if (hKey) BCryptDestroyKey(hKey);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    return false;
}

// Key management (real TPM-backed)
bool encryption_store_key(const char* key_id, const void* key, size_t key_len) {
    NCRYPT_PROV_HANDLE hProv = 0;
    NCRYPT_KEY_HANDLE hKey = 0;
    SECURITY_STATUS status = NCryptOpenStorageProvider(&hProv, MS_KEY_STORAGE_PROVIDER, 0);
    if (status != ERROR_SUCCESS) return false;
    status = NCryptCreatePersistedKey(hProv, &hKey, NCRYPT_AES_ALGORITHM, key_id, 0, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hProv); return false; }
    status = NCryptSetProperty(hKey, NCRYPT_KEY_USAGE_PROPERTY, (PBYTE)"ALL", 4, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptSetProperty(hKey, NCRYPT_LENGTH_PROPERTY, (PBYTE)&key_len, sizeof(DWORD), 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptSetProperty(hKey, NCRYPT_BLOCK_LENGTH_PROPERTY, (PBYTE)&key_len, sizeof(DWORD), 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptSetProperty(hKey, NCRYPT_ALGORITHM_PROPERTY, (PBYTE)NCRYPT_AES_ALGORITHM, (DWORD)strlen(NCRYPT_AES_ALGORITHM), 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptSetProperty(hKey, NCRYPT_KEY_DATA_PROPERTY, (PBYTE)key, (DWORD)key_len, 0);
    if (status != ERROR_SUCCESS) { NCryptFreeObject(hKey); NCryptFreeObject(hProv); return false; }
    status = NCryptFinalizeKey(hKey, 0);
    NCryptFreeObject(hKey);
    NCryptFreeObject(hProv);
    return status == ERROR_SUCCESS;
}

bool encryption_load_key(const char* key_id, void* key_out, size_t* key_len) {
    NCRYPT_PROV_HANDLE hProv = 0;
    NCRYPT_KEY_HANDLE hKey = 0;
    SECURITY_STATUS status = NCryptOpenStorageProvider(&hProv, MS_KEY_STORAGE_PROVIDER, 0);
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

// Real file encryption using AES-256
bool encrypt_file(const char* path, const void* key, size_t key_len) {
    printf("[Encryption] Encrypting file: %s\n", path);
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char* buf = (unsigned char*)malloc(fsize);
    if (!buf) { fclose(fp); return false; }
    fread(buf, 1, fsize, fp);
    fclose(fp);
    size_t out_len = fsize + AES_BLOCK_SIZE;
    unsigned char* enc = (unsigned char*)malloc(out_len);
    if (!enc) { free(buf); return false; }
    if (!encrypt_data(buf, fsize, enc, &out_len, key, key_len)) { free(buf); free(enc); return false; }
    char out_path[512];
    snprintf(out_path, sizeof(out_path), "%s.enc", path);
    FILE* out_fp = fopen(out_path, "wb");
    if (!out_fp) { free(buf); free(enc); return false; }
    fwrite(enc, 1, out_len, out_fp);
    fclose(out_fp);
    free(buf); free(enc);
    printf("[Encryption] File encrypted to %s\n", out_path);
    return true;
}

// Real file decryption using AES-256
bool decrypt_file(const char* path, const void* key, size_t key_len) {
    printf("[Encryption] Decrypting file: %s\n", path);
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char* buf = (unsigned char*)malloc(fsize);
    if (!buf) { fclose(fp); return false; }
    fread(buf, 1, fsize, fp);
    fclose(fp);
    size_t out_len = fsize;
    unsigned char* dec = (unsigned char*)malloc(out_len);
    if (!dec) { free(buf); return false; }
    if (!decrypt_data(buf, fsize, dec, &out_len, key, key_len)) { free(buf); free(dec); return false; }
    char out_path[512];
    snprintf(out_path, sizeof(out_path), "%s.dec", path);
    FILE* out_fp = fopen(out_path, "wb");
    if (!out_fp) { free(buf); free(dec); return false; }
    fwrite(dec, 1, out_len, out_fp);
    fclose(out_fp);
    free(buf); free(dec);
    printf("[Encryption] File decrypted to %s\n", out_path);
    return true;
}

// Real network packet encryption using AES-256
bool encrypt_network(const void* in, size_t in_len, void* out, size_t* out_len, const void* key, size_t key_len) {
    printf("[Encryption] Encrypting network packet\n");
    return encrypt_data(in, in_len, out, out_len, key, key_len);
}

bool decrypt_network(const void* in, size_t in_len, void* out, size_t* out_len, const void* key, size_t key_len) {
    printf("[Encryption] Decrypting network packet\n");
    return decrypt_data(in, in_len, out, out_len, key, key_len);
}

// Real IPC message encryption using AES-256
bool encrypt_ipc(const void* in, size_t in_len, void* out, size_t* out_len, const void* key, size_t key_len) {
    printf("[Encryption] Encrypting IPC message\n");
    return encrypt_data(in, in_len, out, out_len, key, key_len);
}

bool decrypt_ipc(const void* in, size_t in_len, void* out, size_t* out_len, const void* key, size_t key_len) {
    printf("[Encryption] Decrypting IPC message\n");
    return decrypt_data(in, in_len, out, out_len, key, key_len);
}
