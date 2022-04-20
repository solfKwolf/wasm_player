#ifndef UNKNOWN_H
#define UNKNOWN_H

#ifdef __WIN32__
#include <Windows.h>
#else
#include "linuxdef.h"
#endif // __WIN32

#include <assert.h>

#ifndef interface
#define interface struct
#endif // interface



/*
#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
  typedef LONG HRESULT;
#endif

#ifdef __WIDL__
typedef struct {
  unsigned long Data1;
  unsigned short Data2;
  unsigned short Data3;
  byte Data4[8];
} GUID;
#else
typedef struct _GUID {
  unsigned __LONG32 Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];
} GUID;





#ifndef BASETYPES
#define BASETYPES
  typedef unsigned long ULONG;
  typedef ULONG *PULONG;
  typedef unsigned short USHORT;
  typedef USHORT *PUSHORT;
  typedef unsigned char UCHAR;
  typedef UCHAR *PUCHAR;
  typedef char *PSZ;
#endif

*/

#ifndef E_NOINTERFACE
#define E_NOINTERFACE -1
#endif

#ifndef S_OK
#define S_OK 0
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#endif

#ifndef FAILED
#define FAILED(Status) ((HRESULT)(Status)<0)
#endif

#ifdef __cplusplus
extern "C"
#endif // __cplusplus
IID IID_IUNknown;

interface IUNknown
{

    virtual HRESULT __stdcall QueryInterface(const IID& iid, void **ppv) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
};

interface INondelegatingUnknown
{
    virtual HRESULT __stdcall NondelegatingQueryInterface(const IID& iid, void **ppv) = 0;
    virtual ULONG __stdcall NondelegatingAddRef() = 0;
    virtual ULONG __stdcall NondelegatingRelease() = 0;
};


/**
 * @brief 类模板-智能接口指针
 * @param T 模板参数-继承自IUNknown的接口
 * @param piid 接口ID
 */
template <class T, const IID* piid>
class IPtr
{
public:
    /**
     * @brief 构造函数
     */
    IPtr() : m_pI(nullptr) {}


    /**
     * @brief 构造函数
     * @param lp 要托管的接口指针
     */
    IPtr(T* lp)
        : m_pI(nullptr)
    {
        m_pI = lp;
        if(m_pI)
        {
            m_pI->AddRef();
        }
    }

    /**
     * @brief 构造函数
     * @param pI 组件的任意接口
     */
    IPtr(IUNknown* pI)
        : m_pI(nullptr)
    {
        if(pI)
        {
            HRESULT res = pI->QueryInterface(*piid,(void**)&m_pI);
            assert(res>=0);
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param ptr
     */
    IPtr(const IPtr& ptr)
    {
        if(ptr)
        {
            m_pI = ptr.m_pI;
            m_pI->AddRef();
        }
    }

    /**
     * @brief 析构函数
     */
    ~IPtr()
    {
        Release();
    }

    /**
     * @brief Release
     */
    void Release()
    {
        if(m_pI)
        {
            T* pOld = m_pI;
            m_pI = nullptr;
            pOld->Release();
        }
    }

    /**
     * @brief 重载类型转换
     */
    operator T*(){return m_pI;}

    /**
     * @brief 重载取值运算符
     * @return 被托管接口指针指向的实例的引用
     */
    T& operator*()
    {
        assert(m_pI);
        return *m_pI;
    }

    /**
     * @brief 重载取址运算符
     * @return 被托管的接口指针的地址
     */
    T** operator&()
    {
        return &m_pI;
    }

    /**
     * @brief 重载指向运算符
     * @return 被托管的接口指针的值
     */
    T* operator->()
    {
        assert(m_pI);
        return m_pI;
    }

    /**
     * @brief 重载赋值运算符
     * @param pI 同类型接口指针
     * @return 新的被托管的接口指针
     */
    T* operator=(T* pI)
    {
        if(m_pI != pI)
        {
            IUNknown* pOld = m_pI;
            m_pI = pI;
            if(m_pI)
            {
                m_pI->AddRef();
            }
            if(pOld)
            {
                pOld->Release();
            }
        }
        return m_pI;
    }


    /**
     * @brief 重载赋值运算符
     * @param pI 不同类型接口指针
     * @return 新的被托管的接口指针
     */
    T* operator =(IUNknown* pI)
    {
        IUNknown* pOld = m_pI;
        m_pI = nullptr;

        // Query for correct interface
        if(pI)
        {
            HRESULT hr = pI->QueryInterface(*piid,(void**)&m_pI);
            assert(SUCCEEDED(hr)&&(m_pI));
        }
        if(pOld)
        {
            pOld->Release();
        }
        return m_pI;
    }

    /**
     * @brief 重载逻辑非运算符
     * @return
     */
    BOOL operator!()
    {
        return (m_pI == nullptr) ? TRUE : FALSE;
    }

    /**
     * @brief 支持布尔运算
     */
    operator BOOL() const
    {
        return (m_pI != nullptr) ? TRUE : FALSE;
    }

    /**
     * @brief 被托管的接口ID
     * @return 接口ID的引用
     */
    const IID& iid()
    {
        return *piid;
    }

    T* get()
    {
        return m_pI;
    }

private:
    /**
     * @brief 被托管的接口指针
     */
    T* m_pI;
};

#endif // UNKNOWN_H

