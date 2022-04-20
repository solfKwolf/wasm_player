export class MasterClock{
    constructor() {
        this.lSpeed = 1.0;
        this.avPts = 0;
        this.isFirstFrame = true;
        this.isPause = false;
        this.begin;
    }

    seekClock(nMSecond){
        this.avPts = nMSecond;
        this.begin = (new Date()).getTime();
    }

    reset(){
        this.seekClock(0);
    }

    getClock(){
        return ((new Date()).getTime() - this.begin) * this.lSpeed + this.avPts;
    }

    pauseClock(){
        this.avPts = this.getClock();
        this.isPause = true;
    }

    setFirstFrame(isFirstFrame){
        this.isFirstFrame = isFirstFrame;
    }

    IsFirstFrame(){
        return this.isFirstFrame;
    } 

    setSpeed(fSpeed){
        this.lSpeed = fSpeed;
    }

    getSpeed(){
        return this.lSpeed;
    }

}