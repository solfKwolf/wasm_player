const   PacketHeaderLen = 4;
const   RtpHeaderLen    = 12;  

export class HevcRtpParser{
    constructor(){

    }

    splitHeader(src){
        if(src.length < RtpHeaderLen + 2){
            return [true,src.slice(0,RtpHeaderLen),src.slice(RtpHeaderLen)];
        }
        let rtpData = src.slice(RtpHeaderLen);
        let packetNALUnitType = (rtpData[0] & 0x7E) >> 1;

        let skipHeaderLen = 0;
        let isCompletesFrame = false;
        switch (packetNALUnitType){
            case 48:// Aggregation Packet (AP)
                // We skip over the 2-byte Payload Header, and the DONL header (if any).
                skipHeaderLen = 2;
                isCompletesFrame = true;
                break;
            case 49:// Fragmentation Unit (FU)
                // This NALU begins with the 2-byte Payload Header, the 1-byte FU header, and (optionally)
                // the 2-byte DONL header.
                // If the start bit is set, we reconstruct the original NAL header at the end of these
                // 3 (or 5) bytes, and skip over the first 1 (or 3) bytes.
                let startBit = rtpData[2] & 0x80
                let endBit = rtpData[2] & 0x40
                if (0 != startBit) {
                    let nalUnitType = rtpData[2] & 0x3F;
                    let newNalHeader = new ArrayBuffer(2);
                    newNalHeader[0] = (rtpData[0] & 0x81) | (nalUnitType << 1);
                    newNalHeader[1] = rtpData[1];

                    rtpData[1] = newNalHeader[0];
                    rtpData[2] = newNalHeader[1];

                    skipHeaderLen = 1;
                } else {
                    skipHeaderLen = 3;
                }
                isCompletesFrame = (endBit != 0);
                break;
            default:
                isCompletesFrame = true;
                skipHeaderLen = 0;
        }
        return [isCompletesFrame, src.slice(0,skipHeaderLen + RtpHeaderLen) , rtpData.slice(skipHeaderLen) ];
    }

    parsingRtp(header,payload){
        if(header.length < RtpHeaderLen + 2){
            return [0, payload.length];
        }
        let naluHeader = header.slice(RtpHeaderLen);
        let packetNALUnitType = (naluHeader[0] & 0x7E) >> 1;

        let naluSize = 0;
        let naluHeaderSize = 0;

        switch (packetNALUnitType){
            case 48:
            {
                // We skip over the 2-byte Payload Header, and the DONL header (if any).
                naluSize = parseInt((payload[0] << 8) | payload[1]);
                naluHeaderSize = 2;
            }
            break;
            default:
            {
                naluSize = payload.length;
                naluHeaderSize = 0;
            }
        }
        return [naluHeaderSize, naluSize + naluHeaderSize];
    }

}