

import {RtspClientSession} from '../rtspclient/rtspclient.js';
import {H264DataHandle} from '../rtspclient/h264datahandler.js';
import {H265DataHandle} from '../rtspclient/h265datahandler.js';
import {DefaultDataHandle} from '../rtspclient/defaultdatahandler.js';
import {WebPlayer} from './WebPlayer.js';

var readDate;
class RtspHandler{
    constructor(webPlayer){
        this.mediaHandler = new Map();
        this.RtpEventCallback =(event) =>{this.RtpEventHandler(event)};
        this.RtspEventCallback = (data)=>{this.RtspEventHandler(data)};
        this.webPlayer = webPlayer;
    }

    WriteMediaHeader(rtpMediaMap){
        for(var [key, media] of rtpMediaMap){     
            let payload = media.fmt;
            let codecName = "";
            if(media.rtpmap[payload] != null)
                codecName = media.rtpmap[payload].name;
            if("H264" == codecName){
                this.mediaHandler.set(key, new H264DataHandle());
            }
            else if("H265" == codecName){
                this.mediaHandler.set(key, new H265DataHandle());
            }
            else{
                this.mediaHandler.set(key, new DefaultDataHandle());
            }
            let handler = this.mediaHandler.get(key)
            handler.setMediaSubsession(media);
            let header = handler.getHeader();
            header = new Uint8Array(header);  
            media.extradata = header;   
         
        }
        this.webPlayer.setStream(rtpMediaMap);
        
    }

    RtspEventHandler(event){
        switch(event.eventType){
        case 1:
            this.WriteMediaHeader(event.rtpMediaMap);
            break;
        }
    }

    RtpEventHandler(rtspData){
        if(this.mediaHandler.size <= 0){
            console.log("media handler not init");
            return;
        }
        
        let data = this.mediaHandler.get(rtspData.channelNum).parsingData(rtspData.data);

        let packet = new Object();
        packet.data = new Uint8Array(data);
        packet.streamIndex = rtspData.channelNum/2;
        packet.pts = rtspData.curPtsMap.get(rtspData.channelNum).curPts;

        this.webPlayer.setPacket(packet);
    }
}

export class PlayerController {
    constructor(canvasId,width,height) {
        this.canvasId = canvasId;
        this.width = width;
        this.height = height;
        this.currentPos = 0;

        this.player = new WebPlayer();
        this.rtspHandler = new RtspHandler(this.player);
        this.rtspClientSession = new RtspClientSession(this.rtspHandler.RtpEventCallback,this.rtspHandler.RtspEventCallback);
        this.webGLYUVRender = null;
        this.webALRender = null;

        this.player.callSetVideoFormat = (format)=>{this.setVideoRenderFormat(format);};
        this.player.callDrawYUV = (imgData,width,height)=>{this.drawYUV(imgData,width,height);};;
        this.player.callSetAudioFormat = (samepleRate, sampleFormat)=>{this.setAudioRenderFormat(samepleRate, sampleFormat);};
        this.player.callSetAudioData = (audioData)=>{this.setAudioRenderData(audioData);};
        this.player.callPositionChange = (position)=>{this.positionChange(position);};
        this.player.callPlayFinish = ()=>{this.playFinish();};
    }

    loadFile(input) {
        //支持chrome IE10
        if (window.FileReader) {
            var file = input.files[0];
            var reader = new FileReader();
            reader.onload = function () {
                readDate = new Uint8Array(this.result, 0);
                console.log('read over');
            }
            reader.readAsArrayBuffer(file);
        }
    }

    playFunFile() {
        if(!this.webGLYUVRender)
            this.webGLYUVRender = new Module.WebGLYUVRender(this.canvasId,this.width,this.height);
        if(!this.webALRender)
            this.webALRender = new Module.WebALRender();

        this.player.playerLoaded = 'undefined';
        var data = new Uint8Array(readDate);
        this.player.setData(data);
        if (!this.player.initPlay()){
            console.log('init success');
        }     
    }

    playFunRtsp(webURL,rtspURL) {
        if(!this.webGLYUVRender)
            this.webGLYUVRender = new Module.WebGLYUVRender(this.canvasId,this.width,this.height);
        if(!this.webALRender)
            this.webALRender = new Module.WebALRender();

        this.player.playerLoaded = 'undefined';
        if (!this.player.initPlay()){
            console.log('init success');
            this.rtspClientSession.play(webURL,rtspURL);
        }
            
    }

    PlayFile() {
        this.player.isRtspStream = false;
        if (this.player.checkPlayerLoadState()) {
            this.playFunFile();
        }
        else {
            this.player.playerLoaded = ()=>{this.playFunFile();};
        }
    }

    PlayRtsp(webURL,rtspURL) {
        this.player.isRtspStream = true;
        if (this.player.checkPlayerLoadState()) {
            this.playFunRtsp(webURL,rtspURL);
        }
        else {
            this.player.playerLoaded = ()=>{this.playFunRtsp(webURL,rtspURL);};
        }
    }

    stop() {
        this.rtspClientSession.stop();
        this.player.stop();
        this.sleep(100).then(() => {this.webGLYUVRender.clearRenderer();});
    }

    pause() {
        this.rtspClientSession.pause();
        this.player.pause();
    }

    resume() {
        this.rtspClientSession.resume();
        this.player.resume();
    }

    togglePause() {
        if(this.isRunning()){
            this.pause();
        }
        else{
            this.resume();
        }
    }

    seek(nSecond) {
       this.player.seek(nSecond);
    }

    duration() {
        return this.player.duration();
    }

    isRunning() {
        return !(this.player.videoStream.render.isPause && 
                this.player.audioStream.render.isPause )
    }

    volume() {
        return this.webALRender.volume();
    }

    setVolume(volume) {
        this.webALRender.setVolume(volume);
    }

    setScale(scale) {
        // this.player.setScale(scale);
        if(scale >= 0.5 && scale <= 4) {
            this.rtspClientSession.setSpeed(scale);
        }    
    }

    getScale() {
        return this.player.getScale();
    }

    setSpeed(fSpeed) {
        this.player.setSpeed(fSpeed);
        this.webALRender.setSpeed(fSpeed);
    }

    getSpeed() {
        return this.player.getSpeed();
    }

    autoSpeed() {
        this.player.autoSpeed();
    }

    setClockType(clockType) {
        this.player.setClockType(clockType);
    }

    setMaxAutoSpeed(nSpeed) {
        this.player.setMaxAutoSpeed(nSpeed);
    }

    setMinAutoSpeed(nSpeed) {
        this.player.setMinAutoSpeed(nSpeed);
    }

    setCacheTime(nSecond) {
        this.player.setCacheTime(nSecond);
    }

    setCanvasSize(width, height){
        if(width && height)
        {
            this.width = width;
            this.height = height;
        }
    }

    //callback
    setAudioRenderFormat(samepleRate, sampleFormat) {
        this.webALRender.setAudioFormat(samepleRate, sampleFormat);
    }

    setAudioRenderData(audioData) {
        this.webALRender.setAudioData(audioData);
        audioData = null;
    }

    setVideoRenderFormat(format) {
        this.webGLYUVRender.setFormat(format);
    }

    drawYUV(imgData,width,height) {
        this.webGLYUVRender.drawYUV(imgData, width, height);
        imgData = null;
    }

    clearRenderer() {
        this.webGLYUVRender.clearRenderer();
        this.webALRender.clearRenderer();
    }

    positionChange(position) {
        if(this.currentPos != position)
        {
            this.currentPos = position;
        }
    }

    playFinish() {
        console.log("playFinish!");
        this.player.delete();
    }

    //other
    sleep (time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    }

}