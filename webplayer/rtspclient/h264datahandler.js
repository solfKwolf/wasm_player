export class H264DataHandle{
    constructor(){
        this.byteSps = new Array();
        this.bytePps = new Array();
    }
    setMediaSubsession(media){
        let spsInfo = media.fmtp["sprop-parameter-sets"];
        if ("" == spsInfo){
            return;
        }
        let spsPps = spsInfo.split(",");
        let sps = "";
        let pps = "";
        if(spsPps.length == 1){
            sps = spsPps[0];
        }
        else if (spsPps.length == 2){
            sps = spsPps[0];
            pps = spsPps[1];
        }
        let strSps = Base64.decode(sps);
        let strPps = Base64.decode(pps);

        for (let i = 0, j = strSps.length; i < j; ++i) {
            this.byteSps.push(strSps.charCodeAt(i));
        }
        for (let i = 0, j = strPps.length; i < j; ++i) {
            this.bytePps.push(strPps.charCodeAt(i));
        }
    }

    getHeader(){
        if(this.byteSps.length == 0){
            return null;
        }
        let headerByte = new Array(0x00, 0x00, 0x00, 0x01);
        let header = new Array();
        header = header.concat(headerByte);
        header = header.concat(this.byteSps);
        header = header.concat(headerByte);
        header = header.concat(this.bytePps);
        return header;
    }

    parsingData(src){
        let headerByte = new Array(0x00, 0x00, 0x00, 0x01);
        headerByte = headerByte.concat(src);
        return headerByte;
    }
}