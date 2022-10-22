![Box2D Logo](https://box2d.org/images/logo.svg)

# Build Status
[![Build Status](https://github.com/Bambo-Borris/box2d/actions/workflows/build.yml/badge.svg)](https://github.com/Bambo-Borris/box2d/actions)

# Box2D
Box2D is a 2D physics engine for games.

## Features

### Collision
- Continuous collision detection
- Contact callbacks: begin, end, pre-solve, post-solve
- Convex polygons and circles
- Multiple shapes per body
- One-shot contact manifolds
- Dynamic tree broadphase
- Efficient pair management
- Fast broadphase AABB queries
- Collision groups and categories

### Physics
- Continuous physics with time of impact solver
- Persistent body-joint-contact graph
- Island solution and sleep management
- Contact, friction, and restitution
- Stable stacking with a linear-time solver
- Revolute, prismatic, distance, pulley, gear, mouse joint, and other joint types
- Joint limits, motors, and friction
- Momentum decoupled position correction
- Fairly accurate reaction forces/impulses

### System
- Small block and stack allocators
- Centralized tuning parameters
- Highly portable C++ with no use of STL containers

### Testbed
- OpenGL with GLFW
- Graphical user interface with imgui
- Extensible test framework
- Support for loading world dumps

## Building and Installing
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target install
```

## Documentation
- [Manual](https://box2d.org/documentation/)
- [reddit](https://www.reddit.com/r/box2d/)
- [Discord](https://discord.gg/NKYgCBP)

## License
Box2D is developed by Erin Catto, and uses the [MIT license](https://en.wikipedia.org/wiki/MIT_License).
