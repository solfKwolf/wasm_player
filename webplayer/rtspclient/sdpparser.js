

class MediaSubsession{
    constructor(){
        this.MediumName = "";
        this.PayloadFormat = 0;
        this.CodecName = "";
        this.RtpTimestampFrequency = 0;
        this.Channels = 0;
        this.TrackURL = "";
        this.VideoFramerate = 0;
        this.VideoWidth = 0;
        this.VideoHeight = 0;
        this.Fmtp = new Map();
    }
}

class SDPInfo{
    constructor(){
        this.medias = new Array();
    }

    parsingSDP(sdp){
        let sdpMessage = this.parsingSDPLine(sdp);
    }

    parsingSDPLine(strSdp){
        let sdpSession;
        let error;

    }
}