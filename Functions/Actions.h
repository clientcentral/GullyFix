//
// THIS TOOL CODED BY TECNO (YIA) ---
//

#pragma once
#include <windows.h>
#include <string>
#include <shellapi.h>

namespace GullyActions {

    inline bool IsServiceInstalled() {
        SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
        if (!scm) return false;
        SC_HANDLE service = OpenServiceW(scm, L"GamingServices", SERVICE_QUERY_STATUS);
        bool exists = (service != nullptr);
        if (service) CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return exists;
    }

    inline void ReinstallGamingServices() {
        if (IsServiceInstalled()) {
            std::wstring cmdRemove = L"-ExecutionPolicy Bypass -Command \"Get-AppxPackage *gamingservices* -AllUsers | Remove-AppxPackage -AllUsers\"";

            //
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.fMask = SEE_MASK_NOCLOSEPROCESS; // idk kys man
            sei.lpVerb = L"runas";               // admin prms
            sei.lpFile = L"powershell.exe";
            sei.lpParameters = cmdRemove.c_str();
            sei.nShow = SW_HIDE;                 // hide cmd fuck

            if (ShellExecuteExW(&sei)) {
                // wating for done process or wait 30 sec MAX
                WaitForSingleObject(sei.hProcess, 30000);
                CloseHandle(sei.hProcess);
            }
        }

        std::wstring storeLink = L"ms-windows-store://pdp/?productid=9MWPM2CQNLHN";
        ShellExecuteW(NULL, L"open", storeLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

}