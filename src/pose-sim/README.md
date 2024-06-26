# Pose Sim

This is a pose simulator so we can make poses without running them on the sensitive[^1] hardware \
It uses (Legacy) OpenGL to render the robot and a custom YML-like file format to store them.

It *could be* compiled to Windows, but we only need it to run on linux, so there are no Windows binaries for it.

## Setup (Linux-only)

1. Install Toolchain & Libraries
```bash
sudo apt install build-essential cmake libglfw3-dev
```

2. Build Project
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

3. Run
```bash
./pose-sim
```

[^1]: it's not that it is sensitive, it is just a long process to get it *safely* working and is pretty easy to make it break itself (not electrically, but physically)