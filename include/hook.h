#pragma once

#include <string>
#include <functional>

namespace Keylogger {
    // Callback function signature for when a key is pressed.
    // It passes the string representation of the key (e.g., "A", "Ctrl+C", "Space").
    using KeyCallback = std::function<void(const std::string&)>;

    // Starts the global Windows hook. Blocks the current thread, so run it in a separate thread.
    void StartHook(KeyCallback callback);

    // Stops the global hook and unblocks the hook thread.
    void StopHook();
}
