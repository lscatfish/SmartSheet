#include <algorithm>
#include <ChineseEncoding.h>
#include <Fuzzy.h>
#include <string>
#include <stringapiset.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>

namespace fuzzy {

// 计算两个 wstring 的 Levenshtein 距离
static int levenshtein_distance(const std::wstring s, const std::wstring t) {
    const int                         m = static_cast< int >(s.size( ));
    const int                         n = static_cast< int >(t.size( ));
    std::vector< std::vector< int > > dp(m + 1, std::vector< int >(n + 1, 0));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int cost = (s[i - 1] == t[j - 1]) ? 0 : 1;
            dp[i][j] = (std::min)({
                dp[i - 1][j] + 1,          // 删除
                dp[i][j - 1] + 1,          // 插入
                dp[i - 1][j - 1] + cost    // 替换
            });
        }
    }
    return dp[m][n];
}

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param 搜索目标
 * @param 索引库
 * @param 匹配度
 * @return 系列可能的答案
 * @note 调用bool的重载函数
 */
std::vector< std::string > fuzzy_search(
    std::string                       _target,
    const std::vector< std::string > &_searchingLib,
    LEVEL                             _matchLevel) {

    std::vector< std::string > outList;
    if (fuzzy_search(outList, _target, _searchingLib, _matchLevel)) {
        return outList;
    } else {
        return std::vector< std::string >( );
    }
}

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param 系列可能的答案
 * @param 搜索目标
 * @param 索引库
 * @param 匹配度
 * @return 是否搜索成功
 */
bool fuzzy_search(
    std::vector< std::string >       &_outList,
    std::string                       _target,
    const std::vector< std::string > &_searchingLib,
    LEVEL                             _matchLevel) {
    // 目标wstring
    std::wstring targrt = utf8_to_wstring_win(_target);
    // 是否匹配
    auto ifMatch = [&_matchLevel](int dp) -> bool {
        switch (_matchLevel) {
            case fuzzy::LEVEL::High:
                if (dp == 0)
                    return true;
                else
                    return false;
                break;
            case fuzzy::LEVEL::Medium:
                if (dp == 0 || dp == 1)
                    return true;
                else
                    return false;
                break;
            case fuzzy::LEVEL::Low:
                if (dp >= 0 && dp <= 2)
                    return true;
                else
                    return false;
                break;
            case fuzzy::LEVEL::NONE:
                return true;
                break;
            default:
                return false;
                break;
        }
    };

    int s = 0;    // 用于记录lib里是否有满足模糊条件的字符串
    for (auto &libstr : _searchingLib) {
        if (ifMatch(levenshtein_distance(targrt, utf8_to_wstring_win(libstr)))) {
            s++;
            _outList.push_back(libstr);
        }
    }


    return false;
}

}    // namespace fuzzy