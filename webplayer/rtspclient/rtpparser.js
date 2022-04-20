
import {H264RtpParser} from './h264rtpparser.js';
import {HevcRtpParser} from './hevcrtpparser.js';
import {MarkRtpParser} from './markrtpparser.js';
import {SimpleRtpParser} from './simplertpparser.js';

const MaxPayloadLength = 8 * 1024 * 1024;

const PacketHeaderLen = 4;
const RtpHeaderLen = 12;

export class RtpParser {
    constructor(codecName){
        this.payloadBuf = new Array();
        this.payloadLen = 0;
        this.isMarkFrame = false;
        this.codecName = codecName;
        // this.rtpSourceHandler =(codecName) => {this.getRTPSourceHandler(codecName);};
        this.rtpParse = this.getRTPSourceHandler(this.codecName);
    }

    splitRtpPacket(src){
        let header, payload;
        let result = this.rtpParse.splitHeader(src);
        this.isMarkFrame = result[0];
        header = result[1];
        payload = result[2];
        return [header, payload];
    }

    pushData(header, payload){
        let naluInfo = this.rtpParse.parsingRtp(header, payload);
        let naluHeaderSize = naluInfo[0];
        let naluSize = naluInfo[1];
        if(naluSize > payload.length - naluHeaderSize){
            naluSize = payload.length;
        }
        let nalu = payload.slice(naluHeaderSize,naluSize);

        let naluArr = Array.prototype.slice.call(nalu); //uint8array to Array

        this.payloadBuf = this.payloadBuf.concat(naluArr);

        if (this.isMarkFrame){
            let data = this.payloadBuf;
            this.payloadBuf = new Array();
            return [data, naluSize];
        }
        return [null, naluSize];
    }

    getRTPSourceHandler(codecName){
        switch (codecName) {
            case "H264":
                {
                    return new H264RtpParser();
                }
            case "H265":
            case "HEVC":
                {
                    return new HevcRtpParser();
                }
            case "bbw":
                {
                    return new MarkRtpParser();
                }
            default:
                {
                    return new SimpleRtpParser();
                }
            }
    }

}