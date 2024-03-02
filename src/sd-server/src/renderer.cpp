#include <renderer.hpp>

#include <cstring>

// Radian-Degree Conversion Functions
#define rads(d) (d * M_PI / 180.0f)
#define degs(r) (r * 180.0f / M_PI)

extern const char *_shader[2];

Renderer::Renderer(const std::string &name) : Window(name) {
  // Load Shader

  GLuint vert, frag;

  int success;
  char infoLog[1024];

  // ---- Compile Vertex Shader
  vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &_shader[0], nullptr);
  glCompileShader(vert);

  glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert, 1024, nullptr, infoLog);
    printf("Failed to compile Vertex Shader:\n%s", infoLog);
    return;
  }

  // ---- Compile Fragment Shader
  frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &_shader[1], nullptr);
  glCompileShader(frag);

  glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag, 1024, nullptr, infoLog);
    glDeleteShader(vert);
    printf("Failed to compile Fragment Shader:\n%s", infoLog);
    return;
  }

  // ---- Create Shader Program
  shader = glCreateProgram();
  glAttachShader(shader, vert);
  glAttachShader(shader, frag);

  glLinkProgram(shader);

  glDeleteShader(vert);
  glDeleteShader(frag);

  // Check if linking was successful
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
    printf("Failed to link shaders:\n%s", infoLog);
    return;
  }

  // Load Rect
  float vertices[] = {
      // positions      // texture coords
      -1.0f, 1.0f,  0.0f, 1.0f, // top right
      -1.0f, -1.0f, 0.0f, 0.0f, // bottom right
      1.0f,  -1.0f, 1.0f, 0.0f, // bottom left
      1.0f,  1.0f,  1.0f, 1.0f  // top left
  };

  uint32_t indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  glGenVertexArrays(3, V);

  glBindVertexArray(V[0]);

  glBindBuffer(GL_ARRAY_BUFFER, V[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, V[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Config OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
}

Renderer::~Renderer() {
  // Delete Buffers
  glDeleteBuffers(3, V);

  // Delete Textures
  glDeleteTextures(textures.size(), textures.data());
}

void Renderer::clear(GLuint R, GLuint G, GLuint B) {
  // Clear Screen
  glClearColor(R, G, B, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

size_t Renderer::add(unsigned char *data, int width, int height, int channels) {
  GLuint id;

  // Generate Empty Texture
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  // Select Color Mode
  GLint mode = GL_RGB;
  switch (channels) {
  case 1:
    mode = GL_RED;
    break;

  case 2:
    mode = GL_RG;
    break;

  default:
  case 3:
    mode = GL_RGB;
    break;

  case 4:
    mode = GL_RGBA;
    break;
  }

  // Create Texture
  glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE,
               data);

  // Generate MipMap & Set Parameters
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  textures.push_back(id);
  return textures.size() - 1;
}

void Renderer::draw() {
  // TODO optimize using draw.mesh
  glUseProgram(shader);

  // glUniform

  glBindVertexArray(V[0]);
  glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, nullptr);
}

const char *_shader[] = {"#version 300 es\n"

                         "layout(location = 0) in vec2 aPos;\n"
                         "layout(location = 1) in vec2 aTexCoord;\n"

                         "uniform struct {\n"
                         "vec2 center;\n"
                         "vec2 size;\n"
                         "} v;\n"

                         "out vec2 texcoord;\n"

                         "void main(){\n"
                         "texcoord = aTexCoord;\n"
                         "gl_Position = vec4(aPos.xy, 0.0, 1.0);\n"
                         "}\n",

                         "#version 300 es\n"
                         "precision mediump float;\n"
                         "out vec4 FragColor;\n"

                         "in vec2 texcoord;\n"

                         "void main() {\n"
                         "FragColor = vec4(1.0, 0.4, 0.0, 1.0);\n"
                         "}"};

// // Vertex
// "#version 300 es"

// "layout(location = 0) in vec2 aPos;"
// "layout(location = 1) in vec2 aTexCoord;"

// "out struct {"
// "vec2 TexCoord;"
// "vec2 localPos;"
// "vec2 globalPos;"
// "vec2 center;"
// "vec2 size;"
// "} io;"

// "uniform struct {"
// "vec2 center;"
// "vec2 size;"
// "float rotation;"
// "vec2 ratio;"
// "} v;"

// "void main(){"
// "vec2 point = v.size * aPos;"

// "io.localPos = aPos;"
// "io.globalPos = point + v.center;"

// "vec2 pos = v.center;"
// "pos.x += sin(v.rotation) * (point.x) - cos(v.rotation) * (point.y);"
// "pos.y += cos(v.rotation) * (point.x) + sin(v.rotation) * (point.y);"
// "pos *= v.ratio;"

// "io.center = v.center;"
// "io.size = v.size;"

// "io.TexCoord = aTexCoord;"

// "gl_Position = vec4(pos.xy, 0.0, 1.0);"
// "}",

// // Fragment
// "#version 300 es"
// "precision mediump float;"
// "out vec4 FragColor;"

// "in struct {"
// "vec2 TexCoord;"
// "vec2 localPos;"
// "vec2 globalPos;"
// "vec2 center;"
// "vec2 size;"
// "} io;"

// "uniform struct {"
// "int look;"

// "sampler2D tex;"
// "vec3 color;"

// "float corner;"

// "float time;"
// "} f;"

// "void main() {"
// "FragColor = vec4(1.0, 0.0, 0.0, 0.0);"
// "}"};