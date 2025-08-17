#include <algorithm>
#include <basic.hpp>
#include <Encoding.h>
#include <Fuzzy.h>
#include <PersonnelInformation.h>
#include <QingziClass.h>
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

// 判断 Levenshtein 距离与 LEVEL 之间的关系
static bool ifmatch_levenshtein(int dp, LEVEL _matchLevel) {
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
}

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _target 搜索目标
 * @param _searchingLib 索引库
 * @param _matchLevel 匹配度
 * @return 系列可能的答案
 */
list< std::string > search(
    std::string                _target,
    const list< std::string > &_searchingLib,
    LEVEL                      _matchLevel) {

    list< std::string > outList;
    if (search(outList, _target, _searchingLib, _matchLevel)) {
        return outList;
    } else {
        return list< std::string >( );
    }
}

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _outList 系列可能的答案
 * @param _target 搜索目标
 * @param _searchingLib 索引库
 * @param _matchLevel 匹配度
 * @return 是否搜索成功
 */
bool search(
    list< std::string >       &_outList,
    std::string                _target,
    const list< std::string > &_searchingLib,
    LEVEL                      _matchLevel) {
    // 目标wstring
    std::wstring targrt = encoding::utf8_to_wstring_win(_target);

    int s = 0;    // 用于记录lib里是否有满足模糊条件的字符串
    for (auto &libstr : _searchingLib) {
        if (ifmatch_levenshtein(levenshtein_distance(targrt, encoding::utf8_to_wstring_win(libstr)), _matchLevel)) {
            s++;
            _outList.push_back(libstr);
        }
    }

    if (s == 0)
        return false;
    else
        return true;
}

/*
 * @brief 模糊搜索函数,没有返回可能匹配的答案
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _searchingLib 索引库
 * @param _target 搜索目标
 * @param _matchLevel 匹配度
 * @return 是否搜索成功
 */
bool search(
    const list< std::string > &_searchingLib,
    std::string                _target,
    LEVEL                      _matchLevel) {

    int s = 0;    // 用于记录lib里是否有满足模糊条件的字符串
    if (_matchLevel == LEVEL::High) {
        // 直接比较
        for (auto &libstr : _searchingLib) {
            if (libstr == _target) {
                s++;
                return true;
            }
        }
        if (s > 0)
            return true;
        else
            return false;
    }

    // 目标wstring
    std::wstring targrt = encoding::utf8_to_wstring_win(_target);
    for (auto &libstr : _searchingLib) {
        if (ifmatch_levenshtein(levenshtein_distance(targrt, encoding::utf8_to_wstring_win(libstr)), _matchLevel)) {
            s++;
            return true;
        }
    }

    if (s == 0)
        return false;
    else
        return true;
}

/*
 * @brief 模糊搜索人员信息
 * @param _outList 系列可能的人员信息
 * @param _likelyRate 相似度
 * @param _target 搜索目标
 * @param _searchingLib 搜索库
 * @retrun 是否搜索成功
 */
bool search_for_person(
    list< DefPerson >       &_outList,
    list< double >          &_likelyRate,
    DefPerson                _target,
    const list< DefPerson > &_searchingLib) {

    /*
     * ****************************** 函数思路 ************************************
     * 遍历库中所有的人员
     * 第一优先级：各自班级
     * 第二优先级：学号，先采用high的LEVEL；如果没有，再采用low的LEVEL(考虑到34交换成43)
     * 第三优先级：姓名，采用med的LEVEL
     * ****************************** 函数思路 ************************************
     */
    int s = 0;
    for (auto &it_lib : _searchingLib) {
        if (it_lib.classname == _target.classname) {                                        // 第一优先级
            if (DoQingziClass::compare_studentID(it_lib.studentID, _target.studentID)) {    // high
                _outList.push_back(it_lib);
                _likelyRate.push_back(1.0);
                s++;
            } else {    // low匹配
                std::string i1 = it_lib.studentID;
                std::string i2 = _target.studentID;

                // 去掉末尾的t
                if (!i1.empty( ) && (i1.back( ) == 't' || i1.back( ) == 'T')) i1.pop_back( );
                if (!i2.empty( ) && (i2.back( ) == 't' || i2.back( ) == 'T')) i2.pop_back( );
                if (ifmatch_levenshtein(
                        levenshtein_distance(encoding::utf8_to_wstring_win(i1), encoding::utf8_to_wstring_win(i2)),
                        LEVEL::Low)) {
                    _outList.push_back(it_lib);
                    _likelyRate.push_back(0.6);
                    s++;
                } else {    // 姓名匹配
                    if (ifmatch_levenshtein(
                            levenshtein_distance(encoding::utf8_to_wstring_win(it_lib.name), encoding::utf8_to_wstring_win(_target.name)),
                            LEVEL::Medium)) {
                        _outList.push_back(it_lib);
                        _likelyRate.push_back(0.7);
                        s++;
                    }
                }
            }
        }
    }
    if (s == 0)
        return false;
    else
        return true;
}

}    // namespace fuzzy