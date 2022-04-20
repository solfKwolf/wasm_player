export class Demuxer {
    constructor() {
        this.init();
        this.loaded = false;
        this.playerListener = 'undefined';
    }

    init() {
        this.initFinished = false;
        this.demuxFinished = false;
        this.isRunning = false;

        this.duration = 0;
        
        this.initSeekInfo();
        this.initScaleInfo();
    }

    initSeekInfo(){
        this.seekInfo = {
            needSeek : false,
            nSeekSecond : 0
        }
    }

    initScaleInfo() {
        this.scaleInfo = {
            needScale : false,
            fScale : 1.0
        }
    }

    delete() {
        if(typeof(this.demuxerThread) != 'undefined')
            this.demuxerThread.terminate();
    }

    Seek(nSecond) {
        this.seekInfo.needSeek = true;
        this.seekInfo.nSeekSecond = nSecond;
        return true;
    }

    Duration() {
        return this.duration;
    }

    setScale(scale) {
        this.scaleInfo.needScale = true;
        this.scaleInfo.fScale = scale;
    }

    getScale() {
        return this.scaleInfo.fScale;
    }

    load(listener) {
        this.playerListener = listener;

        this.demuxerThread = new Worker("../webplayer/thread/WebDemuxerThread.js");
        this.demuxerThread.addEventListener('message', 
            (event)=>{
                this.threadListener(event);
            }
        );

        var data = new Object;
        data.type = 'load';
        this.demuxerThread.postMessage(data);
    }

    setData(mediaData) {
        var data = new Object;
        data.type = 'setmediadata';
        data.mediaData = mediaData;
        this.demuxerThread.postMessage(data, [data.mediaData.buffer]);
        data.mediaData = null;
    }

    initDemuxer() {
        if(this.loaded == false)
            return false;

        this.initFinished = false;
        this.demuxFinished = false;

        var data = new Object;
        data.type = 'initdemuxer';
        this.demuxerThread.postMessage(data);
        return true;
    }

    callPlayerListener(event) {
        if(typeof(this.playerListener) != 'undefined')
            this.playerListener(event);
    }

    threadListener(event) {
        if (event.data.type == 'loadfinished') {
            this.loaded = true;
            this.callPlayerListener(event);
        }
        else if (event.data.type == 'initdemuxerfinished') {
            this.initFinished = true;
            this.duration = event.data.duration;
            this.callPlayerListener(event);
        }
        else if (event.data.type == 'demuxpakcetfinished') {
            this.isRunning = false;
            var error = event.data.errorcode;
            if(error == 3)
                this.demuxFinished = true;
            this.callPlayerListener(event);
        }
        else if (event.data.type == 'seekfinished') {
            this.initSeekInfo();
        }
        else if (event.data.type == 'setScalefinished') {
            this.initScaleInfo();
        }
    }

    demuxPacket() {
        this.isRunning = true;
        var data = new Object;
        data.type = 'demuxpacket';
        data.seekInfo = this.seekInfo;
        data.scaleInfo = this.scaleInfo;
        this.demuxerThread.postMessage(data);
    }
}