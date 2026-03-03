#pragma once
#include <windows.h>

namespace Tray {
    void Init(HINSTANCE hInstance, int port);
    void Cleanup();
}
