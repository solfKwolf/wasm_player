const   PacketHeaderLen = 4;
const   RtpHeaderLen    = 12;  

export class MarkRtpParser{
    constructor(){

    }

    splitHeader(src){
        if (src.length < 2){
            return [true, src.slice(0,RtpHeaderLen), src.slice(RtpHeaderLen) ];
        }
        let mark = ((src[1] & 0x80) >> 7);
        let isCompletesFrame = (mark == 1);

        return [isCompletesFrame, src.slice(0, RtpHeaderLen), src.slice(RtpHeaderLen) ];
    }

    parsingRtp(header,payload){
        return [0, payload.length];
    }

}