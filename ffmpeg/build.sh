
make clean

emconfigure ./configure \
     --cc="emcc" \
     --cxx="em++" \
     --ar="emar" \
     --prefix=../FFmpeg_install/ \
     --cpu=generic \
     --target-os=none \
     --ranlib=emranlib \
     --arch=x86_32 \
     --disable-doc \
     --disable-debug \
     --disable-ffmpeg \
     --disable-ffplay \
     --disable-ffprobe \
     --disable-symver \
     --disable-everything \
     --disable-asm \
     --disable-network \
     --disable-pthreads \
     --enable-swscale \
     --enable-swresample \
     --enable-avresample \
     --enable-gpl \
     --enable-version3 \
     --enable-nonfree \
     --enable-cross-compile \
     --enable-small \
     --enable-parser=aac \
     --enable-parser=h264 \
     --enable-parser=hevc \
     --enable-demuxer=h264 \
     --enable-demuxer=mov \
     --enable-demuxer=aac \
     --enable-demuxer=mpegts \
     --enable-demuxer=flv \
     --enable-decoder=h264 \
     --enable-decoder=hevc \
     --enable-decoder=aac \
     --enable-muxer=mov \
     --enable-protocol=file \
     --enable-protocol=data   
  