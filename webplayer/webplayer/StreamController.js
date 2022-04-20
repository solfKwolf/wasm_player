
// streamLoaded -- class stream loaded callback function.
// streamInitFinished -- stream init finished callback function.
// renderFrame -- disply(decode and convert frame) frame over callback function.
import {Decoder} from './Decoder.js';
import {Renderer} from './Renderer.js';

export class StreamController {
    constructor(masterClock) {
        this.decoder = new Decoder();

        this.render = new Renderer(masterClock);

        this.packetQueue = new Array();
        this.frameQueue = new Array();

        this.render.startRender = () => { this.convertFrame(); };
        this.render.callPositionChange = (position)=>{this.positionChange(position);};

        this.init();
    }

    init() {
        this.packetQueueSize = 20;
        this.frameQueueSize = 2;

        this.nCacheTime = 2;
        this.isStop = false;

        this.render.init();
    }

    delete() {
        this.decoder.delete();
        this.render.delete();
    }

    load() {
        this.decoder.load(
            (event) => { this.decoderThreadListener(event) }
        );
        this.render.load(
            (event) => { this.renderThreadListener(event) }
        );
    }

    Stop() {
        this.isStop = true;
        this.packetQueue.length = 0;
        this.frameQueue.length = 0;
    }

    Pause() {
        this.render.Pause();
    }

    Resume() {
        this.render.Resume();
    }

    setSpeed(fSpeed) {
        this.render.setSpeed(fSpeed);
    }

    getSpeed() {
        return this.render.getSpeed();
    }

    autoSpeed() {
        this.render.setIsAutoSpeed(true);
    }

    setClockType(clockType) {
        this.render.setClockType(clockType);
    }

    setMaxAutoSpeed(nSpeed) {
        this.render.setMaxAutoSpeed(nSpeed);
    }

    setMinAutoSpeed(nSpeed) {
        this.render.setMinAutoSpeed(nSpeed);
    }

    setCacheTime(nSecond) {
        this.nCacheTime = nSecond;
        this.packetQueueSize = 20;
        this.packetQueueSize = this.nCacheTime * this.packetQueueSize;
    }

    checkStreamLoadState() {
        return this.decoder.loaded && this.render.loaded;
    }

    checkStreamInitState() {
        return this.decoder.initFinished;
    }

    callStreamLoaded() {
        if (typeof (this.streamLoaded) != 'undefined' && this.checkStreamLoadState())
            this.streamLoaded();
    }

    callStreamInitFinished(errorCode) {
        if (typeof (this.streamInitFinished) != 'undefined')
            this.streamInitFinished(errorCode);
    }

    callRenderFrame(data) {
        if (typeof (this.renderFrame) != 'undefined')
            this.renderFrame(data);
    }

    positionChange(position) {
        this.callPositionChange(position);
    }

    decoderThreadListener(event) {
        if (event.data.type == 'loadfinished') {
            console.log('decoder load over');
            this.callStreamLoaded();
        }
        else if (event.data.type == 'initdecoderfinished') {
            this.callStreamInitFinished(event.data.errorcode);
        }
        else if (event.data.type == 'decodepacketfinished') {
            if (!this.isStop) {
                if (event.data.errorcode == 0) {
                    // 生产frame完成，开始消耗
                    this.frameQueue.push(event.data.webFrame);
                    this.convertFrame();
                }
                // frame生产线空,继续生产frame
                this.decodePacket();
            }

        }
    }

    renderThreadListener(event) {
        if (event.data.type == 'loadfinished') {
            console.log('render load over');
            this.callStreamLoaded();
        }
        else if (event.data.type == 'renderframefinished') {
            // 转换frame生产线空,继续转换frame
            this.convertFrame();
            // frame队列变化，尝试解码
            this.decodePacket();
            // 转换完成,完成后续处理
            this.callRenderFrame(event.data);
        }
    }

    initDecoder(webstream) {
        this.decoder.initDecoder(webstream);
    }

    decodePacket() {
        if (this.packetQueue.length > 0 && this.frameQueue < this.frameQueueSize && !this.decoder.isRunning)
            this.decoder.decodePacket(this.packetQueue.shift());
    }

    pushPacket(packet) {
        // 生产pakcet完成，开始消耗
        this.packetQueue.push(packet);
        // packet队列变化，尝试解码
        if(this.decoder.initFinished){
            this.decodePacket();
        } 
    }

    isFull() {
        return this.packetQueue.length >= this.packetQueueSize;
    }

    convertFrame() {
        if (this.frameQueue.length > 0 && !this.render.isRunning && !this.render.isPause) {
            var webframe = this.frameQueue.shift();
            if (webframe.mediaType == 0) {
                this.render.autoSpeed(this.packetQueueSize, this.packetQueue.length);
            }
            this.render.renderFrame(webframe);
        }
    }
}