const   PacketHeaderLen = 4;
const   RtpHeaderLen    = 12;  
    
export class H264RtpParser{
    constructor(){
    }

    splitHeader(src){
        if(src.length < RtpHeaderLen + 1){
            return [true,src.slice(0,RtpHeaderLen),src.slice(RtpHeaderLen)];
        }
        let rtpData = src.slice(RtpHeaderLen);
        let packetNALUnitType = (rtpData[0] & 0x1f);
        if(packetNALUnitType > 1 && packetNALUnitType <= 23){
            packetNALUnitType = 1
        }

        let skipHeaderLen = 0;
        let isCompletesFrame = false;
        switch (packetNALUnitType) {
            case 0:
            case 1: //undefined, but pass them through
            {
                skipHeaderLen = 0;
			    isCompletesFrame = true;
            }
            break;
            case 24: // STAP-A
            {
                skipHeaderLen = 1;
                isCompletesFrame = true;
            }
            break;
            case 25:
            case 26:
            case 27: // STAP-B MTAP16 MTAP24
            {
                skipHeaderLen = 3;
                isCompletesFrame = true;
            }
            break;
            case 28:
            case 29: // FU-A // FU-B
            {
                let startBit = rtpData[1] & 0x80;
                let endBit = rtpData[1] & 0x40;
                if (0 != startBit) {
                    // reset nal header
                    rtpData[1] = (rtpData[0] & 0xe0) | (rtpData[1] & 0x1f);
                    skipHeaderLen = 1;
                } else {
                    skipHeaderLen = 2;
                }
                isCompletesFrame = (endBit != 0);
            }
            break;
            default:
            {
                skipHeaderLen = 0;
                isCompletesFrame = true;
            }
        }
        return [isCompletesFrame, src.slice(0,skipHeaderLen+RtpHeaderLen), rtpData.slice(skipHeaderLen)];
    }

    parsingRtp(header,payload){
        if(header.length < RtpHeaderLen + 2){
            return [0, payload.length];
        }

        let naluHeader = header.slice(RtpHeaderLen);
        let packetNALUnitType = (naluHeader[0] & 0x7E) >> 1;

        let naluHeaderSize = 0;
        let naluSize = 0;
        switch (packetNALUnitType){
            case 24:
            case 25:
            {
                naluSize = parseInt((payload[0] << 8) | naluHeader[1]);
                naluHeaderSize = 2;
            }
            break;
            case 26:
            {
                naluSize = parseInt((naluHeader[3] << 8) | naluHeader[4]);
			    naluHeaderSize = 5;
            }
            break;
            case 27:
            {
                naluSize = parseInt((naluHeader[3] << 8) | naluHeader[4]);
			    naluHeaderSize = 6;
            }
            break;
            default:
            {
                naluSize = payload.length;
                naluHeaderSize = 0;
            }
        }
        return [naluHeaderSize,naluSize];
    }

}