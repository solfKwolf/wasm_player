class WebStream {
    constructor(stream) {
        if(stream == null){
            this.duration = 0;
            this.streamTitle = "";
            this.frameWidth = 0;
            this.frameHeight = 0;
            this.frameFormat = -1;
            this.nSampleRate = 0;
            this.nChannelCount = 0;
            this.audioSampleFormat = -1;
            this.timebase = [1,1000];
            this.frameRate = [1,1];
            return;
        }
        this.streamType = stream.getStreamType().value;
        this.streamCodecType = stream.getStreamCodecType().value;
        this.duration = stream.getDuration();
        this.streamIndex = stream.getStreamIndex();  
        this.streamName= stream.streamName(); 
        this.streamTitle = stream.getStreamTitle();

        this.frameWidth = stream.getFrameWidth();
        this.frameHeight = stream.getFrameHeight();

        this.frameFormat = stream.getFrameFormat().value;

        this.nSampleRate = stream.getAudioSampleRate();
        this.nChannelCount = stream.getAudioChannelCount();
        this.audioSampleFormat = stream.getAudioSampleFormat().value;

        this.timebase = stream.getTimebase();
        this.frameRate = stream.getFrameRate();

        let codecExtradata = stream.getCodecExtradataMem();
        this.codecExtradata = new Uint8Array(codecExtradata.length);
        this.codecExtradata.set(codecExtradata, 0);
    }

    convertToStreamFFmpeg(streamFFmpeg) {
        switch (this.streamType) {
            case -1:
                this.streamType = Module.StreamAVMediaType.AVTypeUnknow;
                break;
            case 0:
                this.streamType = Module.StreamAVMediaType.AVTypeVideo;
                break;
            case 1:
                this.streamType = Module.StreamAVMediaType.AVTypeAudio;
                break;
            case 2:
                this.streamType = Module.StreamAVMediaType.AVTypeData;
                break;
            case 3:
                this.streamType = Module.StreamAVMediaType.AVTypeSubtitle;
                break;
            case 4:
                this.streamType = Module.StreamAVMediaType.AVTypeAttachment;
                break;
            default:
                this.streamType = Module.StreamAVMediaType.AVTypeUnknow;
        }

        streamFFmpeg.setStreamType(this.streamType);

        switch (this.streamCodecType) {
            case -1:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypeUnkow;
                break;
            case 0:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypeH264;
                break;
            case 1:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypeHevc;
                break;
            case 2:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypeAAC;
                break;
            case 3:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypePCMMULAM;
                break;
            case 4:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypePCMALAW;
                break;
            default:
                this.streamCodecType = Module.StreamAVCodecType.CodecTypeUnkow;
                break;
        }
        streamFFmpeg.setStreamCodecType(this.streamCodecType);

        streamFFmpeg.setDuration(this.duration);
        streamFFmpeg.setStreamIndex(this.streamIndex);
        streamFFmpeg.setStreamTitle(this.streamTitle);


        switch (this.frameFormat) {
            case -1:
                this.streamCodecType = Module.StreamFrameFormat.FormatNone;
                break;
            case 0:
                this.streamCodecType = Module.StreamFrameFormat.FormatRGB24;
                break;
            case 1:
                this.streamCodecType = Module.StreamFrameFormat.FormatRGBA;
                break;
            case 2:
                this.streamCodecType = Module.StreamFrameFormat.FormatBGRA;
                break;
            case 3:
                this.streamCodecType = Module.StreamFrameFormat.FormatGRAY8;
                break;
            case 4:
                this.streamCodecType = Module.StreamFrameFormat.FormatYUV420P;
                break;
            case 5:
                this.streamCodecType = Module.StreamFrameFormat.ForamtARGB;
                break;
            case 6:
                this.streamCodecType = Module.StreamFrameFormat.ForamtNV12;
                break;
            case 7:
                this.streamCodecType = Module.StreamFrameFormat.ForamtNV12;
                break;
            default:
                this.streamCodecType = Module.StreamFrameFormat.FormatNone;
                break;
        }

        streamFFmpeg.setFrameFormat(this.frameFormat);

        streamFFmpeg.setFrameSize(this.frameWidth,this.frameHeight);

        switch(this.audioSampleFormat) {
            case -1:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateNone;
                break;
            case 0:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateU8;
                break;
            case 1:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateS16;
                break;
            case 2:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateS32;
                break;
            case 3:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateFLT;
                break;
            case 4:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateDBL;
                break;
            case 5:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateU8P;
                break;
            case 6:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateS16P;
                break;
            case 7:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateS32P;
                break;
            case 8:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateFLTP;
                break;
            case 9:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateDBLP;
                break;
            case 10:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateS64;
                break;
            case 11:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateS64P;
                break;    
            default:
                this.audioSampleFormat = Module.StreamAudioSampleFormat.AudioFormateNone;
            }
        streamFFmpeg.setAudioFormat(this.nSampleRate,this.nChannelCount,this.audioSampleFormat);

        streamFFmpeg.setTimebase(this.timebase);
        streamFFmpeg.setFrameRate(this.frameRate);

        streamFFmpeg.setCodecExtradata(this.codecExtradata);
    }
}