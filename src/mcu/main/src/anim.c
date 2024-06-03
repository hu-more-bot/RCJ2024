#include <anim.h>

#include <pico/time.h>

#if !defined(MIN) || !defined(MAX)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#endif
#define LIMIT(x, min, max) (MAX(MIN(x, max), min))
#define PERCENT(x) (LIMIT(x, 0, 1))

// Pose Config
struct {
  float start;
  size_t state;

  anim_t anim;
  size_t id;

  int isInit;
} _anim = {0, 0, {}, 0, 0};

int anim_init(int startPin, anim_t *anim) {
  if (_anim.isInit || pose_init(startPin))
    return 1;

  // TODO load animation

  return 1;

  _anim.isInit = 1;
}

void anim_set(size_t id) { _anim.id = id; }

void anim_update() {
  float now = to_ms_since_boot(get_absolute_time()) / 1000.0f;
  if (_anim.isInit) {
    if (now > _anim.start + _anim.anim.delay[_anim.state]) {
      _anim.start = now;
      _anim.state++;
    }

    if (_anim.state >= _anim.anim.frames)
      _anim.state = 0;

    pose_set(_anim.anim.pose[_anim.state]);
  }

  pose_update();
}