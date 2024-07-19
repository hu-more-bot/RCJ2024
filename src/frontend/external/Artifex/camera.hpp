/**
 * @brief C++ Bindings for Artifex's Camera Library
 *
 * @date 2024.06.xx
 * @author SamTheDev
 */

#pragma once

#include <functional>

namespace Artifex {

class Camera {
   public:
    struct Frame;

   public:
    Camera(int width, int height, int fps);
    ~Camera();

    int start(std::function<void(const Frame&)> callback);
    void stop();

   public:
    struct Frame {
        int width, height;
        unsigned char* data;
    };

   private:
    struct axCamera_* m_camera{};
};

}  // namespace Artifex