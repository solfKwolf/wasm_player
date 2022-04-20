#ifndef IFILTER_H
#define IFILTER_H

#include <unknown.h>

extern "C" IID IID_ISCALE;

interface IFilter : public IUNknown
{
    enum IFilterError
    {
        NoError = 0,
        InitError = 1,
        ParamError = 2,
        NotInit = 3,
    };

    /**
     * @brief filterFrameAddSubtitle        在Frame上叠加字幕
     * @param frameUnknown                  视频帧接口
     * @param subtitleUnknown               字幕帧接口
     * @return
     */
    virtual IFilterError filterFrameAddSubtitle(IUNknown *frameUnknown, IUNknown *subtitleUnknown) = 0;
};

#endif // IFILTER_H
