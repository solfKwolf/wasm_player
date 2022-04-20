import {Authenticator} from './authenticator.js';

const HeaderInfo = {
    sSessionHeader      : "Session",
	sContentLenHeader   : "Content-length",
	sTransportHeader    : "Transport",
	sRTPInfoHeader      : "RTP-Info",
	sRTPMetaInfoHeader  : "x-RTP-Meta-Info",
	sAuthenticateHeader : "WWW-Authenticate",
	sSameAsLastHeader   : " ,",
	sPublic             : "Public"
}

export class RtspResponseContext{
    constructor(){
        this.status = 0;          
        this.contentLength = 0;       
        this.content = "";             
        this.sessionID = "";          
        this.basicAuthenticator = null;
        this.digestAuthenticator = null;
    }

    parserRtspResponse(response){
        if (response.slice(0,4) != "RTSP" && response.slice(0,4) != "rtsp" ){
            console.log("not rtsp packet")
            return new Error("not rtsp packet");
        }
        this.status = 0;
        this.contentLength = 0;
        this.content = "";

        let endString = "\r\n\r\n";
        let rtspEndPos = response.indexOf(endString)
        if (rtspEndPos == -1){
            console.log("no eof flag")
            return new Error("no eof flag");
        }
        rtspEndPos += endString.length;

        let rtspResponse = response.slice(0,rtspEndPos)

        let tempString = "RTSP/1.0 ";
        let len = tempString.length;
        let strStatus = response.slice(len,len + 3)
        this.status = parseInt(strStatus);

        let fields = rtspResponse.split("\r\n");

        for( let value of fields){
            let keyValue = value.split(":");
            if(2 != keyValue.length){
                continue;
            }
            let theKey = keyValue[0].toUpperCase();
            theKey = theKey.replace(" ", "");
            let theValue = keyValue[1];
            if (theKey == HeaderInfo.sContentLenHeader.toUpperCase()){
                theValue = theValue.replace(" ", "");
                this.contentLength = parseInt(theValue);
                if (response.length - rtspEndPos >= this.contentLength){
                    this.content = response.slice(rtspEndPos,rtspEndPos + this.contentLength);
                }
            }
            else if(theKey == HeaderInfo.sSessionHeader.toUpperCase()){
                    this.sessionID = theValue;
            }
            else if(theKey == HeaderInfo.sAuthenticateHeader.toUpperCase()){
                    let authenticate = this.parsingAuthenticate(theValue);
                    if("Digest" == authenticate.authenticateType){
                        this.digestAuthenticator = new Authenticator();
                        this.digestAuthenticator.realm = authenticate.realm;
                        this.digestAuthenticator.nonce = authenticate.nonce;
                        this.digestAuthenticator.authenicatorType = 1;
                    }
                    else if("Basic" == authenticate.authenticateType){
                        this.basicAuthenticator = new Authenticator();
                        this.basicAuthenticator.realm = authenticate.realm;
                        this.basicAuthenticator.authenicatorType = 2;
                    }
            }       
        }
        return this;
    }

    parsingAuthenticate(authenticateValue){
        let authenticate = {
            realm : "",
            nonce : "",
            stale : "",
            authenticateType : "",
        }

        let pos = 0;
        let fields = authenticateValue.split(" ");
        for (let value of fields){
            let keyValue = value.split("=");
            if(2 != keyValue.length){
                if(value == "Digest" || value == "Basic"){
                    authenticate.authenticateType = value;
                    pos = authenticateValue.indexOf(value);
                    pos += value.length;
                    break;
                }
            }
        }

        let authenticateStr = authenticateValue.slice(pos);
        authenticateStr = authenticateStr.replace(/[\'\"\\\/\b\f\n\r\t]/g, '');
        fields = authenticateStr.split(",");
        for(let value of fields){
            let keyValue = value.split("=");
            let theKey = keyValue[0];
            let theValue = keyValue[1];
            
            if(theKey == " realm"){
                authenticate.realm = theValue;
            }
            else if(theKey == " nonce"){
                authenticate.nonce = theValue;
            }
            else if(theKey == " stale"){
                authenticate.stale = theValue;   
            }
        }
        return authenticate;
    }
}