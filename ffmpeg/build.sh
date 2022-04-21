
make clean
CFLAGS="-pthread -s USE_PTHREADS"
LDFLAGS="$CFLAGS -s INITIAL_MEMORY=33554432 -s PTHREAD_POOL_SIZE=4" # 33554432 bytes = 32 MB
emconfigure ./configure \
     --cc="emcc" \
     --cxx="em++" \
     --ar="emar" \
     --nm="llvm-nm" \
     --objcc="emcc" \
     --dep-cc="emcc" \
     --extra-cflags="$CFLAGS" \
     --extra-cxxflags="$CFLAGS" \
     --extra-ldflags="$LDFLAGS" \
     --prefix=/root/player/ffmpeg/ \
     --cpu=generic \
     --target-os=none \
     --ranlib=emranlib \
     --arch=x86_32 \
     --disable-x86asm \
     --disable-doc \
     --disable-stripping \
     --disable-debug \
     --disable-ffmpeg \
     --disable-ffplay \
     --disable-ffprobe \
     --disable-symver \
     --disable-everything \
     --disable-asm \
     --disable-network \
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

emmake make -j4
  