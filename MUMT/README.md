# MUMT 0.0.5 C++ Library

MUMT 0.0.5 is an open framework for Mutli User Multi Touch interfaces,
powered by OpenCV

## Current Implementation

### Content

- encapsulares a [Finger](\ref Finger "Finger Class")
- includes a [Quad Tree](\ref QuadTree "QuadTree Class") for storing [Finger data](\ref Finger "Finger Class")
- includes a Tracker class for tracking the movement of [fingers](\ref Finger "Finger Class") between frames

### Dependencies

- OpenCV 4
- [TUIO 1.1](../vendor/TUIO/README.md)

## Alpha API

Currently this library is very much in development.

## Examples

## Integration and Testing

The provided CMakeLists.txt should be enough for importing this Library.
Another CMakeLists.txt is required to include OpenCV4 for this to work.