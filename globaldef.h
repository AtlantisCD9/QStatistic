#ifndef GLOBALDEF_H
#define GLOBALDEF_H

#include <QString>

#define DB_NAME ":memory:"
//#define DB_NAME "myTest.db"


#define FP_VERSION_NUM "1.0.0.7"
#define FP_VERSION_INFO "1、增加PO切换时，对日期格式的统一;\n2、增加针对身份证号码的大小写格式统一"



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
