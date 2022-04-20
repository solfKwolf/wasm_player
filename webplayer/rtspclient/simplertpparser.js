const   PacketHeaderLen = 4;
const   RtpHeaderLen    = 12;  

export class SimpleRtpParser{
    constructor(){

    }

    splitHeader(src){
        if (src.length < 2) {
            return [0, src.slice(0,RtpHeaderLen), src.slice(RtpHeaderLen)];
        }
    
        return [true, src.slice(0,RtpHeaderLen), src.slice(RtpHeaderLen)];
    }

    parsingRtp(header,payload){
        return [0, payload.length];
    }

}