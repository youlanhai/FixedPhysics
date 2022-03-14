//////////////////////////////////////////////////////////////////////
/// Desc  IRefCount
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#pragma once

#include "FConfig.hpp"

NS_FXP_BEGIN

/// 引用计数基类，非线程安全
class IRefCount
{
public:
    IRefCount()
    {}

    virtual ~IRefCount()
    {}

    void retain()
    {
        ++refCount_;
    }

    void release()
    {
        --refCount_;
        if(refCount_ <= 0)
        {
            destroyThis();
        }
    }

    int getRefCount()
    {
        return refCount_;
    }

    virtual void destroyThis();

protected:
    int refCount_ = 0;
};

NS_FXP_END
