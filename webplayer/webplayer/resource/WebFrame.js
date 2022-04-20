class WebFrame {
    constructor(frame) {
        if(frame == null){
            return;
        }
        this.ptsMSec = frame.getPtsMsec();
        this.streamIndex = frame.getStreamIndex();
        this.isEnd = frame.isEnd();
        this.frameName = frame.frameName();
        this.mediaType = frame.getFrameType().value;
        this.frametDuration = frame.getFrameDuration();
        this.nFrameRate = frame.getFramenRate();
        this.nPacketSize = frame.getPacketSize();
        var frameData = frame.getData();
        frameData = new Uint8Array(frameData);
        this.frameData = frameData;

        // video
        this.frameWidth = frame.frameWidth();
        this.frameHeight = frame.frameHeight();
        this.format = frame.getFrameFormat().value;
        // audio
        this.sampleRate = frame.getSampleRate();
        this.audioChannels = frame.getAduioChannels();
        this.audioSampleFormat = frame.getAudioSampleFormat().value;
        this.nbSamples = frame.getNBSamples();
    }

    convertToFrameFFmpeg(frameFFmpeg) {
        switch (this.mediaType) {
            case -1:
                this.mediaType = Module.FrameAVMediaType.AVTypeUnknow;
                break;
            case 0:
                this.mediaType = Module.FrameAVMediaType.AVTypeVideo;
                break;
            case 1:
                this.mediaType = Module.FrameAVMediaType.AVTypeAudio;
                break;
            case 2:
                this.mediaType = Module.FrameAVMediaType.AVTypeData;
                break;
            case 3:
                this.mediaType = Module.FrameAVMediaType.AVTypeSubtitle;
                break;
            case 4:
                this.mediaType = Module.FrameAVMediaType.AVTypeAttachment;
                break;
            default:
                this.mediaType = Module.FrameAVMediaType.AVTypeUnknow;
        }

        switch (this.format) {
            case -1:
                this.format = Module.IFrameFrameFormat.FormatNone;
                break;
            case 0:
                this.format = Module.IFrameFrameFormat.FormatRGB24;
                break;
            case 1:
                this.format = Module.IFrameFrameFormat.FormatRGBA;
                break;
            case 2:
                this.format = Module.IFrameFrameFormat.FormatBGRA;
                break;
            case 3:
                this.format = Module.IFrameFrameFormat.FormatGRAY8;
                break;
            case 4:
                this.format = Module.IFrameFrameFormat.FormatYUV420P;
                break;
            case 5:
            this.format = Module.IFrameFrameFormat.ForamtARGB;
                break;
            case 6:
                this.format = Module.IFrameFrameFormat.ForamtNV12;
                break;
            case 7:
                this.format = Module.IFrameFrameFormat.ForamtNV21;
                break;
            default:
                this.format = Module.IFrameFrameFormat.FormatNone;
        }

        frameFFmpeg.setPtsMsec(this.ptsMSec);
        frameFFmpeg.setStreamIndex(this.streamIndex);
        frameFFmpeg.setEofFlag(this.isEnd);
        frameFFmpeg.setFrameType(this.mediaType);
        frameFFmpeg.setFrameDuration(this.frametDuration);
        frameFFmpeg.setFramenRate(this.nFrameRate);
        frameFFmpeg.setPacketSize(this.nPacketSize);
        frameFFmpeg.setData(this.frameData);

        // video
        frameFFmpeg.setFrameSize(this.frameWidth, this.frameHeight);
        frameFFmpeg.setFrameFormat(this.format);
        // audio
        frameFFmpeg.setSampleRate(this.sampleRate);
        frameFFmpeg.setAudioChannels(this.audioChannels);

        switch(this.audioSampleFormat) {
        case -1:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateNone;
            break;
        case 0:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateU8;
            break;
        case 1:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateS16;
            break;
        case 2:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateS32;
            break;
        case 3:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateFLT;
            break;
        case 4:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateDBL;
            break;
        case 5:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateU8P;
            break;
        case 6:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateS16P;
            break;
        case 7:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateS32P;
            break;
        case 8:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateFLTP;
            break;
        case 9:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateDBLP;
            break;
        case 10:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateS64;
            break;
        case 11:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateS64P;
            break;    
        default:
            this.audioSampleFormat = Module.IFrameAudioSampleFormat.AudioFormateNone;
        }
        frameFFmpeg.setAudioSampleFormat(this.audioSampleFormat);
        frameFFmpeg.setNBSamples(this.nbSamples);



    }
}