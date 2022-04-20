#include "comdelegate.h"
#include <unknown.h>
#include <assert.h>
#include <boost/dll/shared_library.hpp>
#include <iostream>

COMDelegate::COMDelegate(const std::string &dllname)
    : m_DllName(dllname)
{
    m_pComdll = new boost::dll::shared_library(m_DllName);
    if(!dllname.empty())
        load();
}

COMDelegate::~COMDelegate()
{
    if(m_pComdll->is_loaded())
        m_pComdll->unload();
    delete m_pComdll;
}

void COMDelegate::setComponent(const std::string &otherDllName)
{
    m_pComdll->unload();
    delete m_pComdll;

    m_DllName = otherDllName;
    m_pComdll = new boost::dll::shared_library(m_DllName);
    load();
}

std::string COMDelegate::dllName()
{
    return m_DllName;
}

IUNknown *COMDelegate::createUnknow(IUNknown *pUnknowOuter)
{
    if(m_fpCreate)
        return m_fpCreate(pUnknowOuter);
    return nullptr;
}

void COMDelegate::load()
{
    if(!m_pComdll)
        return;
    if(m_pComdll->is_loaded())
    {
        std::cout <<  m_DllName << "DLL loaded." << std::endl;

        // 获取CreateInstance函数指针
        assert(m_pComdll->has("CreateInstance"));
        m_fpCreate = m_pComdll->get<IUNknown* (IUNknown*)>("CreateInstance");
        assert(m_fpCreate);
    }
    else
    {
        std::cout << m_DllName <<" DLL load failed." << std::endl;
    }
}



IUNknownDeleter::IUNknownDeleter(IUNknown *pUnknowOuter)
{
    m_pIUNknown = pUnknowOuter;
}

IUNknownDeleter::~IUNknownDeleter()
{
    if(m_pIUNknown)
        m_pIUNknown->Release();
}
