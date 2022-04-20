export class Decoder {
    constructor() {
        this.loaded = false;
        this.initFinished = false;
        this.isRunning = false;

        this.playerListener = 'undefined';
    }

    delete() {
        if(typeof(this.decoderThread) != 'undefined')
            this.decoderThread.terminate();
    }

    load(listener) {
        this.playerListener = listener;

        this.decoderThread = new Worker("../webplayer/thread/WebDecoderThread.js");
        this.decoderThread.addEventListener('message', 
            (event)=>{
                this.threadListener(event);
            }
        );

        var data = new Object;
        data.type = 'load';
        this.decoderThread.postMessage(data);
    }

    initDecoder(stream) {
        this.initFinished = false;
        var data = new Object;
        data.type = 'initdecoder';
        data.stream = stream;
        this.decoderThread.postMessage(data);
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
        else if (event.data.type == 'initdecoderfinished') {
            this.initFinished = true;
            this.callPlayerListener(event);
        }
        else if (event.data.type == "decodepacketfinished") {
            this.isRunning = false;
            this.callPlayerListener(event);
        }
    }

    decodePacket(webpacket) {
        this.isRunning = true;
        var data = new Object;
        data.type = 'decodepacket';
        data.packet = webpacket;
        this.decoderThread.postMessage(data, [data.packet.packetData.buffer]);
        data.packet.packetData = null
    }
}