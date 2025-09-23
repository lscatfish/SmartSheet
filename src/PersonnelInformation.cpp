
#include <basic.hpp>
#include <chstring.hpp>
#include <Encoding.h>
#include <Fuzzy.h>
#include <helper.h>
#include <PersonnelInformation.h>
#include <string>

/*
 * 这个文件不知道用来干什么
 * 现在知道了吧(^-^) lsc_write
 */


// 用于优化DefPerson中的信息
void DefPerson::optimize( ) {
    if (!this->ethnicity.empty( )) {
        if (!this->ethnicity.has_subchstring(U8C(u8"族")))
            this->ethnicity += U8C(u8"族");
    }
    if (!this->politicaloutlook.empty( )) {
        if (this->politicaloutlook.has_subchstring(U8C(u8"无")))
            this->politicaloutlook = U8C(u8"群众");
        else if (this->politicaloutlook.has_subchstring(U8C(u8"团")))
            this->politicaloutlook = U8C(u8"共青团员");
        else if (this->politicaloutlook.has_subchstring(U8C(u8"党")))
            if (this->politicaloutlook.has_subchstring(U8C(u8"预")))
                this->politicaloutlook = U8C(u8"中共预备党员");
            else
                this->politicaloutlook = U8C(u8"中共党员");
    }
    if (this->studentID.size( ) >= 4) {
        // 应当检查（0，4）是否都是数字
        // 可以优化
        auto imp = this->studentID.substr(0, 4);
        if (!imp.is_all_digits( )) {    // 啥都不干
        } else if (!this->grade.empty( )) {
            if (this->grade.has_subchstring(U8C(u8"研"))) {
                this->grade = chstring(U8C(u8"研")) + this->studentID.substr(0, 4) + U8C(u8"级");
            } else {
                this->grade = this->studentID.substr(0, 4) + U8C(u8"级");
            }
        } else {
            this->grade = this->studentID.substr(0, 4) + U8C(u8"级");
        }
    }
    if (!this->classname.empty( )) {
        if (!(this->classname.has_subchstring(U8C(u8"青")) && (this->classname.has_subchstring(U8C(u8"班"))))
            || this->classname.has_subchstring(U8C(u8"例"))) {
            if (this->classname.has_subchstring(U8C(u8"科")))
                this->classname = U8C(u8"青科班");
            else if (this->classname.has_subchstring(U8C(u8"宣")))
                this->classname = U8C(u8"青宣班");
            else if (this->classname.has_subchstring(U8C(u8"文")))
                this->classname = U8C(u8"青文班");
            else if (this->classname.has_subchstring(U8C(u8"骨")))
                this->classname = U8C(u8"青骨班");
            else if (this->classname.has_subchstring(U8C(u8"学")))
                this->classname = U8C(u8"青学班");
            else if (this->classname.has_subchstring(U8C(u8"艺")))
                this->classname = U8C(u8"青艺班");
            else if (this->classname.has_subchstring(U8C(u8"峰")))
                this->classname = U8C(u8"青峰班");
            else if (this->classname.has_subchstring(U8C(u8"公")))
                this->classname = U8C(u8"青公班");
            else if (this->classname.has_subchstring(U8C(u8"社")))
                this->classname = U8C(u8"青社班");
            else if (this->classname.has_subchstring(U8C(u8"书")))
                this->classname = U8C(u8"青书班");
            else if (this->classname.has_subchstring(U8C(u8"膺")))
                this->classname = U8C(u8"青膺班");
            else if (this->classname.has_subchstring(U8C(u8"志")))
                this->classname = U8C(u8"青志班");
            else if (this->classname.has_subchstring(U8C(u8"组")))
                this->classname = U8C(u8"青组班");
        }
    }
    // 有意思吗？内存堆的
}
