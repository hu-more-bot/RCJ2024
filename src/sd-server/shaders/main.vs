#version 300 es

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out struct {
    vec2 TexCoord;// Texture Coordinates

    vec2 localPos;// Local Pos (in rect)
    vec2 globalPos;// Global Pos (on screen)

    vec2 center;// Center
    vec2 size;// Size
} io;

uniform struct {
    vec2 center;
    vec2 size;
    float rotation;

    vec2 ratio;
} v;

void main() {
    vec2 point = v.size * aPos;

    io.localPos = aPos;
    io.globalPos = point + v.center;

    vec2 pos = v.center;
    pos.x += sin(v.rotation) * (point.x) - cos(v.rotation) * (point.y);
    pos.y += cos(v.rotation) * (point.x) + sin(v.rotation) * (point.y);
    pos *= v.ratio;

    io.center = v.center;
    io.size = v.size;

    io.TexCoord = aTexCoord;

    gl_Position = vec4(pos.xy, 0.0, 1.0);
}