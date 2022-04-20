TOTAL_MEMORY=134217728 

emcc --bind \
	-D HAVE_CONFIG_H -D DYNAMICLOADPLAYER \
	-std=c++11 \
	-I ffmpeg/include \
	-I BMIMediaPlayer/interfaces \
	webdemuxer/*.cpp \
	webresource/webpacket.cpp \
	webresource/webstream.cpp \
	webresource/webframe.cpp \
	guid/guids.cpp \
	BMIMediaPlayer/playerplugins/Demuxer/ffmpegDemuxer/demuxerffmpeg.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegFrame/frameffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegPacket/packetffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegStream/streamffmpeg*.cpp \
	ffmpeg/lib/libavformat.a \
	ffmpeg/lib/libavcodec.a \
	ffmpeg/lib/libavutil.a \
	-O3 \
	-s WASM=1 \
	-s ASSERTIONS=1 \
	-s ASSERTIONS=1 \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	-s TOTAL_MEMORY=${TOTAL_MEMORY} \
	-o webplayer/webplayer/thread/libdemuxer.js 
