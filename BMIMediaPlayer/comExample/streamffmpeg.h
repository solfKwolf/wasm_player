#ifndef STREAMFFMPEG_H
#define STREAMFFMPEG_H

#include <unknown.h>
#include <atomic>

class StreamFFmpeg
        : public INondelegatingUnknown
{
public:
    // IUNknown
    virtual HRESULT __stdcall QueryInterface(const IID& iid, void **ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();

    // INondelegatingUnknown
    virtual HRESULT __stdcall NondelegatingQueryInterface(const IID& iid, void **ppv);
    virtual ULONG __stdcall NondelegatingAddRef();
    virtual ULONG __stdcall NondelegatingRelease();
    /**
     * @brief 构造函数
     * @param pUnknowOuter 外部组件Unknown接口指针，nullptr表示不聚合
     */
    StreamFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    ~StreamFFmpeg();

private:
    /**
     * @brief 组件实例的引用计数
     */
    std::atomic_ulong m_cRef;

    /**
     * @brief 指向非代理Unknown接口或外部组件Unknown接口
     */
    IUNknown* m_pUnknowOuter;
};

#endif // STREAMFFMPEG_H
