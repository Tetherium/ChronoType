#pragma once

#include <string>

namespace Server {
    // Starts the local HTTP API to serve requests from the HTML UI.
    // Blocks the thread.
    void Start(int port);

    // Stops the HTTP Server.
    void Stop();
}
