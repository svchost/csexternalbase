#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>


using namespace std;

HANDLE handle;
DWORD64 clientBaseAddress;
DWORD64 engineBaseAddress;

DWORD GetProcessIdFromName(const char* processName)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    std::wstring processNameW(processName, processName + strlen(processName));
    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            std::wstring pe32ExeFile(pe32.szExeFile);
            if (pe32ExeFile == processNameW)
            {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}
template <class T> T ReadMemory(HANDLE hProcess, DWORD64 baseAddress, DWORD offset)
{
    DWORD64 address = baseAddress + offset;
    T buffer;
    ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
    return buffer;
}
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}
HANDLE GetProcessHandle(DWORD processID)
{
    HANDLE processHandle;

    // Open a handle to the process with all access rights
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

    return processHandle;
}
/*
* Read from memory at a given address
* @param addr The address to read from
*
* @return The value at the given address
*/ template<class T> T read(uint32_t addr)
{
    T buffer;
    ReadProcessMemory(handle, (LPVOID)addr, &buffer, sizeof(T), 0);
    return buffer;
}
/*
* Write to memory at a given address with a given value
* @param addr The address to write to
* @param value The value to write
*
* @return The value that was written
*/ template<class T> T write(uint32_t addr, T value)
{
    WriteProcessMemory(handle, (LPVOID)addr, &value, sizeof(T), 0);
    return value;
}


//addhaxcodeorsomethingelsehere



int main()
{
    std::string processName = "csgo.exe";
    DWORD pid = GetProcessIdFromName(processName.c_str());
    if (pid == 0)
    {
        std::cout << "Error:  '" << processName << "' not found." << std::endl;
        return 1;
    }

    std::cout << "Process ID: " << pid << std::endl;

    handle = GetProcessHandle(pid);

    clientBaseAddress = GetModuleBaseAddress(pid, L"client.dll");
    engineBaseAddress = GetModuleBaseAddress(pid, L"engine.dll");
    cout << "clientBaseAddress: " << clientBaseAddress << endl;
    cout << "engineBaseAddress: " << engineBaseAddress << endl;

    while (true)
    {
        //add keybindshere

    }

    CloseHandle(handle);
}
