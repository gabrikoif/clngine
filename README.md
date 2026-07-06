# CLngine

> **Status: In Active Development**  
> *CLngine* is short for **Collision Engine**, a high-performance framework designed to handle real-time physical environments with thousands of interacting objects.

---

## The Core Concept

The goal of this project is to create a highly optimized environment where large numbers of objects interact seamlessly. To achieve this without dropping frames, the architecture is split into two distinct responsibilities:

1. **The Graphics Pipeline:** Uses hardware-accelerated instanced rendering to draw thousands of independent models with unique properties (like position and color) in a single draw call.
2. **The Physics Layer:** Processes real-time bounding volume checks and dynamic impact resolutions on the CPU before streaming the results to the GPU.

---

## Current Architecture

### 1. Instanced Rendering Pipeline

* **Batch Processing:** Uses `glDrawElementsInstanced` to offload drawing operations to the GPU, avoiding CPU overhead.
* **Dynamic GPU Data Streaming:** Implements a dual-buffer setup (`instanceVBO` for transforms and `colorsVBO` for appearance data) utilizing `glBufferSubData` to stream per-frame updates.
* **Frame Synchronization:** Locked via hardware V-Sync to prevent screen tearing and stabilize the render loop.

### 2. World & Motion Layer

* **Decoupled Loop Architecture:** Separates the world update step from the rendering passes.
* **Kinematics & Bounding Volumes:** Tracks object attributes (positions, velocities, and dimensions) to calculate continuous motion.
* **Collision Detection & Resolution:** Computes intersection checks and resolves physical impacts to update object trajectories realistically.

---

## Interactive Elements

* Interactive controls, configurable hotkeys, and benchmarking setups will be introduced as development progresses to ensure a smooth, user-friendly runtime experience.

Here is the new section to add directly to the bottom of your README:

---

## Requirements

To build and run the system locally, ensure your environment meets the following specifications:

* **Build Toolchain:** CMake `3.15+` (for project configuration and cross-platform compilation)
* **Compiler:** `GCC` / `G++ 9+` or any modern C++ compiler supporting standard `C++17` primitives
* **Graphics Driver:** An environment with hardware support for **OpenGL 3.3+ Core Profile**

## Test it yourself

To get the project up and running locally, follow these simple steps:

1. **Clone the repository:**
```bash
git clone https://github.com/gabrikoif/clngine.git

```


2. **Open in your IDE:**
Open the cloned folder in any IDE that supports **CMake**.
* *Recommendation:* I highly recommend you use **Visual Studio Code (VS Code)** with the *CMake Tools* extension for the smoothest setup.


3. **Run the project:**
Once your IDE finishes configuring the CMake project, simply build and run it directly through your environment.

---
`C++17` `OpenGL 3.3+` `GLFW` `GLM`

---
*// Render fast. Simulate faster.*
