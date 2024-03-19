<a href='https://jenkins.shodan.fyi/job/MultiUserMultiTouch/'><img src='https://jenkins.shodan.fyi/buildStatus/icon?job=MultiUserMultiTouch'></a>
# Multi User Multi Touch
## About

## Dependencies

## Build
to build the Project run:
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
