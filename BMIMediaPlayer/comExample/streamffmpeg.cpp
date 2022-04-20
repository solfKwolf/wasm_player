#include "streamffmpeg.h"
#include <iostream>

HRESULT StreamFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG StreamFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG StreamFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT StreamFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
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

ULONG StreamFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG StreamFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Packet FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

StreamFFmpeg::StreamFFmpeg(IUNknown *pUnknowOuter)
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

StreamFFmpeg::~StreamFFmpeg()
{

}

