﻿//////////////////////////////////////////////////////////////////////
/// Desc  IRefCount
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#include "IRefCount.hpp"

NS_FXP_BEGIN

void IRefCount::destroyThis()
{
    delete this;
}

NS_FXP_END