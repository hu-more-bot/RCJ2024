/**
 * @brief C++ Bindings for Artifex's Windowing Library
 *
 * @date 2024.06.xx
 * @author SamTheDev
 */

#pragma once

#include <string>

#include "keys.h"

namespace Artifex {

class Window {
   public:
    Window(const std::string& title, int width, int height);
    ~Window();

    bool update();
    void exit();

    void getSize(int& width, int& height);
    float getRatio();

    void setVSync(int interval);

    int getCursor(float& x, float& y);
    bool getKeyState(axWindowKey key);

    typedef void (*procAddr)();
    static procAddr getProcAddress(const char* procname);

    void makeCurrent();

    // TODO

   private:
    friend class Renderer;

    struct axWindow_* m_window{};
};

}  // namespace Artifex