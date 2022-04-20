extern "C"
{
}
#include <stream.h>
#include <iostream>
#include "filterffmpeg.h"

HRESULT FilterFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG FilterFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG FilterFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT FilterFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG FilterFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG FilterFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Filter FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

FilterFFmpeg::FilterFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
{
    if(pUnknowOuter)
    {
        // 被聚合的，使用外部unknown
        m_pUnknowOuter = pUnknowOuter;
    }
    else
    {
        // 没被聚合，使用非代理unknown
        m_pUnknowOuter = reinterpret_cast<IUNknown*>(static_cast<INondelegatingUnknown*>(this));
    }
}

FilterFFmpeg::~FilterFFmpeg()
{
    std::cout << "delete filter" << std::endl;
}

IFilter::IFilterError FilterFFmpeg::filterFrameAddSubtitle(IUNknown *frameUnknown, IUNknown *subtitleUnknown)
{
    return NoError;
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of FilterFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new FilterFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
