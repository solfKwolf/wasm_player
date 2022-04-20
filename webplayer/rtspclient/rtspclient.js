
import {SDPParser} from './sdp.js';
import {RtpParser} from './rtpparser.js';
import {RtspClientContext} from './rtsprequest.js';
import {RtspResponseContext} from './rtspparser.js';


const RtspEventStatus = {
    RtspEventNone : 0,
    RtspEventRequestSuccess : 1,
    RtspEventRequestError : 2,
    RtspEventDisconnected : 3
}

class RtspEvent{
    constructor(eventType, rtpMediaMap, data){
        this.eventType = eventType; 
        this.rtpMediaMap = rtpMediaMap;   
        this.data = data;      
    }
}

class RtspData{
    constructor(channelNum, data, curPtsMap){
        this.channelNum = channelNum;   
        this.data = data;  
        this.curPtsMap = curPtsMap;
    }        
}

class RtpPts{
    constructor(){
        this.lastTimestamp = 0;
        this.curPts = 0;
        this.clockBase = 0;
    }
}

export class RtspClientSession{
    constructor(rtpHandler,eventHandler){
        this.username = "";              
        this.password = "";              
        this.address = "";                 
        this.timeoutFlag = false;  
        this.timeoutSec = 5000;  
        this.isCloseConn = false;           

        this.dataHandle = rtpHandler;     //RtspData callback
        this.eventHandle = eventHandler;  // RtspEvent callback
        this.rtspResponseQueue = new Array();  
        this.rtspResponseContext = new RtspResponseContext(); 
        this.rtpChannelMap = new Map();         //RtpParser
        this.rtpMediaMap = new Map();           //MediaSubsession  map
        this.curPtsMap = new Map();

        this.sdp = null;
        this.rtspContext = new RtspClientContext();
        this.dataBuffer = new Array();        
    }

    sendEvent(eventType, data){
        if(this.eventHandle == null){
            console.log("nil event queue");
            return;
        }
        this.eventHandle(new RtspEvent(eventType, this.rtpMediaMap, data));
    }

    initBuffer(){
        this.rtspResponseQueue = [];
        this.dataBuffer = [];
        this.rtpChannelMap.clear();
        this.rtpMediaMap.clear();
        this.curPtsMap.clear();
    }

    play(webURL,rtspURL){
        if (!this.parsingURL(rtspURL)){
            console.log("url parse error: " + rtspURL);
        }
        this.initBuffer();
        this.rtspContext.sendConnect(webURL);
        this.rtspContext.webSocket.onopen = () => {this.onopen();}; 
        this.rtspContext.webSocket.onmessage = (evt) => {this.onmessage(evt);};               
        this.rtspContext.webSocket.onclose = () => {this.onclose();};
        this.rtspContext.webSocket.onerror = (evt) =>{this.onerror(evt);};
        return;
    }

    stop(){
        if (this.rtspContext.webSocket.readyState == WebSocket.OPEN){
            this.rtspContext.sendTeardown();
            this.initBuffer();
            // this.close();
        }
        else{
            console.log("websocket not connection...");
        }    
    }

    pause(){
        if (this.rtspContext.webSocket.readyState == WebSocket.OPEN){
            this.rtspContext.sendPause();
        }
        else{
            console.log("websocket not connection...");
        }  
    }

    resume(){
        if (this.rtspContext.webSocket.readyState == WebSocket.OPEN){
            this.rtspContext.sendPlay(0, 1);
        }
        else{
            console.log("websocket not connection...");
        } 
    }

    close(){
        if (this.rtspContext.webSocket.readyState != WebSocket.CLOSED){
            this.rtspContext.webSocket.close();
        }
    }

    async waitRtspResponse(){
        return await new Promise((resolve, reject) =>{
            let response = null;
            let timer;
                const task = () =>{
                    response = this.rtspResponseQueue.shift();
                    if(response != null){
                        clearInterval(timer);
                        if(response instanceof Error){
                            reject(response);
                        } 
                        resolve(response);
                    }    
                }   
                timer = setInterval(task,200);
        })
    }

    parsingURL(rtspURL){
        var uri = new URI(rtspURL)
        if(!uri.is("url")){
            console.log("rtsp url errpr");
            return false
        }
        var urlInfo = URI.parse(rtspURL);
        if (null != urlInfo.hostname && null != urlInfo.port){
            this.address = urlInfo.hostname + ":" + urlInfo.port;
        }     

        if (null != urlInfo.username && null != urlInfo.password){
            this.username = urlInfo.username;
            this.password = urlInfo.password;
        }
        let query = "";
        if(urlInfo.query != null){
            query = "?" + urlInfo.query;
        }
        let path = "";
        if(urlInfo.path != "/"){
            path = urlInfo.path;
        }
        this.rtspContext.rtspURL = urlInfo.protocol + "://" + this.address + path + query;
        return true
    }

    async sendRequest(){
        return await new Promise(async (resolve, reject) =>{
            this.rtspContext.authenicator = null;
    
            this.rtspContext.sendDescribe();  
            let response = await this.waitRtspResponse()
            if(response.status == 401)
            {
                let errorInfo = this.setAuth(response);
                if (errorInfo instanceof Error) {
                    return reject(errorInfo);
                }
                this.rtspContext.sendDescribe();  
                response = await this.waitRtspResponse()
            }
            if (response instanceof Error) {
                return reject(response);
            }
            if (response.status != 200) {
                return reject(new Error("Describe Response Error"));
            }
            let setupInfoArr = this.parseSDP(response);
            if (setupInfoArr instanceof Error) {
                return reject(setupInfoArr);
            }

            for(let i=0; i<setupInfoArr.length; i++){
                let trackURL = setupInfoArr[i].trackURL;
                let rtpIndex = setupInfoArr[i].rtpIndex;
                let rtcpIndex = setupInfoArr[i].rtcpIndex;
                this.rtspContext.sendTcpSetup(trackURL, rtpIndex, rtcpIndex);
                response = await this.waitRtspResponse();
                if(response.sessionID != "")
                {
                    this.rtspContext.sessionID = response.sessionID;
                }
                if (response.status != 200) {
                    return reject(new Error("Setup Response Error"));
                }
            }

            resolve()
            this.rtspContext.sendPlay(0, 1);
            response = await this.waitRtspResponse();
            if (response.status != 200) {
                return reject(new Error("Setup Response Error"));
            }
            return null
        })   
    }

    setAuth(response){
        if(response instanceof Error || response == null){
            return new Error("response error");
        }
        if (this.username == "" || this.password == ""){
            return new Error("no auth info");
        }
        if (null != response.digestAuthenticator){
            this.rtspContext.authenicator = response.digestAuthenticator;
        }
        else if (null != response.basicAuthenticator){
            this.rtspContext.authenicator = response.basicAuthenticator;
        }
        if (null != this.rtspContext.authenicator){
            this.rtspContext.authenicator.username = this.username;
            this.rtspContext.authenicator.password = this.password;
        }
        return null
    }

    parseSDP(response){
        if (200 != response.status){
            return new Error("response error: " + response.status);
        }

        this.sdp = new SDPParser();
        this.sdp.parse(response.content).catch(()=>{
            throw new Error("Failed to parse SDP");
        });
        this.rtspContext.sessionID = "";

        let setupInfoArr = new Array(); 
        let mediaList = this.sdp.getMediaBlockList();

        for (let i = 0; i < mediaList.length; i++){
            let mediaType = mediaList[i];
            let media = this.sdp.media[mediaType];
            let strTrackURL = media.control;
            if(strTrackURL.length < 4 || ("rtsp" != strTrackURL.slice(0,4) && "RTSP" != strTrackURL.slice(0,4))){
                strTrackURL = this.rtspContext.rtspURL + "/" + strTrackURL;
            }
            let rtpIndex = i * 2;
            let rtcpIndex = i * 2 + 1;
            let payload = media.fmt;
            let codecName = media.rtpmap[payload].name;
   
            this.rtpChannelMap.set(rtpIndex,new RtpParser(codecName));
            this.rtpMediaMap.set(rtpIndex, media);

            let rtpPts = new RtpPts();
            rtpPts.clockBase = media.rtpmap[payload].clock;
            this.curPtsMap.set(rtpIndex, rtpPts);

            let setupInfo = new Object();
            setupInfo.trackURL = strTrackURL;
            setupInfo.rtpIndex = rtpIndex;
            setupInfo.rtcpIndex = rtcpIndex;

            setupInfoArr.push(setupInfo);      
        }
        return setupInfoArr;
    }

    parsingData(data){
        let unit8Arr = new Uint8Array(data) ;
        let encodedString = String.fromCharCode.apply(null, unit8Arr);
        
        let headerData = encodedString.slice(0,4);
        if (headerData[0] == "$"){   
            this.parsingRtp(unit8Arr);
        }
        else if (headerData.slice(0,4) == "RTSP"|| headerData.slice(0,4) == "rtsp"){
            let decodedString = decodeURIComponent(escape((encodedString)));//没有这一步中文会乱码
            console.log(decodedString);
            this.parsingRtsp(decodedString);
        }
        return;
    }

    parsingRtp(data){
        this.timeoutFlag = true;
        let header = data.slice(0,4);
        let rtpData = data.slice(4);

        let timestampBufBefore  = rtpData.slice(4,8);
        let buffer = timestampBufBefore.buffer; 
        let view = new DataView(buffer);
        let timestamp  = view.getUint32(0);

        let channelNum = parseInt(header[1]);
        let curPts = 0;
        for (let [key, rtpPts] of this.curPtsMap){
            if(key == channelNum){
                if(rtpPts.lastTimestamp != timestamp){
                    if(rtpPts.lastTimestamp != 0){
                        curPts =  rtpPts.curPts + ((timestamp - rtpPts.lastTimestamp)*1000/rtpPts.clockBase);
                        rtpPts.curPts = curPts;
                    }
                    rtpPts.lastTimestamp = timestamp;
                }
            }
        }

        let rtpParser = this.rtpChannelMap.get(channelNum);
        if (null == rtpParser){
            return ;
        }
        let rtpInfo =  rtpParser.splitRtpPacket(rtpData);
        header = rtpInfo[0];
        let payload = rtpInfo[1];
        let totalLength = 0;
        while(totalLength < payload.length){
            let result = rtpParser.pushData(header,payload.slice(totalLength));
            let nalu = result[0];
            let completionLength = result[1];
            if(null != nalu){
                this.dataHandle(new RtspData(channelNum, nalu, this.curPtsMap));
            }
            totalLength += completionLength;
        }
    }

    parsingRtsp(rtspData){
        let response = this.rtspResponseContext.parserRtspResponse(rtspData);
        if (response instanceof Error){
            console.log("parserRtspResponse error");
            let error = new Error("parserRtspResponse error")
            this.rtspResponseQueue.push(error);
            return;
        }
        this.rtspResponseQueue.push(response); 
        return ;
    }
 
    getHeaderLength(){
        return 4;
    }

    unserializeHeader(header){
        let unit8Arr = new Uint8Array(header) ;
        let headerStr = String.fromCharCode.apply(null, unit8Arr);
   
        if(headerStr[0] == '$'){
            let lenBufBefore = unit8Arr.slice(2,4);
            let len = 0;
            let buffer = lenBufBefore.buffer;
            
            let view = new DataView(buffer);
            len = view.getUint16(0);
            return [len, null];
        }
        else if ((headerStr.slice(0,4) == "RTSP"|| headerStr.slice(0,4) == "rtsp")){
            return [0, "\r\n\r\n"];
        }
        else{
            console.log("header: " + headerStr[0]);
            return [0, null];
        }
    }

    getContentLength(body){
        let unit8Arr = new Uint8Array(body) ;
        let bodyStr = String.fromCharCode.apply(null, unit8Arr);

        if(bodyStr.slice(0,1) == "$"){
            return 0;
        }
        bodyStr = decodeURIComponent(escape((bodyStr)));//没有这一步中文会乱码
        let response = this.rtspResponseContext.parserRtspResponse(bodyStr);
        if(response instanceof Error){
            return 0;
        }   
        return response.contentLength;
    }

    unpack(buf){
        let bufStartPos = 0;
        let headerLength = this.getHeaderLength();
        let bodyLength = 0;
        let contextLength = 0;

        let headBuf = buf.slice(bufStartPos, headerLength);
        if (headBuf.length < 4){
            return new Array();
        }
        bufStartPos += headerLength;

        let msg = null;
        let result = this.unserializeHeader(headBuf);
        let packetLength = result[0];
        let strEOF = result[1];
        if(null != strEOF){
            let curBodyPos = 0;
            for(let i=0; i<buf.length; i++){
                curBodyPos ++;
                if(curBodyPos < strEOF.length){
                    continue;
                }
                let eof = buf.slice(bufStartPos + curBodyPos - strEOF.length, bufStartPos + curBodyPos);
                let unit8Arr = new Uint8Array(eof) ;
                let eofStr = String.fromCharCode.apply(null, unit8Arr);
                if(strEOF == eofStr){
                    break;
                }
            }
            bodyLength = curBodyPos;
        }
        else if(0 != packetLength){
            bodyLength = packetLength;
        }
        else{
            console.log("other data...", buf.byteLength);
            return new Error("The stream data is error"); 
        }
        bufStartPos += bodyLength;
        contextLength = this.getContentLength(buf.slice(0,bufStartPos));
        bufStartPos += contextLength;
  
        if(bufStartPos > buf.length)
        {     
            msg = new Array();
        }
        else{
            msg = buf.slice(0,bufStartPos);
        }
        return msg;
    }

    onopen(){
        console.log("connect success");
        this.setTimeoutFlag();
        this.sendRequest().then((errorInfo)=>{
            if(errorInfo instanceof Error)
            {
                console.log(errorInfo)
            }
            this.sendEvent(RtspEventStatus.RtspEventRequestSuccess, null)
        });  
    }

    onmessage(event){
        if (event.data != null){  
            let dataArr = Array.prototype.slice.call(new Uint8Array(event.data)); //ArrayBuffer to Array
            this.dataBuffer = this.dataBuffer.concat(dataArr)
            let len = this.dataBuffer.length;
            while(len > 0)  { 
                let packet = this.unpack(this.dataBuffer);
                if (packet instanceof Error){
                    len = 0;
                    this.dataBuffer = [];
                    break;
                }       
                for(let i=0; i< packet.length; i++){
                    this.dataBuffer.shift();
                }
                len = this.dataBuffer.length;
                if(packet.length > 0){
                    this.parsingData(packet);  
                } 
                if(packet.length == 0){
                    len = 0;
                    break;
                }
            }      
        }
    }

    onclose(){
        console.log("connect close"); 
        this.isCloseConn = true;
        this.sendEvent(RtspEventStatus.RtspEventDisconnected, null);
    }

    onerror(event){
        console.log("connect error: " + event); 
        this.sendEvent(RtspEventStatus.RtspEventRequestError, null)
    }

    setTimeoutFlag(){
        return new Promise((resolve, reject) =>{
            let timer = null;
            const task = ()=>{
                if(!this.timeoutFlag && !this.isCloseConn)
                {
                    this.stop();
                    clearInterval(timer);
                    reject(new Error("receive rtp data timeout"));
                }
                else {
                    this.timeoutFlag = false;
                } 
            }
            timer = setInterval(task,this.timeoutSec);
        })
    }
}
