class WebPacket {
    constructor(packet) {
        if(packet == null){
            return;
        }
       this.streamIndex = packet.getStreamIndex();
       var packetData = packet.getData();
       packetData  = new Uint8Array(packetData);
       this.packetData = packetData;
       this.ptsMSec = packet.getPtsMSec();
       this.dtsMSec = packet.getDtsMSec();
       this.isKeyframe = packet.isKeyframe();
       this.isEnd = packet.isEnd();
       this.packetName = packet.packetName();
       this.mediaType = packet.getPacketType().value;
       this.packetPos = packet.getPacketPos();
       this.packetDuration = packet.getPacketDuration();
       this.packetFlag = packet.getPacketFlag();
       
    }

    convertToPacketFFmpeg(packetFFmpeg) {
        switch (this.mediaType) {
            case -1:
                this.mediaType = Module.PacketAVMediaType.AVTypeUnknow;
                break;
            case 0:
                this.mediaType = Module.PacketAVMediaType.AVTypeVideo;
                break;
            case 1:
                this.mediaType = Module.PacketAVMediaType.AVTypeAudio;
                break;
            case 2:
                this.mediaType = Module.PacketAVMediaType.AVTypeData;
                break;
            case 3:
                this.mediaType = Module.PacketAVMediaType.AVTypeSubtitle;
                break;
            case 4:
                this.mediaType = Module.PacketAVMediaType.AVTypeAttachment;
                break;
            default:
                this.mediaType = Module.PacketAVMediaType.AVTypeUnknow;
        }

        packetFFmpeg.setStreamIndex(this.streamIndex);
        packetFFmpeg.setData(this.packetData);
        packetFFmpeg.setPtsMSec(this.ptsMSec);
        packetFFmpeg.setDtsMSec(this.dtsMSec);
        packetFFmpeg.setKeyframeFlag(this.isKeyframe);

        packetFFmpeg.setEofFlag(this.isEnd);
        packetFFmpeg.setPacketType(this.mediaType);
        packetFFmpeg.setPacketPos(this.packetPos);
        packetFFmpeg.setPacketDuration(this.packetDuration);
        packetFFmpeg.setPacketFlag(this.packetFlag);

    }
}