TOTAL_MEMORY=671088640

emcc --bind \
	-D HAVE_CONFIG_H -D DYNAMICLOADPLAYER \
	-std=c++11 \
	-I ffmpeg/include \
	-I BMIMediaPlayer/interfaces \
	webmuxer/*.cpp \
	webdecoder/decode_worker.cpp \
	webresource/webframe.cpp \
	webresource/webpacket.cpp \
	webresource/webstream.cpp \
	guid/guids.cpp \
	webrender/WebGLYUVRender.cpp \
	webrender/WebALRender.cpp \
	BMIMediaPlayer/playerplugins/Muxer/ffmpegMuxer/muxerffmpeg.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegPacket/packetffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegStream/streamffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Resource/ffmpegFrame/frameffmpeg*.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/audiodecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/realdecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/subitledecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/videodecoder.cpp \
	BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/decoderffmpeg.cpp \
	ffmpeg/lib/libswscale.a \
	ffmpeg/lib/libavformat.a \
	ffmpeg/lib/libavcodec.a \
	ffmpeg/lib/libavutil.a \
	-O3 \
	-pthread \
	-s PTHREAD_POOL_SIZE=20 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	-s TOTAL_MEMORY=${TOTAL_MEMORY} \
	-o webplayer/webplayer/lib/libplayer.js 
