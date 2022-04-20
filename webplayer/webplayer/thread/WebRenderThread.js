
importScripts("./libscale.js","../resource/WebStream.js","../resource/WebFrame.js")

var swscale;
var outbuffer = null;
var bufferSize = 0;
Module.onRuntimeInitialized = async _ => {
    var data = new Object();
    data.type = 'loadfinished';
    swscale = new Module.WebSwscalerFFmpeg();
    swscale.AddRef();
    postMessage(data);
}

onmessage = function (event) {
    if (event.data.type == 'load') {
        console.log('begin load render.');
    }
    else if(event.data.type == "renderframe"){
        render(event.data);
    }
}

async function render(frameData){
    var frame = frameData.frame;
    var clock = frameData.clock;

    var data = new Object;
    data.type = "renderframefinished";
    data.isDiscard = frameData.isDiscard;
    if(frameData.isDiscard) {
        postMessage(data);
        return;
    }

    var frameFFmpeg = new Module.WebFrameFFmpeg();
    frameFFmpeg.AddRef();

    frame.__proto__ =  WebFrame.prototype;
    frame.convertToFrameFFmpeg(frameFFmpeg);

    var frameType = frameFFmpeg.getFrameType();
    var framePts = frameFFmpeg.getPtsMsec();
    if(frameType == Module.FrameAVMediaType.AVTypeVideo)
    {
        var width = frameFFmpeg.frameWidth();
        var height = frameFFmpeg.frameHeight();
        var size = 0;
        if(frameFFmpeg.getFrameFormat() != Module.IFrameFrameFormat.FormatYUV420P)
        {
            size = swscale.getPictureSize(Module.IScaleFrameFormat.FormatYUV420P,width,height);
            if(!outbuffer || size != bufferSize)
            {
                Module._free(outbuffer);
                outbuffer = Module._malloc(size);
            }
            bufferSize = size;
            var error = swscale.convertImgToWebBuffer(frameFFmpeg,outbuffer,Module.IScaleFrameFormat.FormatYUV420P,width,height);
            if(error != Module.IScaleError.NoError)
            {
                console.log("convertImgToWebBuffer error.");
            }
        }   
    }
    else if(frameType == Module.FrameAVMediaType.AVTypeAudio)
    {
        if(frameFFmpeg.getAudioSampleFormat() != Module.IFrameAudioSampleFormat.AudioFormateS16 ||
            frameFFmpeg.getAduioChannels() != 2)
        {
            var error = swscale.converAudio(frameFFmpeg, 2, frameFFmpeg.getSampleRate(), Module.IFrameAudioSampleFormat.AudioFormateS16);  
            if(error != Module.IScaleError.NoError)
            {
                console.log("converAudio error");
            }
        }
    }

    if(framePts > clock)
    {
        await sleep(framePts - clock);
    }

    data.webFrame = new WebFrame(frameFFmpeg);
    postMessage(data, [data.webFrame.frameData.buffer]);
    data.webFrame.frameData = null;
    frame.frameData = null;
    
    frameFFmpeg.Release();
    // swscale.Release();

}

function sleep (time) {
    return new Promise((resolve) => setTimeout(resolve, time));
  }