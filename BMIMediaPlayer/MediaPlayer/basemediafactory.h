#ifndef BASEMEDIAFACTORY_H
#define BASEMEDIAFACTORY_H

struct IUNknown;

class BaseMediaFactory
{
public:
    BaseMediaFactory();

    // fyf: 基类需要将析构函数写为虚函数
    virtual ~BaseMediaFactory();

    virtual IUNknown *createDemuxerUNknown() = 0;
    virtual IUNknown *createDecoderUNknown() = 0;
    virtual IUNknown *createPacketUNknown() = 0;
    virtual IUNknown *createStreamUNknown() = 0;
    virtual IUNknown *createFrameUNknown() = 0;
    virtual IUNknown *createScaleUNknown() = 0;
};

#endif // BASEFACTORY_H
