#ifndef GLOBALDEF_H
#define GLOBALDEF_H

#include <QString>

#define DB_NAME ":memory:"
//#define DB_NAME "myTest.db"

#define FP_VERSION_NUM "1.0.0.8"
#define FP_VERSION_INFO "1、修复同时出现“外出公干”和“PO切换”时产生外出公干工时归属PO错误的问题；\n2、调整导入界面，提高操作效率"

enum ENUM_EXPORT_XLS_TYPE
{
    EX_MONTH_TOTAL=0,
    EX_MERGE_TOTAL
};

enum ENUM_IMPORT_XLS_TYPE
{
    IM_DETAIL=0,
    IM_ABNORMAL,
    IM_PO_SWITCH,
    IM_STATISTICS
};

//punch_type:0,normal;1,beLate or leaveEarly;2,abnormal;
enum ENUM_PUNCH_TYPE
{
    PUNCH_NORMAL=0,
    PUNCH_BELATE_OR_LEAVEEARLY,
    PUNCH_ABNORMAL
};

#endif // GLOBALDEF_H
