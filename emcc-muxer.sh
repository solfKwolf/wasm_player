TOTAL_MEMORY=134217728 

emcc --bind \
	-D HAVE_CONFIG_H -D DYNAMICLOADPLAYER \
	-std=c++11 \
	-I ffmpeg/include \
	-I BMIMediaPlayer/interfaces \
	webmuxer/*.cpp \
	webresource/webpacket.cpp \
	webresource/webstream.cpp \
	guid/guids.cpp \
	BMIMediaPlayer/playerplugins/Muxer/ffmpegMuxer/muxerffmpeg.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegPacket/packetffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegStream/streamffmpeg*.cpp \
	ffmpeg/lib/libavformat.a \
	ffmpeg/lib/libavcodec.a \
	ffmpeg/lib/libavutil.a \
	-O3 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	-s TOTAL_MEMORY=${TOTAL_MEMORY} \
	-o webplayer/webplayer/lib/libmuxer.js 
