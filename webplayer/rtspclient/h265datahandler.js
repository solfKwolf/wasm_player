export class H265DataHandle{
    constructor(){
        this.byteVps = new Array();
        this.byteSps = new Array();
        this.bytePps = new Array();
        this.gotIDR = true;
        this.isFirstPacket = true;
    }
    setMediaSubsession(media){
        if(media.fmtp == null){
            return 
        }
        let vpsInfo = media.fmtp["sprop-vps"];
        let spsInfo = media.fmtp["sprop-sps"];
        let ppsInfo = media.fmtp["sprop-pps"];
        this.gotIDR = false;

        if(vpsInfo != null){
            let strVps = Base64.decode(vpsInfo);
            for (let i = 0, j = strVps.length; i < j; ++i) {
                this.byteVps.push(strVps.charCodeAt(i));
            }
        }
        if(spsInfo != null){
            let strSps = Base64.decode(spsInfo); 
            for (let i = 0, j = strSps.length; i < j; ++i) {
                this.byteSps.push(strSps.charCodeAt(i));
            }
        }
        if(ppsInfo != null){
            let strPps = Base64.decode(ppsInfo); 
            for (let i = 0, j = strPps.length; i < j; ++i) {
                this.bytePps.push(strPps.charCodeAt(i));
            }
        }
    }

    getHeader(){
        if(this.byteVps.length == 0){
            return new Array();
        }
        let headerByte = new Array(0x00, 0x00, 0x00, 0x01);
        let header = new Array();
        header = header.concat(headerByte);
        header = header.concat(this.byteVps);
        header = header.concat(headerByte);
        header = header.concat(this.byteSps);
        header = header.concat(headerByte);
        header = header.concat(this.bytePps);  
        return header;
    }

    isIDRFrame(data){
        let code = -1;
        code = (code << 8) + parseInt(data[0]);
        let naluType = (code & 0x7e) >> 1;

        let isIDR = false;
        if(naluType >= 16 && naluType <= 21){
            isIDR = true
        }
        else
        {
            this.gotIDR = false;
        }
        let header = isIDR && !this.gotIDR;
        this.gotIDR = isIDR || this.gotIDR;
        return header;
    }

    isEnableFrame(data){
        let code = -1;
        code = (code << 8) + parseInt(data[0]);
        let naluType = (code & 0x7e) >> 1;

        let isEnable = false;
        if((naluType >= 16 && naluType <= 21) || (naluType >= 32 && naluType <= 40)){
            isEnable = true
        }
        else
        {
            this.gotIDR = false;
        }
        let header = isEnable && !this.gotIDR;
        this.gotIDR = isEnable || this.gotIDR;
        return header;
    }

    parsingData(src){
        let headerByte = new Array(0x00, 0x00, 0x00, 0x01);
        let dst = new Array();
        if(this.isFirstPacket && !this.isEnableFrame(src)) {
            return dst
        }
        this.isFirstPacket = false
        // if(this.addHeader(src) && this.byteVps.length !=0 ){
        //     dst = dst.concat(this.getHeader());
        // }
        dst = dst.concat(headerByte);
        dst = dst.concat(src);

        return dst;
    }
}