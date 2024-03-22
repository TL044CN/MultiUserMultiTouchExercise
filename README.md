<a href='https://jenkins.shodan.fyi/job/MultiUserMultiTouch/'><img src='https://jenkins.shodan.fyi/buildStatus/icon?job=MultiUserMultiTouch'></a>
# Multi User Multi Touch
## About
A Small Exercise Build for the Multi User Multi Touch compact course at h_da (Hochschule Darmstadt)

## Dependencies

- Processing Program
    - OpenCV (added as submodule)
    - TUIO1.1 (Provided in the source)
- Display Program
    - electron
    - node-osc
    - typescript
    - copyfiles

## Build

### Processing Program

Run:

```bash 
# Set BUILD_TYPE to one of:
# - Release             (Release Build)
# - Debug               (Debug Build)
# - MinSizeRel          (Minimal Size Release)
# - RelWithDebInfo      (Release with Debug Info)
cmake -B build/ -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build build/ --config=${BUILD_TYPE}
```

on Linux or Windows with cmake installed.
With VSCode this should be easy thanks to automated kit detection.

### Interface / Display

Run:

```bash
build . --target Interface
```

<u>Alternatively</u>:

1. navigate to `/Interface`
2. run `npm install`
3. run `npm run compile`

## Running the Program(s)

The Processing Program is located in the build folder of the top directory.
`build/MultiUserMultiTouchExercise`

The Interface/Display Program can be run by navigating to the `/Interface` directory
and running the command `npm run`