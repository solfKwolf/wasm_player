var ErrorCodeMap = new Map()

class MP4Muxer {
    static getErrorMessage(errorCode) {
        if(ErrorCodeMap.size == 0) {
            ErrorCodeMap.set(Module.IMuxError.ParamError, "param error");
            ErrorCodeMap.set(Module.IMuxError.InitError, "init muxer error");
            ErrorCodeMap.set(Module.IMuxError.OpenURLError, "open url error");
            ErrorCodeMap.set(Module.IMuxError.CreateStreamError, "create stream error");
            ErrorCodeMap.set(Module.IMuxError.MuxError, "mux packet error");
        }
        return ErrorCodeMap.get(errorCode)
    }
    constructor() {
        this._emitter = new EventEmitter;
        this._muxer = new Module.WebMuxerFFmpeg();
        this._muxer.AddRef();

        this._mediaData = [];

        this._onInitSegment = null;
        this._onMediaSegment = null;
    }

    destroy() {
        if(this._muxer) {
            this._muxer.Release();
            this._muxer = null;
        }

        this._emitter.removeAllListeners();
        this._emitter = null;

        this._onInitSegment = null;
        this._onMediaSegment = null;
    }

    on(event, listener) {
        this._emitter.addListener(event, listener);
    }

    off(event, listener) {
        this._emitter.removeAllListeners(event, listener);
    }

    initMuxer(webstreams) {
        let resultInit = this._initMuxer()
        if (!resultInit.match) {
            this._emitter.emit(MuxerEvents.INIT_ERROR, {code: resultInit.errorCode, msg: resultInit.message});
            return;
        }

        for (let i = 0; i < webstreams.length; i++) {
            this._addMediaStream(webstreams[i]);
        }
        
        let resulteHeader = this._generateHeader()
        if (!resulteHeader.match) {
            this._emitter.emit(MuxerEvents.GENERATE_HEADER_ERROR, {code: resulteHeader.errorCode, msg: resulteHeader.message});
            return;
        }
    }

    addPacket(packet) {
        let result = this._addPacket(packet);
        if(!result.match) {
            this._emitter.emit(MuxerEvents.MUX_PACKET_ERROR, {code: resulteHeader.errorCode, msg: resulteHeader.message});
            return;
        }
    }

    generateTrailer() {
        let result = this._generateTrailer();
        if(!result.match) {
            this._emitter.emit(MuxerEvents.GENERATE_TRAILER_ERROR, {code: resulteHeader.errorCode, msg: resulteHeader.message});
        }
    }

    getMimeCodec() {
        return 'video/mp4;codecs=' + this._muxer.getStrCodecs();
    }

    _initMuxer() {
        let ioFuncObj = {
            WriteData:this._onMediaData.bind(this),
        };

        let ioCallbackObj = Module.IOWriter.implement(ioFuncObj)
        let errorCode = this._muxer.initMuxerIO(ioCallbackObj, "mp4")
        if (errorCode != Module.IMuxError.NoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:MP4Muxer.getErrorMessage(errorCode),
            };
        }
        return {match:true};
    }

    _addMediaStream(webStream) {
        let streamFFmpeg = new Module.WebStreamFFmpeg();
        streamFFmpeg.AddRef();
        webStream.convertToStreamFFmpeg(streamFFmpeg)

        let errorCode = this._muxer.addStream(streamFFmpeg)
        if (errorCode != Module.IMuxError.NoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:MP4Muxer.getErrorMessage(errorCode),
            };
        }
        
        streamFFmpeg.Release()
        return {match:true};
    }

    _generateHeader() {
        let errorCode = this._muxer.writeHeader()
        if (errorCode != Module.IMuxError.NoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:MP4Muxer.getErrorMessage(errorCode),
            };
        }

        if (!this._onInitSegment) {
            throw new IllegalStateException('MP4Remuxer: onInitSegment callback must be specified!');
        }

        if (this._mediaData.length != 0) {
            this._onInitSegment(this._getAllData());
        }

        return {
            match:true,
        };
    }

    _generateTrailer() {
        let errorCode = this._muxer.writeTrailer();
        if (errorCode != Module.IMuxError.NoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:MP4Muxer.getErrorMessage(errorCode),
            };
        }

        if (!this._onMediaSegment) {
            throw new IllegalStateException('MP4Remuxer: onMediaSegment callback must be specified!');
        }

        if (this._mediaData.length != 0) {
            this._onMediaSegment(this._getAllData());
        }

        return {
            match:true,
        };
    }

    _addPacket(packet) {
        let packetFFmpeg = new Module.WebPacketFFmpeg();
        packetFFmpeg.AddRef();
        packet.convertToPacketFFmpeg(packetFFmpeg);
        let errorCode = this._muxer.muxPacket(packetFFmpeg)

        if (errorCode != Module.IMuxError.NoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:MP4Muxer.getErrorMessage(errorCode),
            };
        }
        packetFFmpeg.Release();

        if (!this._onMediaSegment) {
            throw new IllegalStateException('MP4Remuxer: onInitSegment callback must be specified!');
        }

        if (this._mediaData.length != 0) {
            this._onMediaSegment(this._getAllData());
        }

        if (this._mediaData.length != 0) {
            this._mediaData = [];
        }
        
        return {
            match:true
        };
    }

    /* prototype: function onInitSegment(initSegment: ArrayBuffer): void
       InitSegment: {
           data: ArrayBuffer
       }
    */
    get onInitSegment() {
        return this._onInitSegment;
    }

    set onInitSegment(callback) {
        this._onInitSegment = callback;
    }

    /* prototype: function onMediaSegment(mediaSegment: MediaSegment): void
    MediaSegment: {
        data: ArrayBuffer
    }
    */
    get onMediaSegment() {
        return this._onMediaSegment;
    }

    set onMediaSegment(callback) {
        this._onMediaSegment = callback;
    }

    _getAllData() {
        let mediaData = new Uint8Array(this._mediaData);
        mediaData = mediaData.buffer;
        if (this._mediaData.length != 0) {
            this._mediaData = [];
        }
        return mediaData;
    }

    _onMediaData(data) {
        let dataArr = Array.prototype.slice.call(data); //ArrayBuffer to Array
        this._mediaData = this._mediaData.concat(dataArr)
        return  data.length;
    }
}