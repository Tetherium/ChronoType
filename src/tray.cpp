#include "tray.h"
#include <string>
#include <shellapi.h>

namespace Tray {
    NOTIFYICONDATA nid = {};
    HWND hWnd = NULL;
    int currentPort = 8080;

    #define WM_TRAYICON (WM_USER + 1)
    #define ID_TRAY_EXIT 1001
    #define ID_TRAY_OPEN 1002

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_TRAYICON:
                if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP) {
                    POINT pt;
                    GetCursorPos(&pt);
                    HMENU hMenu = CreatePopupMenu();
                    InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, ID_TRAY_OPEN, "Open Dashboard");
                    InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, "Exit ChronoType");
                    SetForegroundWindow(hwnd);
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
                    DestroyMenu(hMenu);

                    if (cmd == ID_TRAY_OPEN) {
                        std::string url = "http://localhost:" + std::to_string(currentPort);
                        ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    } else if (cmd == ID_TRAY_EXIT) {
                        PostQuitMessage(0);
                    }
                }
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    void Init(HINSTANCE hInstance, int port) {
        currentPort = port;
        const char* class_name = "ChronoTypeTrayClass";
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = class_name;
        RegisterClass(&wc);

        hWnd = CreateWindow(class_name, "ChronoType", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);

        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hWnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); // 1 => refers to IDI_ICON1 in resource.rc
        if (!nid.hIcon) {
            // Fallback if logo.ico is missing
            nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        }
        strcpy_s(nid.szTip, sizeof(nid.szTip), "ChronoType is running");
        Shell_NotifyIcon(NIM_ADD, &nid);
    }

    void Cleanup() {
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }
}
