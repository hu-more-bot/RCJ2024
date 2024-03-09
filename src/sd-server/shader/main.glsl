#shader vertex

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

#shader fragment

#version 300 es
precision mediump float;
out vec4 FragColor;

in struct {
    vec2 TexCoord;

    vec2 localPos;
    vec2 globalPos;

    vec2 center;
    vec2 size;
} io;

uniform struct {
    int look;

    sampler2D tex;
    vec3 color;

    float corner;

    float time;
} f;

// from https://iquilezles.org/articles/distfunctions
float roundedBoxSDF(vec2 center, vec2 size, float radius) {
    return length(max(abs(center) - size + radius, 0.0)) - radius;
}

void main() {
    float radius = clamp(f.corner, 0.0, 1.0) * min(io.size.x, io.size.y);

    float distance =
    roundedBoxSDF(io.globalPos - io.center, io.size, radius);

    float smoothedAlpha = 1.0 - smoothstep(0.0, 0.003, distance);

    vec3 color;
    switch (f.look) {
        default :
        case 0:
        // TODO: customizability
        color = 0.5 + 0.5 * cos(f.time + io.globalPos.xyx + vec3(0.0, 2.0, 4.0));
        break;

        case 1:
        // Custom Color
        color = f.color;
        break;

        case 2:
        // Texture
        color = texture(f.tex, io.TexCoord).xyz;
        break;
    }

    FragColor = vec4(color, smoothedAlpha);
}