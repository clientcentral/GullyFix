//
// THIS TOOL CODED BY TECNO (YIA) ---
//
#include <windows.h>
#include <wrl.h>
#include <WebView2.h>
#include <dwmapi.h>
#include <WebView2.h>
#include <string>
#include <shlobj.h>
#include "Functions/Actions.h"

#pragma comment(lib, "dwmapi.lib")

using namespace Microsoft::WRL;

#define IDI_APP_ICON 101
#define IDR_HTML_UI  101

HWND g_hwnd;
ComPtr<ICoreWebView2Controller> g_controller;

std::wstring GetResourceString(int resourceID) {
    HRSRC hRes = FindResourceW(GetModuleHandle(NULL), MAKEINTRESOURCEW(resourceID), (LPCWSTR)RT_RCDATA);
    if (!hRes) return L"";
    HGLOBAL hData = LoadResource(NULL, hRes);
    DWORD size = SizeofResource(NULL, hRes);
    const char* data = (const char*)LockResource(hData);

    int wsize = MultiByteToWideChar(CP_UTF8, 0, data, size, NULL, 0);
    std::wstring content(wsize, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, data, size, &content[0], wsize);
    return content;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {


        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProc(hWnd, message, wParam, lParam);
            if (hit == HTCLIENT) {
                return HTCAPTION;
            }
            return hit;
        }


        case WM_SIZE: {
            if (g_controller != nullptr) {
                RECT bounds;
                GetClientRect(hWnd, &bounds);
                g_controller->put_Bounds(bounds);
            }
            break;
        }

        case WM_DESTROY: {
            if (g_controller != nullptr) {
                g_controller->Close();
                g_controller = nullptr;
            }
            PostQuitMessage(0);
            break;
        }


        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 1. icon and class shit
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GullyLauncherClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
    RegisterClassW(&wc);

    //windows things
    DWORD style = WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX;

    int windowWidth = 785;
    int windowHeight = 463;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;

    g_hwnd = CreateWindowW(
        wc.lpszClassName,
        L"Gully Launcher",
        style,
        xPos, yPos,
        windowWidth, windowHeight,
        nullptr, nullptr, hInstance, nullptr
    );


    if (g_hwnd) {

        int corner = 2;
        DwmSetWindowAttribute(g_hwnd, 33, &corner, sizeof(corner));

        BOOL darkSetting = TRUE;
        DwmSetWindowAttribute(g_hwnd, 20, &darkSetting, sizeof(darkSetting));
    }

   ShowWindow(g_hwnd, SW_SHOW);
   UpdateWindow(g_hwnd);

   //tmep folder -cuz this will make folder named GullyLauncher.exe.WebView2-
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring userDataFolder = std::wstring(tempPath) + L"GullyLauncher_Cache";

    // 3.kys
    CreateCoreWebView2EnvironmentWithOptions(nullptr, userDataFolder.c_str(), nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                env->CreateCoreWebView2Controller(g_hwnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            if (controller != nullptr) {
                                g_controller = controller;
                                ComPtr<ICoreWebView2> webview;
                                g_controller->get_CoreWebView2(&webview);


                                RECT bounds;
                                GetClientRect(g_hwnd, &bounds);
                                g_controller->put_Bounds(bounds);
                                g_controller->put_IsVisible(TRUE);


                                ComPtr<ICoreWebView2Settings> settings;
                                if (SUCCEEDED(webview->get_Settings(&settings))) {
                                    settings->put_IsWebMessageEnabled(TRUE);
                                    settings->put_AreDevToolsEnabled(TRUE);
                                }


                                std::wstring html = GetResourceString(101);
                                std::wstring css  = GetResourceString(102);
                                std::wstring js   = GetResourceString(103);

                                std::wstring finalUI = html;
                                size_t headPos = finalUI.find(L"</head>");
                                if (headPos != std::wstring::npos) {
                                    finalUI.insert(headPos, L"<style>" + css + L"</style>");
                                }
                                size_t bodyPos = finalUI.find(L"</body>");
                                if (bodyPos != std::wstring::npos) {
                                    finalUI.insert(bodyPos, L"<script>" + js + L"</script>");
                                }


                                webview->NavigateToString(finalUI.c_str());
                                webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                                    [](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                                        LPWSTR m;
                                        if (SUCCEEDED(args->TryGetWebMessageAsString(&m))) {
                                            std::wstring message(m);

                                            if (message.length() >= 2 && message.front() == L'\"')
                                                message = message.substr(1, message.length() - 2);

                                            if (message == L"start_drag") {

                                                ReleaseCapture();
                                                SendMessage(g_hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                                            }
                                            else if (message == L"window_close") {

                                                DestroyWindow(g_hwnd);
                                            }
                                            else if (message == L"window_minimize") {
                                                ShowWindow(g_hwnd, SW_MINIMIZE);
                                            }
                                            else if (message == L"start_silent_fix") {
                                                GullyActions::ReinstallGamingServices();
                                                sender->PostWebMessageAsString(L"2");
                                            }


                                            CoTaskMemFree(m);
                                        }
                                        return S_OK;
                                }).Get(), nullptr);

                                SetWindowPos(g_hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                            }

                            return S_OK;

                }).Get());

                return S_OK;

    }).Get());

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}