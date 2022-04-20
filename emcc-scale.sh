TOTAL_MEMORY=134217728 

emcc --bind \
	-D HAVE_CONFIG_H -D DYNAMICLOADPLAYER \
	-std=c++11 \
	-I ffmpeg/include \
	-I BMIMediaPlayer/interfaces \
	webresource/webpacket.cpp \
	webresource/webstream.cpp \
	webresource/webframe.cpp \
	webswscaler/*.cpp \
	guid/guids.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegFrame/frameffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegPacket/packetffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegStream/streamffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Swscale/ffmpegSwscaler/swscalerffmpeg.cpp \
	ffmpeg/lib/libswresample.a \
	ffmpeg/lib/libswscale.a \
	ffmpeg/lib/libavutil.a \
	-O3 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	-s TOTAL_MEMORY=${TOTAL_MEMORY} \
	-o webplayer/webplayer/thread/libscale.js 
