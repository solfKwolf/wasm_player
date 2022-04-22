
import {StreamController} from './StreamController.js';
import {Demuxer} from './Demuxer.js';
import {MasterClock} from './common/MasterClock.js';

export class WebPlayer {
    constructor() {
        this.init();
        this.isRtspStream = true;

        this.demuxer = new Demuxer();
        this.demuxer.load(
            (event) => { this.demuxerThreadListener(event); }
        );

        this.masterClock = new MasterClock();

        this.videoStream = new StreamController(this.masterClock);
        this.videoStream.streamLoaded = () => { this.callPlayerLoaded(); };
        this.videoStream.streamInitFinished = (errorcode) => { this.startPlay(); };
        this.videoStream.renderFrame = (data) => { this.renderVideoFrame(data); };
        this.videoStream.callPositionChange = (position)=>{this.positionChange(position);};
        this.videoStream.load();

        this.audioStream = new StreamController(this.masterClock);
        this.audioStream.streamLoaded = () => { this.callPlayerLoaded(); };
        this.audioStream.streamInitFinished = () => { this.startPlay(); };
        this.audioStream.renderFrame = (data) => { this.renderAudioFrame(data); };
        this.audioStream.load();   

    }

    init() {
        this.isPlaying = false;
        this.isStop = false;
    }

    initPlay() {
        this.init(); 
        this.videoStream.init();
        this.audioStream.init();
        if(!this.isRtspStream){
            this.demuxer.init();
            this.demuxer.initDemuxer();
        }    
    }


    delete() {
        this.demuxer.delete();
        this.videoStream.delete();
        this.audioStream.delete();
    }

    setData(mediaData) {
        this.demuxer.setData(mediaData);
    }

    setPacket(packet) {
        var mediaType = -1;
        if(packet.streamIndex == 0){
            mediaType = 0;
        }
        else if(packet.streamIndex == 1){
            mediaType = 1;
        }

        var webPacket = new WebPacket();
        webPacket.streamIndex = packet.streamIndex;
        webPacket.packetData = packet.data;
        webPacket.ptsMSec = packet.pts;
        webPacket.dtsMSec = packet.pts;
        webPacket.isKeyframe = false;
        webPacket.isEnd = this.isStop;
        webPacket.mediaType = mediaType;
        webPacket.packetPos = 0;
        webPacket.packetDuration = 0;
        webPacket.packetFlag = 0;

        if (!this.isStop) {
            if (webPacket.mediaType == 0)
                this.videoStream.pushPacket(webPacket);
            // else if (webPacket.mediaType == 1){}
                // this.audioStream.pushPacket(webPacket);
        }
    }

    setStream(rtpMediaMap){
        var webStreams = new Array();
        for(var [key, media] of rtpMediaMap){
            var payload = media.fmt;
            var codecName = "";
            if(media.rtpmap[payload] != null)
                codecName = media.rtpmap[payload].name;
            var extradata = media.extradata;

            var streamIndex = key/2;
            var streamType = -1;
            if(streamIndex == 0){
                streamType = 0;
            }
            else if(streamIndex == 1){
                streamType = 1;
            }
            
            var streamCodecType = -1
            if (codecName == "H264"){
                streamCodecType = 0;
            }
            else if (codecName == "H265"){
                streamCodecType = 1;
            }
            else if (codecName == "AAC"){
                streamCodecType = 2;
            }
            else if (codecName == "PCMA"){
                // streamCodecType = 4;
            }     
            
            var webStream = new WebStream();
            webStream.streamIndex = streamIndex;
            webStream.streamType = streamType;
            webStream.streamCodecType = streamCodecType;    
            webStream.codecExtradata = extradata;

            webStreams.push(webStream);
        }   
        this.initDecoder(webStreams);        
    }

    stop() {
        this.isStop = true;
        this.videoStream.Stop();
        this.audioStream.Stop();
    }

    pause() {
        this.videoStream.Pause();
        this.audioStream.Pause();
    }

    resume() {
        this.videoStream.Resume();
        this.audioStream.Resume();
    }

    seek(nSecond){
        this.demuxer.Seek(nSecond);
    }

    duration() {
        return this.demuxer.Duration();
    }

    setScale(scale) {
        this.demuxer.setScale(scale);
    }

    getScale() {
        return this.demuxer.getScale();
    }

    setSpeed(fSpeed) {
        this.videoStream.setSpeed(fSpeed);
    }

    getSpeed() {
        return this.videoStream.getSpeed();
    }

    autoSpeed() {
        this.videoStream.autoSpeed();
    }

    setClockType(clockType) {
        this.videoStream.setClockType(clockType);
        this.audioStream.setClockType(clockType);
    }

    setMaxAutoSpeed(nSpeed) {
        this.videoStream.setMaxAutoSpeed(nSpeed);
        this.audioStream.setMaxAutoSpeed(nSpeed);
    }

    setMinAutoSpeed(nSpeed) {
        this.videoStream.setMinAutoSpeed(nSpeed);
        this.audioStream.setMinAutoSpeed(nSpeed);
    }

    setCacheTime(nSecond) {
        this.videoStream.setCacheTime(nSecond);
        this.audioStream.setCacheTime(nSecond);
    }

    checkPlayerLoadState() {
        return this.demuxer.loaded && this.videoStream.checkStreamLoadState() && this.audioStream.checkStreamLoadState();
    }

    callPlayerLoaded() {
        if (typeof (this.playerLoaded) != 'undefined' && this.checkPlayerLoadState())
            this.playerLoaded();
    }

    checkMediaLoadState() {
        if(this.isRtspStream){
            return true;
        }
        else
        {
            return this.demuxer.initFinished && this.videoStream.checkStreamInitState() && this.audioStream.checkStreamInitState();
        }
        
    }

    startPlay() {
        if (this.isPlaying == true)
            return;
        if (this.checkMediaLoadState() == true) {
            if(!this.isRtspStream){
                this.demuxer.demuxPacket();
            }  
            this.isPlaying = true;
        }
    }

    initDecoder(webstreams) {
        for (var i = 0; i < webstreams.length; ++i) {
            if (webstreams[i].streamType == 0) {
                var nPixelFormat = 4;
                this.callSetVideoFormat(nPixelFormat);
                this.videoStream.initDecoder(webstreams[i])
            }
            else if (webstreams[i].streamType == 1) {
                var samepleRate = webstreams[i].nSampleRate;
                var sampleFormat = webstreams[i].audioSampleFormat;
                this.callSetAudioFormat(samepleRate, sampleFormat);
                this.audioStream.initDecoder(webstreams[i])
            }
        }
    }

    demuxPakcet() {
        if (!this.demuxer.demuxFinished && !this.demuxer.isRunning &&
            !this.videoStream.isFull() && !this.audioStream.isFull())
            this.demuxer.demuxPacket();
    }

    demuxPakcetFinished(data) {
        var error = data.errorcode;
        var packet = data.webPacket;
        if (error == 0) {
            if (!this.isStop) {
                if (packet.mediaType == 0)
                    this.videoStream.pushPacket(packet);
                else if (packet.mediaType == 1)
                    this.audioStream.pushPacket(packet);
                this.demuxPakcet();
            }
        }
        else if (error == 3) {
            // this.callPlayFinish();
            console.log('demux eof.');
        }
        else {
            console.log('demux error.');
        }
    }

    renderVideoFrame(data) {
        if(!data.isDiscard)
            this.callDrawYUV(data.webFrame.frameData, data.webFrame.frameWidth, data.webFrame.frameHeight);
        if(!this.isRtspStream){
            this.demuxPakcet();
        }
        data.webFrame.frameData = null;
    }

    renderAudioFrame(data) {
        if(!data.isDiscard)
            this.callSetAudioData(data.webFrame.frameData);
        if(!this.isRtspStream){
            this.demuxPakcet();
        }
        if (!data.isDiscard) {
            data.webFrame.frameData = null;
        }
    }

    positionChange(position) {
        this.callPositionChange(position);
    }

    playFinish() {
        this.callPlayFinish();
    }

    demuxerThreadListener(event) {
        if (event.data.type == 'loadfinished') {
            console.log("demuxer thread load finished.");
            this.callPlayerLoaded();
        }
        else if (event.data.type == 'initdemuxerfinished') {
            this.initDecoder(event.data.webStreams);
        }
        else if (event.data.type == 'demuxpakcetfinished') {
            this.demuxPakcetFinished(event.data);
        }
        else if (event.data.type == 'seekfinished') {
            this.callPlayerListener(event);
        }
        else if (event.data.type == 'durationfinished') {
            this.callPlayerListener(event);
        }
    }
}