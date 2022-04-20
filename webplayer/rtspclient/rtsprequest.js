import {Authenticator} from './authenticator.js';

export class RtspClientContext{
    constructor(){
        this.rtspURL = "";
        this.userAgent = "None";
        this.controlID = "trackID";
        this.sessionID = "";
        this.setupHeaders = "";
        this.cseq = 0;
        this.bandwidth = 0;
        this.authenicator = new Authenticator();
        this.webSocket = null;
    }

    sendConnect(webURL){
        this.webSocket = new WebSocket(webURL);
        this.webSocket.binaryType = "arraybuffer";
    }

    sendRequest(request){
        if (this.webSocket.readyState === WebSocket.OPEN){
            console.log(request);
            this.webSocket.send(request); 
            this.cseq++;
        }
    }

    sendDescribe(){
        // if (!session.rtspRequestInitial){
        //     return new Error("waiting last request Reply")
        // }

        let request = "DESCRIBE " + this.rtspURL + " RTSP/1.0\r\n" +
		"CSeq:" + this.cseq +"\r\n" +
		"Accept: application/sdp\r\n" +
        "User-agent: " + this.userAgent + "\r\n";
        
        if (0 != this.bandwidth){
            request += "Bandwidth:" + this.bandwidth + "\r\n";
        }

        if (null != this.authenicator){
            request += this.authenicator.createAuthenticatorString("DESCRIBE", this.rtspURL)
        }

        request += "\r\n";
        this.sendRequest(request);
        return ;
    }

    sendTcpSetup(inTrackURL, inClientRTPid, inClientRTCPid){ 
        let request = "SETUP " + inTrackURL + " RTSP/1.0\r\n" +
		"CSeq: " + this.cseq + "\r\n" +
		"Session: " + this.sessionID + "\r\n" +
		"Transport: RTP/AVP/TCP;unicast;interleaved=" + inClientRTPid + "-" + inClientRTCPid + "\r\n" +
		this.setupHeaders +
        "User-agent: " + this.userAgent + "\r\n";
        
        if (0 != this.bandwidth) {
            request += "Bandwidth: " + this.bandwidth + "\r\n";
        }

        if (null != this.authenicator){
            request += this.authenicator.createAuthenticatorString("SETUP",this.rtspURL);
        }
        request += "\r\n";
        this.sendRequest(request);
    }

    sendPlay(inStartTimeSec, inSpeed){
        let strSpeed = "";
        if(inSpeed != 1){
            strSpeed = "Speed: " + parseFloat(inSpeed) + "\r\n";
        }
        let strStartTime = "";
        if(inStartTimeSec != 0){
            strSpeed = "Range: npt=" + parseFloat(inStartTimeSec) + ".0-\r\n";
        }

        let request = "PLAY " + this.rtspURL + " RTSP/1.0\r\n" +
		"CSeq: " + this.cseq + "\r\n" +
		"Session: " + this.sessionID + "\r\n" +
		strStartTime +
		strSpeed +
		"x-prebuffer: maxtime=3.0\r\n" +
        "User-agent: " + this.userAgent + "\r\n";
        
        if (0 != this.bandwidth) {
            request += "Bandwidth: " + this.bandwidth + "\r\n";
        }

        if (null != this.authenicator){
            request += this.authenicator.createAuthenticatorString("PLAY",this.rtspURL);
        }

        request += "\r\n";
        this.sendRequest(request);
    }

    sendPause(){
        let request = "PAUSE " + this.rtspURL + " RTSP/1.0\r\n" +
		"CSeq: " + this.cseq + "\r\n" +
		"Session: " + this.sessionID + "\r\n" +
        "User-agent: " + this.userAgent + "\r\n";
        
        request += "\r\n";
        this.sendRequest(request);
    }

    sendTeardown(){
        let request = "TEARDOWN " + this.rtspURL + " RTSP/1.0\r\n" +
		"CSeq: " + this.cseq + "\r\n" +
        "User-agent: " + this.userAgent + "\r\n";
        
        if(null != this.authenicator){
            request += this.authenicator.createAuthenticatorString("TEARDOWN", this.rtspURL);
        }

        request += "\r\n";
        this.sendRequest(request);
    }
}
