BUILD_DIR=./build
SKETCH_TOOL=/Applications/Sketch.app/Contents/Resources/sketchtool/bin/sketchtool

sketch_icons:
	${SKETCH_TOOL} export artboards ./assets/AppIcon.sketch --output=${BUILD_DIR}/res/AppIcon.iconset
	iconutil -c icns ${BUILD_DIR}/res/AppIcon.iconset
	# rm -rf ${BUILD_DIR}/res/AppIcon.iconset

clean:
	rm -rf ${BUILD_DIR}
	mkdir -p ${BUILD_DIR}
