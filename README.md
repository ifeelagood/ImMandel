# ImMandel

A featured mandelbrot viewer using imgui

## Build Instructions

Requirements:

1. Visual Studio 2022
2. vcpkg installed as component of Visual Studio 2022
3. cmake installed as component of Visual Studio 2022


## Features

### GUI

- [ ] mouse/keyboard controls
- [ ] switch rendering backends, colouring algorithms
- [ ] profiling

### Rendering Backends
- [ ] CPU backend with fixed precision (float, double)
- [ ] CPU backend with arbitrary precision (mpfr)
- [ ] SIMD backend with fixed precision (float, double)
- [ ] SIMD backend with arbitrary precision [paper](https://www.texmacs.org/joris/fpsimd/fpsimd.html)
- [ ] CUDA backend with fixed precision (float, double)
- [ ] CUDA backend with arbitrary precision (?)
- [ ] multithreading
- [ ] variations for all SSE/AVX/AVX2/AVX512

### Algorithms/Techniques
- [ ] cardoid/bulb checking
- [ ] periodicity checking
- [ ] basic coloring algorithm
- [ ] continuous coloring
- [ ] pertubation theory and series approximation
