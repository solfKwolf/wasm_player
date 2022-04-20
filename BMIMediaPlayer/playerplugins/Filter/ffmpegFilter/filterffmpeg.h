#ifndef FILTERFFMPEG_H
#define FILTERFFMPEG_H

#if defined(FILTER_LIBRARY)
#  define FILTERSHARED_EXPORT __declspec(dllexport)
#else
#  define FILTERSHARED_EXPORT __declspec(dllimport)
#endif

extern "C"
{
    struct AVStream;
    struct AVCodec;
    struct AVCodecContext;
}

#include <unknown.h>
#include <filter.h>
#include <atomic>

class FilterFFmpeg
        : public INondelegatingUnknown
        , public IFilter
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
    FilterFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~FilterFFmpeg();

private:
    /**
     * @brief 组件实例的引用计数
     */
    std::atomic_ulong m_cRef;

    /**
     * @brief 指向非代理Unknown接口或外部组件Unknown接口
     */
    IUNknown* m_pUnknowOuter;

public:
    virtual IFilterError filterFrameAddSubtitle(IUNknown *frameUnknown, IUNknown *subtitleUnknown);

private:

};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
FILTERSHARED_EXPORT
#else
#endif // __WIN32__
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // FILTERFFMPEG_H
