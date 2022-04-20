#ifndef COMDELEGATE_H
#define COMDELEGATE_H

#include <string>
#include "util_global.h"

namespace boost {
    namespace dll {
        class shared_library;
    }
}
struct IUNknown;

class UTILSHARED_EXPORT COMDelegate
{
public:
    COMDelegate();
    COMDelegate(const std::string &dllname);
    ~COMDelegate();

    void setComponent(const std::string &otherDllName);
    std::string dllName();
    IUNknown* createUnknow(IUNknown* pUnknowOuter = nullptr);
private:
    boost::dll::shared_library* m_pComdll;
    typedef IUNknown* (*CreateInstance)(IUNknown*);
    CreateInstance m_fpCreate;
    std::string m_DllName;
    /**
     * @brief load 加载动态库
     */
    void load();
};

class UTILSHARED_EXPORT  IUNknownDeleter
{
public:
    IUNknownDeleter(IUNknown* pUnknowOuter);
    ~IUNknownDeleter();

private:
    IUNknown* m_pIUNknown;
};

#endif // COMDELEGATE_H
