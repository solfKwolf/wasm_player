    import {RtspClientSession} from '../rtspcient.js';
    import {H264DataHandle} from '../h264datahandler.js';
    import {H265DataHandle} from '../h265datahandler.js';
    import {DefaultDataHandle} from '../defaultdatahandler.js';

    let databuffer = new Array();

    class RtspHandler{
    constructor(){
        this.mediaHandler = new Map();
        this.RtpEventCallback =(event) =>{this.RtpEventHandler(event)};
        this.RtspEventCallback = (data)=>{this.RtspEventHandler(data)};
        
    }

    WriteMediaHeader(session){
        for(var [key, media] of session.rtpMediaMap){
        
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
        let header = this.mediaHandler.get(key).getHeader();
        databuffer = databuffer.concat(header);
        }
    }

    RtspEventHandler(event){
        switch(event.eventType){
        case 1:
        console.log("WriteMediaHeader " + event.eventType);
        this.WriteMediaHeader(event.session);
        break;
        }
    }

    RtpEventHandler(rtspData){
        if(this.mediaHandler.size <= 0){
        console.log("media handler not init");
        return;
        }
        
        let data = this.mediaHandler.get(rtspData.channelNum).parsingData(rtspData.data);
        if(databuffer.length < 1024*1024*3){
            databuffer = databuffer.concat(data);
        }
    }
    }

    let rtspHandler = new RtspHandler();
    rtspClientSession = new RtspClientSession(rtspHandler.RtpEventCallback,rtspHandler.RtspEventCallback);
    // let rtspurl = "rtsp://192.168.1.103:10556/planback?channel=1&starttime=20181228134420&endtime=20181228151000&isreduce=0";
    let rtspurl = "rtsp://admin:HK123456@192.168.12.16:554";
    // let rtspurl = "rtsp://admin:hk234567@192.168.10.103:554/Streaming/Channels/101?transportmode=unicast&profile=Profil_1";
    rtspClientSession.play("ws://192.168.1.82:8080/websocket", rtspurl);

    function writeFile(){
    return new Promise((resolve, reject) =>{
        let timer = null;
        const task = ()=>{
        if(databuffer.length > 1024*1024*3)
        {
            let video = new Uint8Array(databuffer);
            doSave(video,"video/mp4","video");
            clearInterval(timer);
            resolve();
        } 
        }
        timer = setInterval(task,2000);
    })
    }


    function doSave(value, type, name) {
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


    writeFile();