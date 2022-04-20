importScripts("./libdemuxer.js","../resource/WebPacket.js","../resource/WebStream.js")

var readDate;
// var dataBuffer = new Array();
var demuxer;
var currentPts = 0;
Module.onRuntimeInitialized = async _ => {
    var data = new Object();
    data.type = 'loadfinished';
    postMessage(data);
}

function initdemuxer() {
    var pos = 0;
    var func = {
        SeekData: function (position, whence) {
            return position + whence;
        },
        ReadData: function (data) {
            if (pos >= readDate.byteLength) {
                console.log('Read Data Len:' + 0);
                return 0;
            }
            var subArray;
            if (readDate.byteLength >= pos + data.byteLength) {
                subArray = readDate.subarray(pos, pos + data.byteLength);
                pos = pos + data.byteLength;
            }
            else {
                subArray = readDate.subarray(pos, readDate.byteLength);
                pos = readDate.byteLength;
            }
            data.set(subArray, 0);
            var retLen = subArray.byteLength;
            console.log('Read Data Len:' + retLen);
            return retLen;
        }
        // ReadData: function (data) {
        //     if (dataBuffer.length <= 0) {
        //         console.log('Read Data Len:' + 0);
        //         return 0;
        //     }
        //     var subArray = dataBuffer.shift();
        //     data.set(subArray, 0);
        //     var retLen = subArray.byteLength;
        //     return retLen;
        // }
    };

    var interfaceObj = Module.IODevice.implement(func);

    demuxer = new Module.WebDemuxerFFmpeg();
    demuxer.AddRef();

    if (demuxer.initDemuxerIO(interfaceObj) != Module.IDemuxError.DemuxerNoError) {
        console.log('init demuxer error');
        return;
    }
    console.log('init demuxer success');

    currentPts = 0;
}

function demux() { 

    var packet = new Module.WebPacketFFmpeg();
    packet.AddRef();

    var error = demuxer.demuxStream(packet);
    var data = new Object;
    data.errorcode = error.value;
    data.type = 'demuxpakcetfinished';

    // reset pts
    // packet.setPtsMSec(currentPts);
    // packet.setDtsMSec(currentPts);
    // currentPts += 40;

    if (error == Module.IDemuxError.DemuxerNoError) {
        data.webPacket = new WebPacket(packet);
        postMessage(data,[data.webPacket.packetData.buffer]);
        data.webPacket.packetData = null;
    }
    else if (error == Module.IDemuxError.DemuxerDemuxerEof) {
        data.webPacket = new WebPacket(packet);
        postMessage(data,[data.webPacket.packetData.buffer]);
        data.webPacket.packetData = null;
    }
    else {
        postMessage(data);
    }
    
    packet.Release();

}

function getStreams(){
    var nStream = demuxer.getStreamCount();
    var webStreams = new Array(nStream);
    for(var i=0; i<nStream; ++i)
    {
        var stream = new Module.WebStreamFFmpeg;
        stream.AddRef();
        demuxer.getStream(i,stream);          
        webStreams[i] = new WebStream(stream);
    }
    return webStreams;
}

function controlDemux(data){
    var seekInfo = data.seekInfo;
    var scaleInfo = data.scaleInfo;

    if(seekInfo.needSeek)
    {
        if(demuxer.seek(seekInfo.nSeekSecond))
        {
            var data = new Object;
            data.type = 'seekfinished';
            postMessage(data);
        }
    }

    if(scaleInfo.needScale)
    {
        demuxer.setScale(scaleInfo.fScale);
        var data = new Object;
        data.type = 'setScalefinished';
        postMessage(data);
    }
}

onmessage = function (event) {
    if (event.data.type == 'load') {
        console.log('begin load media.');
    }
    else if (event.data.type == 'initdemuxer') {
        initdemuxer();
        
        var data = new Object;
        data.type = 'initdemuxerfinished';
        data.webStreams = getStreams();
        data.duration = demuxer.getDuration();

        postMessage(data);
    }
    else if (event.data.type == 'demuxpacket') {
        controlDemux(event.data);
        demux();
    }
    else if (event.data.type == 'setmediadata') {
        readDate = event.data.mediaData;
        // dataBuffer.push(event.data.mediaData);
    }
}