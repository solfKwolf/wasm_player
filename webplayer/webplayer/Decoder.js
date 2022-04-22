export class Decoder {
    constructor() {
        this.loaded = false;
        this.initFinished = false;
        this.isRunning = false;
        this.frames = new Array;

        this.playerListener = 'undefined';
    }

    delete() {
        if(typeof(this.decoderThread) != 'undefined') {
            this.decoderThread.Release();
        }
    }

    load(listener) {
        this.playerListener = listener;

        var data = new Object;
        data.type = 'loadfinished';
        this.threadListener(data);
    }

    initDecoder(stream) {
        this.decoderThread = new Module.DecodeWorker();

        this.initFinished = false;
        
        var streamFFmpeg = new Module.WebStreamFFmpeg();
        streamFFmpeg.AddRef();
        stream.__proto__ =  WebStream.prototype;
        stream.convertToStreamFFmpeg(streamFFmpeg);
        // 同步初始化
        var ret = this.decoderThread.initDecoder(streamFFmpeg);
        var data = new Object;
        data.type = 'initdecoderfinished';
        data.errorcode = ret;
        this.threadListener(data);
    }

    callPlayerListener(event) {
        if(typeof(this.playerListener) != 'undefined')
            this.playerListener(event);
    }

    threadListener(data) {
        var event = new Object;
        event.data = data;

        if (event.data.type == 'loadfinished') {
            this.loaded = true;
            this.callPlayerListener(event);
        }
        else if (event.data.type == 'initdecoderfinished') {
            if (event.data.errorcode != Module.IDecodeError.NoError) {
                console.log("init decoder error");
                return;
            }
            this.initFinished = true;
            this.callPlayerListener(event);
        }
        else if (event.data.type == "decodepacketfinished") {
            this.isRunning = false;
            this.callPlayerListener(event);
        }
    }

    checkData() {
        var retData = new Module.DecoderData();
        this.decoderThread.getRetData(retData);
        if (retData.Ret() == Module.IDecodeError.ParamError) {
            var tmpThis = this;
            this.timer = setTimeout(function(){
                tmpThis.checkData();
            }, 1);
            retData.Release();
            return;
        }

        let frame = retData.Frame();
        let packet = retData.Packet();
        let error = retData.Ret();
        let data = new Object;
        data.errorcode = error.value;
        data.type = "decodepacketfinished";
        data.webFrame = null;

        if(error == Module.IDecodeError.NoError)
        {
            data.webFrame = new WebFrame(frame);
            data.mediaType = data.webFrame.mediaType;
        }
        else if(error == Module.IDecodeError.DecodeEof)
        {
            console.log('decodeData eof');
        }
        else if(error == Module.IDecodeError.DecodeError)
        {
            console.log('decodeData error');
        }
        else if(error == Module.IDecodeError.FrameUseless)
        {
            console.log('decodeData frame useless');
        }
        this.threadListener(data);
        data.webFrame = null
        if (packet != null) {
            packet.Release();
        }
        if (frame != null) {
            frame.Release();
        }
        retData.Release();
    }

    decodePacket(webpacket) {
        this.isRunning = true;
        var packetFFmpeg = new Module.WebPacketFFmpeg();
        packetFFmpeg.AddRef();
        webpacket.__proto__ =  WebPacket.prototype;
        webpacket.convertToPacketFFmpeg(packetFFmpeg);
        webpacket.packetData = null;

        var frameFFmpeg = new Module.WebFrameFFmpeg();
        frameFFmpeg.AddRef();

        this.decoderThread.decodeData(packetFFmpeg, frameFFmpeg);
        this.checkData();
        frameFFmpeg.Release();
        packetFFmpeg.Release();
        // var data = new Object;
        // data.type = 'decodepacket';
        // data.packet = webpacket;
        // this.decoderThread.postMessage(data, [data.packet.packetData.buffer]);
        // data.packet.packetData = null
    }
}