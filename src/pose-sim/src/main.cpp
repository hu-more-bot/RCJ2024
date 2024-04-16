// #include <Artifex/core/window.hpp>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <pose.hpp>

#include <cmath>
#include <ctime>

// using namespace Artifex;

#define rads(degs) ((degs) * M_PI / 180.0)
#define degs(rads) ((rads) * 180.0 / M_PI)

const float armLength[] = {0.3, 0.2};

float time();

void drawSkeleton(pose_t pose);

struct Servo {
  float value;

  // Smoothing Amount (0 = disable)
  float smoothing;

  float prev; // prev value (for smoothing)
} servo[8];

struct {
  float amount = 0.03;
  pose_t value, prev;
} smooth;

int main() {
  srand(time(0));

  // Create Window
  glfwInit();

  GLFWwindow *window = glfwCreateWindow(480, 480, "PoseSim", 0, 0);
  if (!window) {
    fprintf(stderr, "Failed to create window\n");
    return 1;
  }

  glfwSetWindowAspectRatio(window, 1, 1);
  glfwMakeContextCurrent(window);

  // Print App Info
  printf("Pose Simulator for RCJ2024\nby Team HU-More-Bot\n\n");
  printf("Controls:\n");
  printf("- Use the Arrow Keys to rotate the model\n");
  printf("- Pressing Space reloads the current pose\n\n");

  Pose pose = Pose::load("../template.pose");

  glLineWidth(6);
  glPointSize(4);
  glEnable(GL_DEPTH_TEST);
  float past, now = time();

  float start = now;
  size_t state = 0;

  float camera[3]{};

  bool hasKey = false;
  while (!glfwWindowShouldClose(window)) { // window.update()) {
    // Update View
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate Deltatime
    past = now;
    now = time();
    float deltaTime = now - past;

    if (glfwGetKey(window, GLFW_KEY_SPACE))
      hasKey = true;
    else if (hasKey == true) {
      hasKey = false;
      pose = Pose::load("../template.pose");
      state = 0;
    }

    // Calculate Camera
    if (glfwGetKey(window, GLFW_KEY_UP) && camera[0] > -89)
      camera[0] -= 45 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) && camera[0] < 89)
      camera[0] += 45 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT))
      camera[1] += 45 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT))
      camera[1] -= 45 * deltaTime;

    glLoadIdentity();
    glRotatef(camera[0], 1, 0, 0);
    glRotatef(camera[1], 0, 1, 0);

    // if (glfwGetKey(window, GLFW_KEY_Q))
    //   a += 15 * deltaTime;
    // if (glfwGetKey(window, GLFW_KEY_A))
    //   a -= 15 * deltaTime;

    // if (glfwGetKey(window, GLFW_KEY_W))
    //   b += 15 * deltaTime;
    // if (glfwGetKey(window, GLFW_KEY_S))
    //   b -= 15 * deltaTime;

    // if (glfwGetKey(window, GLFW_KEY_E))
    //   c += 15 * deltaTime;
    // if (glfwGetKey(window, GLFW_KEY_D))
    //   c -= 15 * deltaTime;

    // Play Animation
    const auto &anim = pose.anims["idle"];
    if (now > start + anim[state].delay) {
      start = now;
      state++;

      if (state >= anim.size())
        state = 0;

      // printf("State %zu\n", state, anim[state].delay);
    }
    smooth.value = pose.poses[anim[state].pose];

    pose_t p;
    // Update Smooth Values
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 2; j++) {
        const float &v = (j == 0 ? smooth.value.left : smooth.value.right)[i];
        float &prev = (j == 0 ? smooth.prev.left : smooth.prev.right)[i];

        // pwm = low + % * (high-low)
        // float v = s->min + LIMIT(value, 0.0f, 1.0f) * (s->max - s->min);
        float value = v;

        // Set invalid prev
        if (prev == 0)
          prev = value;

        // Smooth out movement
        // pwm = (pwm * s_amount) + (prev * (100% - s_amount))
        value = (value * smooth.amount) + (prev * (1.0 - smooth.amount));
        prev = value;

        // Double-Check Values
        // if (MAX(s->min, s->max) < value || value < MIN(s->min, s->max))
        // continue; // invalid value, something went wrong
        // if (i == 3)
        // printf("%.2f %.2f\n", s->value, value);

        // servo_setMillis(s->pin, value);
        (j == 0 ? p.left : p.right)[i] = value;
      }
    }

    drawSkeleton(p);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // TODO
  return 0;
}

void drawSkeleton(pose_t pose) {
  /* Draw Spine */ {
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    // spine
    glVertex3d(0.0f, -0.5f, 0.0f);
    glVertex3d(0.0f, 0.5f, 0.0f);

    // elbow
    glVertex3d(-0.2f, 0.5f, 0.0f);
    glVertex3d(0.2f, 0.5f, 0.0f);

    // neck
    glVertex3d(0.0f, 0.5f, 0.0f);
    glVertex3d(0.0f, 0.55f, 0.0f);
    glEnd();
  }

  glPolygonMode(GL_FRONT, GL_LINE);
  glPolygonMode(GL_BACK, GL_POINT);

  /* Draw Base, LCD and Face */ {
    glBegin(GL_QUADS);
    glVertex3d(-0.2, -0.5, -0.2);
    glVertex3d(-0.2, -0.5, 0.2);
    glVertex3d(-0.2, -0.7, 0.2);
    glVertex3d(-0.2, -0.7, -0.2);

    glVertex3d(0.2, -0.5, -0.2);
    glVertex3d(-0.2, -0.5, -0.2);
    glVertex3d(-0.2, -0.7, -0.2);
    glVertex3d(0.2, -0.7, -0.2);

    glVertex3d(0.2, -0.5, 0.2);
    glVertex3d(0.2, -0.5, -0.2);
    glVertex3d(0.2, -0.7, -0.2);
    glVertex3d(0.2, -0.7, 0.2);

    glVertex3d(-0.2, -0.5, 0.2);
    glVertex3d(0.2, -0.5, 0.2);
    glVertex3d(0.2, -0.7, 0.2);
    glVertex3d(-0.2, -0.7, 0.2);

    // box top
    glVertex3d(-0.2, -0.5, -0.2);
    glVertex3d(0.2, -0.5, -0.2);
    glVertex3d(0.2, -0.5, 0.2);
    glVertex3d(-0.2, -0.5, 0.2);

    // box bottom
    glVertex3d(0.2, -0.7, -0.2);
    glVertex3d(-0.2, -0.7, -0.2);
    glVertex3d(-0.2, -0.7, 0.2);
    glVertex3d(0.2, -0.7, 0.2);

    // lcd
    glVertex3d(0.1, 0.3, -0.05);
    glVertex3d(-0.1, 0.3, -0.05);
    glVertex3d(-0.1, -0.1, -0.05);
    glVertex3d(0.1, -0.1, -0.05);

    // face
    glVertex3d(0.1, 0.55, 0.0);
    glVertex3d(0.1, 0.65, 0.0);
    glVertex3d(-0.1, 0.65, 0.0);
    glVertex3d(-0.1, 0.55, 0.0);
    glEnd();
  }

  // Draw Hands
  for (int i = 0; i < 2; i++) {
    glBegin(GL_LINE_STRIP);

    float arm[3] = {-0.2f + i * 0.4f, 0.5, 0};

    glColor3f(i == 0, 0.1f, i == 1);
    glVertex3d(arm[0], arm[1], arm[2]);

    // a limits: 0 - 90
    // b limits: -25 - 90
    // c limits: 0 - 150

    // a = 45 + sin(time()) * 45;
    // b = -25 + 45 + sin(time()) * 45;
    // c = 75 + sin(time()) * 75;

    float a = (i == 0 ? pose.right : pose.left)[0] * 90;
    float b = (i == 0 ? pose.right : pose.left)[1] * 90;
    float c = (i == 0 ? pose.right : pose.left)[2] * 150;
    float d = (i == 0 ? pose.right : pose.left)[3] * 90;
    // float d = (sin(time()) + 1) / 2.0 * 180;

    const float A = rads(i == 0 ? a : -a);
    const float B = rads(90 - b);
    const float C = rads(90 - (b + c));
    const float D = rads(i == 0 ? d : -d);

    arm[0] -= armLength[0] * sin(A);
    arm[1] -= armLength[0] * sin(B) * cos(A);
    arm[2] -= armLength[0] * cos(B);

    glColor3f(i == 0, 0.4f, i == 1);
    glVertex3d(arm[0], arm[1], arm[2]);

    arm[0] -= armLength[1] * (sin(A) + sin(D) * cos(C)) / 2.0;
    arm[1] -= armLength[1] * (sin(C) * cos(A));
    arm[2] -= armLength[1] * (cos(C) * cos(D));
    glColor3f(i == 0, 0.8f, i == 1);
    glVertex3d(arm[0], arm[1], arm[2]);

    glEnd();

    /*
    opengl axes

       y
       |
       @___ x
      /
     z

    */
  }

  glFlush();
}

float time() {
  timespec res{};
  clock_gettime(CLOCK_MONOTONIC, &res);
  return static_cast<float>(1000.0f * static_cast<float>(res.tv_sec) +
                            static_cast<float>(res.tv_nsec) / 1e6) /
         1000.0f;
}