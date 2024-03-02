#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <cstddef>
#include <cstdint>

#include <vector>
#include <unordered_map>

class AL
{
public:
    AL();
    ~AL();

    size_t add(std::vector<int16_t> &data, int freq);
    void remove(size_t id);

    void play(size_t id, bool wait = true);

private:
    ALCdevice *device{};
    ALCcontext *context{};

    // TODO: fix id system
    std::vector<ALuint> buffers;
};