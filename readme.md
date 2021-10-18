## Canvas Animation Experiments

```
make clean sketch_icons
cd build
cmake .. && make -j8
```

For iOS [check the corresponding guide](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md):

```
cmake -Bbuild-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=9.3 ..
```
