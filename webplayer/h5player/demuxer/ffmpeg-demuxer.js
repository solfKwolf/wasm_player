var ErrorCodeMap = new Map()

let bufferSize = 0;
class FFmpegDemuxer {

    static getErrorMessage(errorCode) {
        if(ErrorCodeMap.size == 0) {
            ErrorCodeMap.set(Module.IDemuxError.DemuxerParamError, "param error");
            ErrorCodeMap.set(Module.IDemuxError.DemuxerInitError, "init muxer error");
            ErrorCodeMap.set(Module.IDemuxError.DemuxerDemuxerEof, "demuxer eof");
            ErrorCodeMap.set(Module.IDemuxError.DemuxerNeedlessConver, "needless cover");
            ErrorCodeMap.set(Module.IDemuxError.DemuxerStreamIndexError, "stream index error");
            ErrorCodeMap.set(Module.IDemuxError.DemuxerDemuxError, "demux data error");
            ErrorCodeMap.set(Module.IDemuxError.DemuxerNotInit, "not init");
        }
        return ErrorCodeMap.get(errorCode)
    }

    constructor() {
        this._emitter = new EventEmitter;
        this._demuxer = new Module.WebDemuxerFFmpeg();
        this._demuxer.AddRef();

        this._readBuffer = [];
        this._meidaSize = 0;
        this._isInit = false;

        this._onLoadedPacket = null;
        this._onLoadedStream = null;
    }
    
    destroy() {
        if(this._demuxer) {
            this._demuxer.Release();
            this._demuxer = null;
        }

        this._emitter = removeAllListeners();
        this._emitter = null;
        this._readBuffer = [];
        this._meidaSize = 0;
        this._isInit = false;

        this._onLoadedPacket = null;
        this._onLoadedStream = null;
    }

    on(event, listener) {
        this._emitter.addListener(event, listener);
    }

    off(event, listener) {
        this._emitter.removeAllListeners(event, listener);
    }

    appendData(data) {
        this._readBuffer.push(data)
        this._meidaSize += data.length;

        if (this._meidaSize < 1024*1024*1 || this._isInit) { // 1M
            return;
        } 

        let result = this._initDemuxer();
        this._isInit = result.match;
        if (result.match === false) {
            this._emitter.emit(DemuxerEvents.INIT_ERROR, {code: result.errorCode, msg: result.message});
            return;
        }
        
        this._emitter.emit(DemuxerEvents.INIT_SUCCESS);

        let webstreams = new Array(this._getStreamCount());
        for (let i = 0; i < this._getStreamCount(); i++) {
            webstreams[i] = this._getStream(i);
        }
        if (!this._onLoadedStream) {
            throw new IllegalStateException('FFmpegDemuxer: _onLoadedStream callback must be specified!');
        }
        this._onLoadedStream(webstreams)

        while(true) {
            let result = this._demux()
            if (!result.match) {
                if(result.errorCode == Module.IDemuxError.DemuxerDemuxerEof) {
                    this._emitter.emit(DemuxerEvents.DEMUXER_EOF)
                } else {
                    this._emitter.emit(DemuxerEvents.DEMUXER_ERROR, {code: result.errorCode, msg: result.message});
                }
                break;
            }
        }
    }

    _initDemuxer() {
        let ioFuncObj = {
            SeekData:this._seekData.bind(this),
            ReadData:this._readData.bind(this),
        };
        let ioCallbackObj = Module.IODevice.implement(ioFuncObj);
        let errorCode = this._demuxer.initDemuxerIO(ioCallbackObj)
        if (errorCode != Module.IDemuxError.DemuxerNoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:FFmpegDemuxer.getErrorMessage(errorCode),
            };
        }
        return {match:true};
    }

    _getStreamCount() {
        if (this._demuxer) {
            return this._demuxer.getStreamCount();
        }
        return 0;
    }

    _getStream(index) {
        let stream = new Module.WebStreamFFmpeg;
        stream.AddRef();
        this._demuxer.getStream(index, stream);
        let webStream = new WebStream(stream);
        stream.Release();
        return webStream;
    }

    _demux() {
        let packet = new Module.WebPacketFFmpeg();
        packet.AddRef();

        let errorCode = this._demuxer.demuxStream(packet);
        let resultPacket = new WebPacket(packet)
        packet.Release();
        if (errorCode != Module.IDemuxError.DemuxerNoError) {
            return {
                match: false,
                errorCode:errorCode,
                message:FFmpegDemuxer.getErrorMessage(errorCode),
            };
        }

        if (!this._onLoadedPacket) {
            throw new IllegalStateException('FFmpegDemuxer: _onLoadedPacket callback must be specified!');
        }
        this._onLoadedPacket(resultPacket)
        return {
            match: true,
        }
    }

    get onLoadedStream() {
        return this._onLoadedStream;
    }

    set onLoadedStream(callback) {
        this._onLoadedStream = callback;
    }

    get onLoadedPacket() {
        return this._onLoadedPacket;
    }

    set onLoadedPacket(callback) {
        this._onLoadedPacket = callback;
    }

    _seekData(position, whence) {
        return position + whence
    }

    _readData(data) {
        if(this._readBuffer.length <= 0) {
            return 0
        }
        let shiftData = this._readBuffer.shift();
        let resultArray;
        if (shiftData.byteLength > data.byteLength) {
            resultArray = shiftData.subarray(0, data.byteLength);

            let newShiftArray = shiftData.subarray(data.byteLength);
            this._readBuffer.unshift(newShiftArray);
        } else {
            resultArray = shiftData.subarray(0, shiftData.byteLength)
        }
        data.set(resultArray, 0);
        bufferSize = bufferSize + resultArray.byteLength;
        return resultArray.byteLength;
    }
}