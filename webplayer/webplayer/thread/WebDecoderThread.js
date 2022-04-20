importScripts("./libdecoder.js","../resource/WebStream.js","../resource/WebPacket.js","../resource/WebFrame.js")

var decoder;
Module.onRuntimeInitialized = async _ => {
    var data = new Object();
    data.type = 'loadfinished';
    postMessage(data);
}

onmessage = function (event) {
    if (event.data.type == 'load') {
        console.log('begin load decoder.');
    }
    else if (event.data.type == 'initdecoder') {
        var errorCode = initDecoder(event.data.stream);

        var data = new Object;
        data.type = 'initdecoderfinished';
        data.errorcode = errorCode;
        postMessage(data);
    }
    else if (event.data.type == 'decodepacket') {
        decode(event.data.packet);
    }

}

function initDecoder(stream){
    decoder = new Module.WebDecoderFFmpeg();
    decoder.AddRef();
    var streamFFmpeg = new Module.WebStreamFFmpeg();
    streamFFmpeg.AddRef();
    stream.__proto__ =  WebStream.prototype;
    stream.convertToStreamFFmpeg(streamFFmpeg);

    var errorCode = decoder.initDecoder(streamFFmpeg)

    if (errorCode != Module.IDecodeError.NoError) {
        console.log('init decoder error');
    }
    streamFFmpeg.Release();
    return errorCode;
}

function decode(packet){
    packet.__proto__ =  WebPacket.prototype;
    var packetFFmpeg = new Module.WebPacketFFmpeg();
    packetFFmpeg.AddRef();
    packet.convertToPacketFFmpeg(packetFFmpeg);
    packet.packetData = null;

    var frameFFmpeg = new Module.WebFrameFFmpeg();
    frameFFmpeg.AddRef();
    var error = decoder.decodeData(packetFFmpeg,frameFFmpeg);
    var data = new Object;
    data.errorcode = error.value;
    data.type = 'decodepacketfinished';
    data.mediaType = packet.mediaType.value;

    if(error == Module.IDecodeError.NoError)
    {
        data.webFrame = new WebFrame(frameFFmpeg);
        postMessage(data,[data.webFrame.frameData.buffer]);
        data.webFrame = null;
    }
    else if(error == Module.IDecodeError.DecodeEof)
    {
        console.log('decodeData eof');
        postMessage(data);
    }
    else if(error == Module.IDecodeError.DecodeError)
    {
        console.log('decodeData error');
        postMessage(data);
    }    
    else if(error == Module.IDecodeError.FrameUseless)
    {
        postMessage(data);
    }  
    packetFFmpeg.Release();
    frameFFmpeg.Release();
}
