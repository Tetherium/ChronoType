#include "hook.h"
#include "db.h"
#include "server.h"
#include "tray.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <windows.h>

void EnableAutoStart() {
    HKEY hKey;
    const char* czStartName = "ChronoType";
    char szPathToExe[MAX_PATH];
    GetModuleFileName(NULL, szPathToExe, MAX_PATH);
    RegCreateKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
    RegSetValueEx(hKey, czStartName, 0, REG_SZ, (const BYTE*)szPathToExe, sizeof(szPathToExe));
    RegCloseKey(hKey);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Enable Auto-Start
    EnableAutoStart();

    // 2. Initialize DB
    if (!Database::Initialize("database.db")) {
        return 1;
    }

    // 3. Start Keyboard Hook in a separate thread
    std::thread hookThread([]() {
        std::string currentWord = "";

        Keylogger::StartHook([&currentWord](const std::string& key) {
            Database::LogKeyPress(key);

            if (key == "Space" || key == "Enter") {
                if (!currentWord.empty()) {
                    Database::LogWord(currentWord);
                    currentWord.clear();
                }
            } else if (key == "Backspace") {
                if (!currentWord.empty()) {
                    currentWord.pop_back();
                }
            } else if (key.find("Ctrl") == std::string::npos && 
                       key.find("Alt") == std::string::npos && 
                       key.find("Shift") == std::string::npos && 
                       key.find("Esc") == std::string::npos &&
                       key.find("Tab") == std::string::npos &&
                       key.find("CapsLock") == std::string::npos &&
                       key.find("Up") == std::string::npos &&
                       key.find("Down") == std::string::npos &&
                       key.find("Left") == std::string::npos &&
                       key.find("Right") == std::string::npos) {
                
                currentWord += key;
            }
        });
    });

    // 4. Start Web Server in a separate thread
    DeleteFile("current_port.txt"); // Prevent reading a ghost port from previous crashes
    std::thread serverThread([]() {
        Server::Start(8080);
    });

    // 5. Retrive bound port from the background thread dynamically
    int port = 8080;
    for (int i=0; i<50; i++) { 
        std::ifstream pf("current_port.txt");
        if(pf.is_open()) {
            pf >> port;
            pf.close();
            break;
        }
        Sleep(100);
    }

    // 6. Init System Tray
    Tray::Init(hInstance, port);

    // 7. Windows Message Loop (keeps main thread alive implicitly for WinMain without a console)
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 8. Cleanup upon Exit request from System Tray
    Tray::Cleanup();
    Keylogger::StopHook();
    Server::Stop();

    if (hookThread.joinable()) hookThread.join();
    if (serverThread.joinable()) serverThread.join();

    DeleteFile("current_port.txt");

    return 0;
}
