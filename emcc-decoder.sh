TOTAL_MEMORY=134217728 

emcc --bind \
	-D HAVE_CONFIG_H -D DYNAMICLOADPLAYER \
	-std=c++11 \
	-I ffmpeg/include \
	-I BMIMediaPlayer/interfaces \
	webdecoder/*.cpp \
	webresource/webpacket.cpp \
	webresource/webstream.cpp \
	webresource/webframe.cpp \
	guid/guids.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/audiodecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/realdecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/subitledecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/videodecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/decoderffmpeg.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegFrame/frameffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegPacket/packetffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegStream/streamffmpeg*.cpp \
	ffmpeg/lib/libswscale.a \
	ffmpeg/lib/libavformat.a \
	ffmpeg/lib/libavcodec.a \
	ffmpeg/lib/libavutil.a \
	-O3 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	-s TOTAL_MEMORY=${TOTAL_MEMORY} \
	-o webplayer/webplayer/thread/libdecoder.js 

