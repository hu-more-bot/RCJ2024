/**
 * @brief C++ Bindings for Artifex's Rendering Library
 *
 * @date 2024.06.xx
 * @author SamTheDev
 */

#pragma once

#include <Artifex/vec.hpp>

namespace Artifex {

class Renderer {
   public:
    struct Mesh;
    struct DrawInfo;

   public:
    explicit Renderer(class Window& window);
    ~Renderer();

    void update();
    void clear(const vec<4>& color);

    unsigned int loadShader(const char* vertex, const char* fragment, const char* geometry = nullptr);

    unsigned int loadTexture(int width, int height, int channels, const unsigned char* data);

    Mesh loadMesh(const std::vector<vec<2>>& vertices, const std::vector<unsigned int>& indices);

    void unloadShader(unsigned int shader);

    void unloadTexture(unsigned int texture);

    void unloadMesh(Mesh mesh);

    void draw(const DrawInfo& drawInfo);

    static int checkErrors();

   public:
    struct DrawInfo {
        vec<2> center, size;
        float rotation;

        vec<4> corner;

        vec<3> color;
        unsigned int texture;

        //        enum {
        //            DYNAMIC = 0,
        //            COLOR,
        //            TEXTURE
        //        } style;

        int style;
    };

   private:
    struct axRenderer_* m_renderer{};
};

}  // namespace Artifex