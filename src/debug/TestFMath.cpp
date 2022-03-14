#include "physics2d/FPhysics2D.hpp"
#include "physics2d/FRigidbody.hpp"
#include "physics2d/FCollider.hpp"
#include "LogTool.hpp"
#include "TestTool.hpp"
#include "math/FMath.hpp"
#include "math/FVector2.hpp"

NS_FXP_BEGIN

static std::ostream& operator << (std::ostream &os, const FFloat &v)
{
    os << v.value;
    return os;
}

static int testFFloatArgs(FFloat a)
{
    return a.value;
}

static void testSinMath()
{
    // 测试三角函数
    LS_TEST_CMP(FMath::sin(FFloat(0)), FFloat(0));
    LS_TEST_CMP(FMath::sin(FFloat(30)), FFloat(true, 511));
    LS_TEST_CMP(FMath::sin(FFloat(45)), FFloat(true, 724));
    LS_TEST_CMP(FMath::sin(FFloat(90)), FFloat(1));
    LS_TEST_CMP(FMath::sin(FFloat(120)), FFloat(true, 887));
    LS_TEST_CMP(FMath::sin(FFloat(180)), FFloat(0));
    LS_TEST_CMP(FMath::sin(FFloat(210)), FFloat(true, -511));
    LS_TEST_CMP(FMath::sin(FFloat(270)), FFloat(-1));
    LS_TEST_CMP(FMath::sin(FFloat(300)), FFloat(true, -887));
    LS_TEST_CMP(FMath::sin(FFloat(360)), FFloat(0));
    LS_TEST_CMP(FMath::sin(FFloat(390)), FFloat(true, 511));
    LS_TEST_CMP(FMath::sin(FFloat(-30)), FFloat(true, -512));
    LS_TEST_CMP(FMath::sin(FFloat(-45)), FFloat(true, -724));
    LS_TEST_CMP(FMath::sin(FFloat(-90)), FFloat(-1));
    LS_TEST_CMP(FMath::sin(FFloat(-120)), FFloat(true, -886));
    LS_TEST_CMP(FMath::sin(FFloat(-180)), FFloat(0));
    LS_TEST_CMP(FMath::sin(FFloat(-210)), FFloat(true, 512));
    LS_TEST_CMP(FMath::sin(FFloat(-270)), FFloat(1));
    LS_TEST_CMP(FMath::sin(FFloat(-300)), FFloat(true, 886));
    LS_TEST_CMP(FMath::sin(FFloat(-360)), FFloat(0));
    LS_TEST_CMP(FMath::sin(FFloat(-390)), FFloat(true, -512));

    // 角度取值范围[-90, 90]
    LS_TEST_CMP(FMath::asin(FFloat(-1)), FFloat(-90));
    LS_TEST_CMP(FMath::asin(FFloat(0)), FFloat(0));
    LS_TEST_CMP(FMath::asin(FFloat(1)), FFloat(90));
    LS_TEST_CMP(FMath::asin(FFloat(true, -512)), FFloat(true, -30689)); //-30
    LS_TEST_CMP(FMath::asin(FFloat(true, -725)), FFloat(-45));
    LS_TEST_CMP(FMath::asin(FFloat(true, -886)), FFloat(true, -61378)); // -60
    LS_TEST_CMP(FMath::asin(FFloat(true, 511)), FFloat(true, 30689)); // 30
    LS_TEST_CMP(FMath::asin(FFloat(true, 724)), FFloat(true, 45987)); // 45
    LS_TEST_CMP(FMath::asin(FFloat(true, 886)), FFloat(true, 61378)); // 60

    LS_TEST_CMP(FMath::cos(FFloat(0)), FFloat(1));
    LS_TEST_CMP(FMath::cos(FFloat(30)), FFloat(true, 887));
    LS_TEST_CMP(FMath::cos(FFloat(45)), FFloat(true, 724));
    LS_TEST_CMP(FMath::cos(FFloat(90)), FFloat(0));
    LS_TEST_CMP(FMath::cos(FFloat(120)), FFloat(true, -511));
    LS_TEST_CMP(FMath::cos(FFloat(180)), FFloat(-1));
    LS_TEST_CMP(FMath::cos(FFloat(210)), FFloat(true, -887));
    LS_TEST_CMP(FMath::cos(FFloat(270)), FFloat(0));
    LS_TEST_CMP(FMath::cos(FFloat(300)), FFloat(true, 511));
    LS_TEST_CMP(FMath::cos(FFloat(360)), FFloat(1));
    LS_TEST_CMP(FMath::cos(FFloat(390)), FFloat(true, 887));
    LS_TEST_CMP(FMath::cos(FFloat(-30)), FFloat(true, 886));
    LS_TEST_CMP(FMath::cos(FFloat(-45)), FFloat(true, 724));
    LS_TEST_CMP(FMath::cos(FFloat(-90)), FFloat(0));
    LS_TEST_CMP(FMath::cos(FFloat(-120)), FFloat(true, -512));
    LS_TEST_CMP(FMath::cos(FFloat(-180)), FFloat(-1));
    LS_TEST_CMP(FMath::cos(FFloat(-210)), FFloat(true, -886));
    LS_TEST_CMP(FMath::cos(FFloat(-270)), FFloat(0));
    LS_TEST_CMP(FMath::cos(FFloat(-300)), FFloat(true, 512));
    LS_TEST_CMP(FMath::cos(FFloat(-360)), FFloat(1));
    LS_TEST_CMP(FMath::cos(FFloat(-390)), FFloat(true, 886));

    // 角度取值范围[0, 180]
    LS_TEST_CMP(FMath::acos(FFloat(-1)), FFloat(180));
    LS_TEST_CMP(FMath::acos(FFloat(0)), FFloat(90));
    LS_TEST_CMP(FMath::acos(FFloat(1)), FFloat(0));
    LS_TEST_CMP(FMath::acos(FFloat(true, -512)), FFloat(true, 122849)); //120
    LS_TEST_CMP(FMath::acos(FFloat(true, -887)), FFloat(true, 153538)); //150
    LS_TEST_CMP(FMath::acos(FFloat(true, 511)), FFloat(true, 61471)); //60
    LS_TEST_CMP(FMath::acos(FFloat(true, 724)), FFloat(true, 46173)); //45
    LS_TEST_CMP(FMath::acos(FFloat(true, 886)), FFloat(true, 30782)); //30

    LS_TEST_CMP(FMath::tan(FFloat(0)), FFloat(0));
    LS_TEST_CMP(FMath::tan(FFloat(30)), FFloat(true, 590));
    LS_TEST_CMP(FMath::tan(FFloat(45)), FFloat(1));
    LS_TEST_CMP(FMath::tan(FFloat(90)), FFloat(true, 219902325));
    LS_TEST_CMP(FMath::tan(FFloat(120)), FFloat(true, -1775));
    LS_TEST_CMP(FMath::tan(FFloat(180)), FFloat(0));
    LS_TEST_CMP(FMath::tan(FFloat(210)), FFloat(true, 590));
    LS_TEST_CMP(FMath::tan(FFloat(270)), FFloat(true, 219902325));
    LS_TEST_CMP(FMath::tan(FFloat(300)), FFloat(true, -1775));
    LS_TEST_CMP(FMath::tan(FFloat(360)), FFloat(0));
    LS_TEST_CMP(FMath::tan(FFloat(390)), FFloat(true, 590));
    LS_TEST_CMP(FMath::tan(FFloat(-30)), FFloat(true, -592));
    LS_TEST_CMP(FMath::tan(FFloat(-45)), FFloat(-1));
    LS_TEST_CMP(FMath::tan(FFloat(-89)), FFloat(true, -59257));
    LS_TEST_CMP(FMath::tan(FFloat(-120)), FFloat(true, 1769));
    LS_TEST_CMP(FMath::tan(FFloat(-180)), FFloat(0));
    LS_TEST_CMP(FMath::tan(FFloat(-210)), FFloat(true, -592));
    LS_TEST_CMP(FMath::tan(FFloat(-269)), FFloat(true, -59257));
    LS_TEST_CMP(FMath::tan(FFloat(-300)), FFloat(true, 1769));
    LS_TEST_CMP(FMath::tan(FFloat(-360)), FFloat(0));
    LS_TEST_CMP(FMath::tan(FFloat(-390)), FFloat(true, -592));

    // 角度取值范围(-90, 90)
    LS_TEST_CMP(FMath::atan(FFloat(-1)), FFloat(-45));
    LS_TEST_CMP(FMath::atan(FFloat(0)), FFloat(0));
    LS_TEST_CMP(FMath::atan(FFloat(1)), FFloat(45));
    LS_TEST_CMP(FMath::atan(FFloat(true, -1775)), FFloat(true, -61378)); //-60
    LS_TEST_CMP(FMath::atan(FFloat(true, -592)), FFloat(true, -30689)); //-30
    LS_TEST_CMP(FMath::atan(FFloat(true, -59257)), FFloat(true, -91054)); //-89
    LS_TEST_CMP(FMath::atan(FFloat(true, 1775)), FFloat(true, 61378)); //60
    LS_TEST_CMP(FMath::atan(FFloat(true, 592)), FFloat(true, 30689)); //30
    LS_TEST_CMP(FMath::atan(FFloat(true, 59257)), FFloat(true, 91054)); //89
    LS_TEST_CMP(FMath::atan(FFloat(true, 219902325)), FFloat(90));

    // atan2(y, x)
    // 1象限
    LS_TEST_CMP(FMath::atan2(FFloat(1), FFloat(1)), FFloat(true, 46080)); // 45
    LS_TEST_CMP(FMath::atan2(FFloat(1), FFloat(2)), FFloat(true, 27279)); // 26
    // 2象限
    LS_TEST_CMP(FMath::atan2(FFloat(1), FFloat(-1)), FFloat(true, 138240)); // 135
    LS_TEST_CMP(FMath::atan2(FFloat(1), FFloat(-2)), FFloat(true, 157041)); // 153
    // 3象限
    LS_TEST_CMP(FMath::atan2(FFloat(-1), FFloat(-1)), FFloat(true, -138240)); // -135
    LS_TEST_CMP(FMath::atan2(FFloat(-1), FFloat(-2)), FFloat(true, -157041)); // -153
    // 4象限
    LS_TEST_CMP(FMath::atan2(FFloat(-1), FFloat(1)), FFloat(true, -46080)); // -45
    LS_TEST_CMP(FMath::atan2(FFloat(-1), FFloat(2)), FFloat(true, -27279)); // -26
}

FXP_API void testFMath()
{
    FFloat v;
    //FFloat v1 = 1.0f;
    FFloat v2 = 1LL;
    //testFFloatArgs(1.0f);

    // LS_TEST_CMP(testFFloatArgs(1), FFloat::PRECISION);

    // 测试赋值
    v = FFloat(2);
    LS_TEST_CMP(v, FFloat(2));
    v = 3;
    LS_TEST_CMP(v, FFloat(3));

    // 不允许编译通过的操作
    //v = 1.0f;
    //v = 1LL;

    // 测试+-*/
    LS_TEST_CMP(-FFloat(2), FFloat(-2));
    LS_TEST_CMP(FFloat(2) + FFloat(1), FFloat(3));
    LS_TEST_CMP(FFloat(2) - FFloat(1), FFloat(1));
    LS_TEST_CMP(FFloat(2) * FFloat(3), FFloat(6));
    LS_TEST_CMP(FFloat(4) / FFloat(2), FFloat(2));
    LS_TEST_CMP(FFloat(4) / FFloat(3), FFloat(true, 1365));

    // 测试++ --
    v = ++FFloat(2);
    LS_TEST_CMP(v, FFloat(3));
    v = FFloat(2)++;
    LS_TEST_CMP(v, FFloat(2));
    v = --FFloat(2);
    LS_TEST_CMP(v, FFloat(1));
    v = FFloat(2)--;
    LS_TEST_CMP(v, FFloat(2));

    // 测试比较运算
    LS_TEST(FFloat(2) > FFloat(1));
    LS_TEST(FFloat(2) >= FFloat(1));
    LS_TEST(FFloat(2) >= FFloat(2));
    LS_TEST(FFloat(2) < FFloat(3));
    LS_TEST(FFloat(2) <= FFloat(3));
    LS_TEST(FFloat(3) <= FFloat(3));
    LS_TEST(FFloat(2) != FFloat(1));
    LS_TEST(FFloat(2) == FFloat(2));

    // 测试类型转换
    LS_TEST_CMP(FFloat(3).asInt(), 3);
    LS_TEST_CMP(FFloat(3.141f).asFloat(), 3.14075f);
    LS_TEST_CMP(FFloat(3.141f).asDouble(), 3.14075);
    LS_TEST_CMP((int)FFloat(3), 3);
    LS_TEST_CMP((float)FFloat(3.141f), 3.14075f);
    LS_TEST_CMP((double)FFloat(3.141f), 3.14075);

    LOG_DEBUG("FMath::FloatMin: %d, FMath::FloatMax: %d", FMath::FloatMin.value, FMath::FloatMax.value);
    LS_TEST(FMath::FloatMin < -1000);
    LS_TEST(FMath::FloatMax > 1000);

    // 测试开方运算
    LS_TEST_CMP(FMath::sqrt32(0xffffffff), 0xffff);
    LS_TEST_CMP(FMath::sqrt32(9), 3);
    LS_TEST_CMP(FMath::sqrt32(4), 2);
    LS_TEST_CMP(FMath::sqrt32(1), 1);
    LS_TEST_CMP(FMath::sqrt32(2), 1);
    LS_TEST_CMP(FMath::sqrt32(3), 1);
    LS_TEST_CMP(FMath::sqrt32(5), 2);
    LS_TEST_CMP(FMath::sqrt32(6), 2);
    LS_TEST_CMP(FMath::sqrt32(10000), 100);
    LS_TEST_CMP(FMath::sqrt(FFloat(41)), FFloat(true, 6556));

    LS_TEST_CMP(FMath::sqrt(FFloat(true, 0x7fffffff)), FFloat(true, 1482910));
    LS_TEST_CMP(FMath::sqrt(FFloat(9)), FFloat(3));
    LS_TEST_CMP(FMath::sqrt(FFloat(10000)), FFloat(100));

    // 测试三角函数
    testSinMath();

    // 测试整数是否越界
    FVector2 vec2(FFloat(0), FFloat(true, 64699));
    LS_TEST(vec2.length() > 0);
    vec2.set(4, 5);
    LS_TEST_CMP(vec2.lengthSq(), 41);
    LS_TEST_CMP(vec2.length(), FFloat(true, 6556));
    LS_TEST_CMP(vec2.dot(vec2), 41);
    LS_TEST_CMP(vec2.cross(vec2), 0);
    LS_TEST_CMP(vec2.cross(FVector2(-5, 4)), 41);
    LS_TEST_CMP(vec2.distanceTo(FVector2(4, 8)), 3);
    LS_TEST_CMP(vec2.distanceToSq(FVector2(4, 8)), 9);

    FBB bb(FVector2(0, 0), FVector2(4, 8));
    FVector2 start, end;
    start.set(-2, -2);
    end.set(10, 10);
    LS_TEST(bb.clipLine(start, end));
    LS_TEST_CMP(start.x, FFloat(0));
    LS_TEST_CMP(start.y, FFloat(0));
    LS_TEST_CMP(end.x, FFloat(4));
    LS_TEST_CMP(end.y, FFloat(4));
}

NS_FXP_END
