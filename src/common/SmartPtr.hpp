//////////////////////////////////////////////////////////////////////
/// Desc  SmartPtr
/// Time  2019/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "FConfig.hpp"
#include <cassert>

NS_FXP_BEGIN

///智能指针
template<typename T>
class SmartPtr
{
public:
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T * const_pointer;
    typedef const T & const_reference;
    typedef SmartPtr<T> this_type;

    SmartPtr()
        : m_ptr(nullptr)
    {}

    SmartPtr(pointer p)
        : m_ptr(p)
    {
        if (m_ptr != nullptr)
        {
            m_ptr->retain();
        }
    }

    SmartPtr(const this_type & o)
        : m_ptr(o.m_ptr)
    {
        if (m_ptr != nullptr)
        {
            m_ptr->retain();
        }
    }

    template<typename U>
    SmartPtr(U * p)
        : m_ptr(nullptr)
    {
        *this = p;
    }
    
    template<typename U>
    SmartPtr(const SmartPtr<U> & o)
        : m_ptr(nullptr)
    {
        *this = o.get();
    }
    
    // move construct
    SmartPtr(this_type && o)
        : m_ptr(o.m_ptr)
    {
        o.m_ptr = nullptr;
    }

    // move construct
    template<typename U>
    SmartPtr(SmartPtr<U> && o)
        : m_ptr(o.get())
    {
        o.reset();
    }
    
    ~SmartPtr()
    {
        if(m_ptr != nullptr)
        {
            m_ptr->release();
        }
    }

    pointer get() const
    {
        return m_ptr;
    }

    template<typename U>
    U* cast() const
    {
        return dynamic_cast<U*>(const_cast<T*>(m_ptr));
    }

    bool hasObject() const
    {
        return m_ptr != 0;
    }

    bool exists() const
    {
        return m_ptr != 0;
    }

    void reset()
    {
        m_ptr = nullptr;
    }

public:

    operator bool () const
    {
        return m_ptr != nullptr;
    }

    pointer operator -> () const
    {
        assert(m_ptr != nullptr && "SmartPtr::operator ->");
        return m_ptr;
    }

    reference operator * () const
    {
        assert(m_ptr != nullptr && "SmartPtr::operator *");
        return *m_ptr;
    }

    const this_type & operator = (pointer p)
    {
        if(m_ptr != p)
        {
            if(m_ptr != nullptr) m_ptr->release();
            m_ptr = p;
            if (m_ptr != nullptr)
            {
                m_ptr->retain();
            }
        }
        return *this;
    }

    template<typename U>
    const this_type & operator = (U *p)
    {
        if(m_ptr != p)
        {
            if(m_ptr != nullptr) m_ptr->release();

            if(p != nullptr)
            {
                m_ptr = dynamic_cast<pointer>(p);
                assert(m_ptr && "SmartPtr::operator= - Invalid type cast!");
                m_ptr->retain();
            }
            else
            {
                m_ptr = nullptr;
            }
        }
        return *this;
    }

    const this_type & operator = (const this_type & o)
    {
        return *this = o.m_ptr;
    }

    template<typename U>
    const this_type & operator = (const SmartPtr<U> & o)
    {
        return *this = o.get();
    }

    const this_type & operator = (this_type && o)
    {
        if(m_ptr != o.m_ptr)
        {
            if(m_ptr != nullptr)
            {
                m_ptr->release();
            }
            m_ptr = o.m_ptr;
            o.m_ptr = nullptr;
        }
        return *this;
    }

    template<typename U>
    const this_type & operator = (SmartPtr<U> && o)
    {
        if(m_ptr != o.get())
        {
            if(m_ptr != nullptr)
            {
                m_ptr->release();
            }

            m_ptr = dynamic_cast<pointer>(o.get());
            assert(m_ptr && "SmartPtr::operator= - Invalid type cast!");

            o.reset();
        }
        return *this;
    }

    bool operator == (const this_type & o) const
    {
        return m_ptr == o.m_ptr;
    }

    bool operator < (const this_type & o) const
    {
        return m_ptr < o.m_ptr;
    }

    bool operator > (const this_type & o) const
    {
        return m_ptr > o.m_ptr;
    }

#if 0

    SmartPtr(std::nullptr_t)
        : m_ptr(nullptr)
    {
    }

    const this_type & operator = (std::nullptr_t)
    {
        if (m_ptr) m_ptr->release();
        m_ptr = nullptr;
        return *this;
    }

#endif

private:
    pointer m_ptr;
};


template<typename T, typename U>
bool operator == (const SmartPtr<T> & l, const U * r)
{
    return l.get() == r;
}

template<typename T, typename U>
bool operator == (const T *l, const SmartPtr<U> & r)
{
    return l == r.get();
}

template<typename T, typename U>
bool operator == (const SmartPtr<T> & l, const SmartPtr<U> & r)
{
    return l.get() == r.get();
}

template<typename T, typename U>
bool operator != (const SmartPtr<T> & l, const U * r)
{
    return l.get() != r;
}

template<typename T, typename U>
bool operator != (const T *l, const SmartPtr<U> & r)
{
    return l != r.get();
}

template<typename T, typename U>
bool operator != (const SmartPtr<T> & l, const SmartPtr<U> & r)
{
    return l.get() != r.get();
}


template<typename T>
bool operator == (const SmartPtr<T> & l, std::nullptr_t)
{
    return l.get() == nullptr;
}

template<typename T>
bool operator != (const SmartPtr<T> & l, std::nullptr_t)
{
    return l.get() != nullptr;
}

NS_FXP_END
