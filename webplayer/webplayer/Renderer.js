export class Renderer {
    constructor(masterClock) {
        this.masterClock = masterClock;
        // this.masterClock.__proto__ =  MasterClock.prototype;

        this.init();

        this.loaded = false;
        this.playerListener = 'undefined';
        this.isAutoSpeedFlag = false;
        this.minSpeed = 0.9;
        this.maxSpeed = 1.1;

        if(typeof this.SpeedModel == "undefined"){
            this.SpeedModel = {
                SpeedNormal : 0,
                SpeedUp : 1,
                SpeedDown : 2
            }
        }
        this.speedModel = this.SpeedModel.SpeedNormal;

        if(typeof this.ClockType == "undefined"){
            this.ClockType = {
                VideoClock : 0,
                AudioClock : 1,
                OtherClock : 2,
                NoneClock : 3
            }
        }
        this.clockType = this.ClockType.VideoClock;
    }

    init() {
        this.isRunning = false;
        this.isPause = false;
        this.masterClock.setFirstFrame(true);
    }

    delete() {
        if(typeof(this.renderThread) != 'undefined') {
            this.renderThread.terminate();
        }
    }

    load(listener) {
        this.playerListener = listener;

        this.renderThread = new Worker("../webplayer/thread/WebRenderThread.js");
        this.renderThread.addEventListener('message',
            (event)=>{
                this.threadListener(event);
            }
        );

        var data = new Object;
        data.type = 'load';
        this.renderThread.postMessage(data);
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
        else if(event.data.type == 'renderframefinished'){
            this.isRunning = false;
            this.callPlayerListener(event);
        }
    }

    renderFrame(webframe) {
        var data = new Object;
        data.type = 'renderframe';
        data.frame = webframe;
        data.isDiscard = false;
        data.clock = this.masterClock.getClock();
        if(this.masterClock.IsFirstFrame())
        {
            if(webframe.ptsMSec >= 0)
            {
                this.masterClock.seekClock(webframe.ptsMSec);
                data.clock = this.masterClock.getClock();
                this.masterClock.setFirstFrame(false);
            }     
        }
        if(webframe.ptsMSec - data.clock > 2000)
        {
            this.masterClock.seekClock(webframe.ptsMSec);
            data.clock = this.masterClock.getClock();
        }
        if(webframe.ptsMSec + webframe.frametDuration <= data.clock)
        {
            var clockType = this.getClockType();
            if(webframe.mediaType == 0 && clockType == this.ClockType.VideoClock)
            {
                this.masterClock.seekClock(webframe.ptsMSec + webframe.frametDuration);
                data.clock = this.masterClock.getClock();
            }
            else if(webframe.mediaType == 1 && clockType == this.ClockType.AudioClock)
            {
                this.masterClock.seekClock(webframe.ptsMSec + webframe.frametDuration);
                data.clock = this.masterClock.getClock();
            }
            else if(clockType == this.ClockType.OtherClock)
            {
                this.masterClock.seekClock(webframe.ptsMSec + webframe.frametDuration);
            }
            else
            {
                console.log("discard frame...");
                data.isDiscard = true;
            }
        }

        if(webframe.mediaType == 0){
            this.callPositionChange(data.clock/1000);
        } 

        this.isRunning = true;
        this.renderThread.postMessage(data, [data.frame.frameData.buffer]);
        data.frame.frameData = null
    }

    Pause() {
        this.isPause = true;
        this.masterClock.reset();
        this.masterClock.pauseClock();
    }

    Resume() {
        this.isPause = false;
        this.startRender();
    }

    setSpeed(fSpeed){
        this.masterClock.setSpeed(fSpeed);
    }

    getSpeed() {
        return this.masterClock.getSpeed();
    }

    setIsAutoSpeed(isAutoSpeed) {
        this.isAutoSpeedFlag = isAutoSpeed;
    }

    autoSpeed(nQueueMax,nQueueSize) {
        // console.log("packet queue size is: ", nQueueSize);
        if(!this.isAutoSpeedFlag)
        {
            return;
        }
        // 缓存不多时0.9倍速播放
        if(nQueueSize < nQueueMax * 1/3)
        {
            if(this.speedModel == this.SpeedModel.SpeedNormal)
            {
                this.speedModel == this.SpeedModel.SpeedDown;
                this.setSpeed(this.minSpeed);
            }
            else if(this.speedModel == this.SpeedModel.SpeedUp)
            {
                this.speedModel == this.SpeedModel.SpeedDown;
                this.setSpeed(this.minSpeed);
            }
        }
        // 缓存大于最大缓冲区1/2时1.1倍速播放
        
        else if(nQueueSize > nQueueMax * 1/2)
        {
            if(this.speedModel == this.SpeedModel.SpeedNormal)
            {
                this.speedModel == this.SpeedModel.SpeedUp;
                this.setSpeed(this.maxSpeed);
            }
            else if(this.speedModel == this.SpeedModel.SpeedDown)
            {
                this.speedModel == this.SpeedModel.SpeedUp;
                this.setSpeed(this.maxSpeed);
            }
        }
        else if(this.speedModel != this.SpeedModel.SpeedNormal)
        {
            this.speedModel = this.SpeedModel.SpeedNormal;
            this.setSpeed(1.0);
        }
    }

    setMaxAutoSpeed(nSpeed) {
        this.maxSpeed = nSpeed;
    }

    setMinAutoSpeed(nSpeed) {
        this.minSpeed = nSpeed;
    }

    setClockType(clockType) {
        this.clockType = clockType;
    }

    getClockType() {
        return this.clockType;
    }

}