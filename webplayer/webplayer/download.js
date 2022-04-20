import {RtspClientSession} from '../rtspclient/rtspclient.js';
import {H264DataHandle} from '../rtspclient/h264datahandler.js';
import {H265DataHandle} from '../rtspclient/h265datahandler.js';
import {DefaultDataHandle} from '../rtspclient/defaultdatahandler.js';

var g_buffer = new Array();
var g_bufferSize = 1024*1024*5;
var g_duration = 1000*60*30;

class RtspHandler{
    constructor(download){
        this.mediaHandler = new Map();
        this.RtpEventCallback =(event) =>{this.RtpEventHandler(event)};
        this.RtspEventCallback = (data)=>{this.RtspEventHandler(data)};
        this.download = download;
    }

    WriteMediaHeader(rtpMediaMap){
        for(var [key, media] of rtpMediaMap){     
            let payload = media.fmt;
            let codecName = media.rtpmap[payload].name;
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

            // g_buffer = g_buffer.concat(header);        
        } 

        this.download.setStream(rtpMediaMap);

        for(var [key, media] of rtpMediaMap){
            let channelNum = key/2;

            let handler = this.mediaHandler.get(key)
            let header = handler.getHeader();
            header = new Uint8Array(header);  

            let packet = new Object();
            packet.data = new Uint8Array(header);
            packet.streamIndex = channelNum;
            packet.pts = 0;

            this.download.setPacket(packet);
        }

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

        // g_buffer = g_buffer.concat(data); 

        // if (g_buffer.length > g_bufferSize)
        // {
        //     console.log("RtpEventHandler: ",g_buffer.length)
        // }

        let packet = new Object();
        packet.data = new Uint8Array(data);
        packet.streamIndex = rtspData.channelNum/2;
        packet.pts = rtspData.curPtsMap.get(rtspData.channelNum).curPts;

        this.download.setPacket(packet);

    }
}

export class Download{
    constructor(){
        this.buffer = new Array();
        this.packetQueue = new Array();

        this.rtspHandler = new RtspHandler(this);
        this.rtspClientSession = new RtspClientSession(this.rtspHandler.RtpEventCallback,this.rtspHandler.RtspEventCallback);

        this.initMuxer();
    }

    initMuxer(){
        var func = {
            WriteData: function(data){
                let dataArr = Array.prototype.slice.call(new Uint8Array(data)); //ArrayBuffer to Array
                console.log("WriteData : ", g_buffer.length);
                g_buffer = g_buffer.concat(dataArr);
                return data.length;
            }
        };
        var interfaceObj = Module.IOWriter.implement(func);

        this.muxerFFmpeg = new Module.WebMuxerFFmpeg();
        this.muxerFFmpeg.AddRef();

        let errcode = this.muxerFFmpeg.initMuxerIO(interfaceObj, "mpegts")
        if (errcode != Module.IMuxError.NoError) {
            console.log('init muxer error: ', errcode);
            return;
        }
        console.log('init muxer success');
    }

    startDownload(webURL,rtspURL){
        if (webURL != null && rtspURL != null)
        {
            this.rtspClientSession.play(webURL,rtspURL);
            this.writeFile();
        }
    }

    setPacket(packet){
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
            if (webPacket.streamIndex == 0)
                this.packetQueue.push(webPacket);
                this.muxPacket();
            // else if (webPacket.mediaType == 1){}
                // this.audioStream.pushPacket(webPacket);
        }
    }

    setStream(rtpMediaMap){
        for(var [key, media] of rtpMediaMap){
            var payload = media.fmt;
            var codecName = media.rtpmap[payload].name;
            var extradata = media.extradata;

            var streamIndex = key/2;
            var streamType = -1;
            if(media.type == "video"){
                streamType = 0;
            }
            else if(media.type == "audio"){
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
            console.log("write header")
            
            if (streamIndex == 0){
                var webStream = new WebStream();
                webStream.streamIndex = streamIndex;
                webStream.streamType = streamType;
                webStream.streamCodecType = streamCodecType;    
                webStream.codecExtradata = extradata;
                webStream.frameWidth = 1920;
                webStream.frameHeight = 1080;
                console.log("extradata:", extradata)
    
                var streamFFmpeg = new Module.WebStreamFFmpeg();
                streamFFmpeg.AddRef();
                webStream.convertToStreamFFmpeg(streamFFmpeg);
    
                var errorCode = this.muxerFFmpeg.addStream(streamFFmpeg);
                if (errorCode != Module.IMuxError.NoError) {
                    console.log('add stream error.');
                }
                streamFFmpeg.Release();
            }   
        }   
        if (this.muxerFFmpeg.writeHeader() != Module.IMuxError.NoError) {
            console.log('write header error.');
        }
    }   

    muxPacket(){
        let packet = this.packetQueue.shift();
        if (packet != null){
            var packetFFmpeg = new Module.WebPacketFFmpeg();
            packetFFmpeg.AddRef();
            packet.convertToPacketFFmpeg(packetFFmpeg);
            var errcode = this.muxerFFmpeg.muxPacket(packetFFmpeg);
            if (errcode != Module.IMuxError.NoError) {
                console.log("mux packet error.");
            }
            packetFFmpeg.Release();
        }
    } 

    writeFile(){
        return new Promise((resolve, reject) =>{
            let timer = null;
            const task = ()=>{
                if(g_buffer.length > g_bufferSize)
                {
                    this.rtspClientSession.stop();
                    clearInterval(timer);
                    let video = new Uint8Array(g_buffer);
                    this.doSave(video,"video/ts","video.ts"); 
                    resolve();
                } 
            }
            timer = setInterval(task,2000);
        })
    }

    doSave(value, type, name) {
        var blob;
        if (typeof window.Blob == "function") {
            blob = new Blob([value], {type: type});
        } else {
            var BlobBuilder = window.BlobBuilder || window.MozBlobBuilder || window.WebKitBlobBuilder || window.MSBlobBuilder;
            var bb = new BlobBuilder();
            bb.append(value);
            blob = bb.getBlob(type);
        }
        var URL = window.URL || window.webkitURL;
        var bloburl = URL.createObjectURL(blob);
        var anchor = document.createElement("a");
        if ('download' in anchor) {
            anchor.style.visibility = "hidden";
            anchor.href = bloburl;
            anchor.download = name;
            document.body.appendChild(anchor);
            var evt = document.createEvent("MouseEvents");
            evt.initEvent("click", true, true);
            anchor.dispatchEvent(evt);
            document.body.removeChild(anchor);
        } else if (navigator.msSaveBlob) {
            navigator.msSaveBlob(blob, name);
        } else {
            location.href = bloburl;
        }
    }
}
