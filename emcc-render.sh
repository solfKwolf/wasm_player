TOTAL_MEMORY=134217728 

emcc --bind \
	-std=c++11 \
	-I BMIMediaPlayer/interfaces \
	webrender/WebGLYUVRender.cpp \
	webrender/WebALRender.cpp \
	-O3 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	-s TOTAL_MEMORY=${TOTAL_MEMORY} \
	-o webplayer/webplayer/lib/librender.js 
	
