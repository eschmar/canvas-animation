# Canvas Animation Experiments

https://user-images.githubusercontent.com/2345471/137802875-c386300f-4582-44eb-a5ee-a711fa45d52e.mp4

## build

```
make clean sketch_icons
cd build
cmake .. && make -j8
```

For iOS [check the corresponding guide](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md):

```
cmake -Bbuild-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=9.3 ..
```
