#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <wininet.h>
#include <Minhook.h>
#include "skCrypter.h" 


#pragma comment(lib, "wininet.lib")




typedef BOOL(WINAPI* WriteProcessMemory_t)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
WriteProcessMemory_t TrueWriteProcessMemory = nullptr;

std::ofstream logFile;


std::string BufferToAoB(const BYTE* buffer, SIZE_T size) {
    std::ostringstream aob;
    aob << std::hex << std::uppercase << std::setfill('0');
    for (SIZE_T i = 0; i < size; ++i) {
        aob << std::setw(2) << (static_cast<int>(buffer[i]) & 0xFF) << " ";
    }
    return aob.str();
}


std::string FormatAddress(uintptr_t address) {
    std::ostringstream oss;
    oss << "0x" << std::hex << address;
    return oss.str();
}


void LogMemoryChange(LPVOID address, const std::vector<BYTE>& originalBytes, const std::vector<BYTE>& newBytes) {
    // Directly set the log file path to the user's Desktop
    std::string logFilePath = "C:\\Users\\%USERNAME%\\Desktop\\AoB_Logs.txt";  // Path to Desktop folder

    // Create the file path dynamically with the current username
    char username[MAX_PATH];
    DWORD size = MAX_PATH;
    if (GetEnvironmentVariableA("USERNAME", username, size)) {
        logFilePath.replace(logFilePath.find("%USERNAME%"), 9, username);  // Replace %USERNAME% with the actual username
    }

    std::ofstream logFile(logFilePath, std::ios::app);  // Open the file in append mode

    // Check if the file is opened successfully
    if (!logFile.is_open()) {
        MessageBox(NULL, "Failed to open log file on Desktop! Ensure the application has proper permissions.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Write the log content
    logFile << skCrypt("Welcome to Panel cracker made by Lucifer! | discord: https://discord.gg/celestialmods") << std::endl;
    logFile << skCrypt("Memory Change Detected:") << std::endl;
    logFile << skCrypt("Address: ") << FormatAddress(reinterpret_cast<uintptr_t>(address)) << std::endl;
    logFile << skCrypt("Original Bytes: ") << BufferToAoB(originalBytes.data(), originalBytes.size()) << std::endl;
    logFile << skCrypt("New Bytes: ") << BufferToAoB(newBytes.data(), newBytes.size()) << std::endl << std::endl;

    logFile.close();  // Close the file
}



BOOL WINAPI HookedWriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
    
    SIZE_T fullRangeSize = 50; 

    std::vector<BYTE> fullOriginalBytes(fullRangeSize); 
    ReadProcessMemory(hProcess, (LPBYTE)lpBaseAddress, fullOriginalBytes.data(), fullRangeSize, NULL); 

    
    BOOL result = TrueWriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);

    if (result) {
       
        std::vector<BYTE> newBytes(fullOriginalBytes); 
        memcpy(newBytes.data(), lpBuffer, nSize);      

        LogMemoryChange(lpBaseAddress, fullOriginalBytes, newBytes); 
    }
    return result;
}




std::string FetchPastebinContent(const std::string& url) {
    HINTERNET hInternet = InternetOpenA("DLLHook", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";

    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[1024];
    DWORD bytesRead;
    std::string content;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead) {
        content.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return content;
}

std::string GenerateURL() {

    std::string part1 = "https://";
    std::string part2 = "pastebin.com/";
    std::string part3 = "raw/";
    std::string part4 = "86D9Li9Z";
    return part1 + part2 + part3 + part4;
}

bool IsHookEnabled() {
    
    std::string decryptedURL = GenerateURL();
    std::string pastebinContent = FetchPastebinContent(decryptedURL);
    return pastebinContent.find("ON") != std::string::npos;
   
}
void ErasePEHeader(HMODULE hModule) {
    DWORD oldProtect;
    VirtualProtect(hModule, 4096, PAGE_READWRITE, &oldProtect);
    SecureZeroMemory(hModule, 4096);
    VirtualProtect(hModule, 4096, oldProtect, &oldProtect);
}

//  FIX: Define the missing function BypassDLLDetection

void BypassDLLDetection() {
    ErasePEHeader(GetModuleHandle(NULL));
    logFile << "[?? DLL] Bypassed DLL Detection" << std::endl;
}


// Install the hook using MinHook
void InstallHook() {
    
    if (IsDebuggerPresent()) {
        MessageBox(NULL, skCrypt("Debugger detected!"), skCrypt("Error"), MB_OK | MB_ICONERROR);
        ExitProcess(1);
    }
    if (!IsHookEnabled()) {
        MessageBox(NULL, skCrypt("Subscription Paused!"), skCrypt("Info"), MB_OK);
        ExitProcess(1);
    }

    if (MH_Initialize() != MH_OK) {
        MessageBox(NULL, "Initialization failed", "Error", MB_ICONERROR);
        return;
    }

    if (MH_CreateHookApi(L"kernel32", skCrypt("WriteProcessMemory"), &HookedWriteProcessMemory, reinterpret_cast<LPVOID*>(&TrueWriteProcessMemory)) != MH_OK) {
        MessageBox(NULL, "Failed to create hook", "Error", MB_ICONERROR);
        return;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        MessageBox(NULL, "Failed to enable hook", "Error", MB_ICONERROR);
        return;
    }
    BypassDLLDetection();
}


// Uninstall the hook and clean up
void UninstallHook() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

DWORD WINAPI InstallHookThread(LPVOID lpParam) {
    InstallHook();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, InstallHookThread, NULL, 0, NULL);  // Create a new thread for hook installation
        break;
    case DLL_PROCESS_DETACH:
        UninstallHook();
        break;
    }
    return TRUE;
}
