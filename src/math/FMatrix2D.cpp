//////////////////////////////////////////////////////////////////////
/// Desc  FMatrix
/// Time  2020/12/04
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FMatrix2D.hpp"

NS_FXP_BEGIN

const FMatrix2D FMatrix2D::IDENTITY = FMatrix2D(
    FFloat(1), FFloat(0),
    FFloat(0), FFloat(1),
    FFloat(0), FFloat(0)
);

NS_FXP_END
