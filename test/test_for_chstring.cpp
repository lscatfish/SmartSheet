
#include <algorithm>
#include <basic.hpp>
#include <chstring.hpp>
#include <cstdlib>
#include <iostream>
#include <test/test_for_chstring.h>
#include <vector>
#include <string>

void test_for_chstring_cout( ) {
    std::vector< chstring > a{
        "jdjvn",
        "I love Chongqing",
        U8C(u8"our earth"),
        "a",
        U8C(u8"i"),
        "我们的世界",
        U8C(u8"青公班123"),
        "青公班123",
        "青社班1",
        chstring("青社班1", chstring::csType::SYS),
        "青志班1",
        "青骨班1",
        U8C(u8"青公班1"),
        "青公班1",
        chstring("青公班1", chstring::csType::SYS),
        chstring("青公班1", chstring::csType::UTF8),
        "你粉嫩粉嫩的妓女风纪扣v你",
        U8C(u8"你的呢二v俄军哦v你")
    };
    for (const auto &t : a) {
        std::cout << t << std::endl;
    }
    // 排序
    std::sort(a.begin( ), a.end( ), [](const chstring &a, const chstring &b) { return a < b; });
    std::cout << std::endl;
    for (const auto &t : a) {
        std::cout << t << std::endl;
    }
}

// 测试字符串的相加功能
void test_for_chstring_plus( ) {
    std::vector< chstring > a{
        "jdjvn",
        "I love Chongqing",
        U8C(u8"our earth"),
        "a",
        U8C(u8"i"),
        "我们的世界",
        U8C(u8"青公班123"),
        "青公班123",
        "青社班1",
        chstring("青社班1", chstring::csType::SYS),
        "青志班1",
        "青骨班1",
        U8C(u8"青公班1"),
        "青公班1",
        chstring("青公班1", chstring::csType::SYS),
        chstring("青公班1", chstring::csType::UTF8),
        "你粉嫩粉嫩的妓女风纪扣v你",
        U8C(u8"你的呢二v俄军哦v你")
    };

    chstring plus1;
    for (const auto &c : a) {
        plus1 += c;
    }
    std::cout << plus1 << std::endl;
    if (plus1.cstype( ) == chstring::csType::SYS) {
        std::cout << "SYS"<<std::endl;
    } else if (plus1.cstype( ) == chstring::csType::UTF8) {
        std::cout << "UTF8" << std::endl;
    }
    std::cout << std::endl;
    plus1 = chstring(chstring::csType::SYS);
    for (const auto &c : a) {
        plus1 += c;
    }
    std::cout << plus1 << std::endl;
    if (plus1.cstype( ) == chstring::csType::SYS) {
        std::cout << "SYS" << std::endl;
    } else if (plus1.cstype( ) == chstring::csType::UTF8) {
        std::cout << "UTF8" << std::endl;
    }

    std::cout << "\n\n" << plus1 + "\n";
    std::cout << plus1 + std::string("\n");

    chstring p2 = chstring( plus1,chstring::csType::UTF8);
    std::cout << p2 + chstring("\n\n", chstring::csType::SYS);
}
