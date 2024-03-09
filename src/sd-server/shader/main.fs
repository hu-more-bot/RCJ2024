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