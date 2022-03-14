//////////////////////////////////////////////////////////////////////
/// Desc  FVector3
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FVector3.hpp"

NS_FXP_BEGIN

/*static*/ const FVector3 FVector3::Zero(FFloat(0), FFloat(0), FFloat(0));
/*static*/ const FVector3 FVector3::One (FFloat(1), FFloat(1), FFloat(1));
/*static*/ const FVector3 FVector3::XAxis(FFloat(1), FFloat(0), FFloat(0));
/*static*/ const FVector3 FVector3::YAxis(FFloat(0), FFloat(1), FFloat(0));
/*static*/ const FVector3 FVector3::ZAxis(FFloat(0), FFloat(0), FFloat(1));

NS_FXP_END
