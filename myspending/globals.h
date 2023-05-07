#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

namespace GlobalValues {
    static const QString APP_NAME = "MySpending";


    static const QString SQL_TYPE = "QSQLITE";
    static const QString SQL_CONNECTION_DEFAULT_NAME = "QSQLITE";


    static const QString SQL_TABLENAME_CATEGORY = "category";
    static const QString SQL_TABLENAME_ENTRY = "entry";

    static const QString SQL_COLUMNNAME_ID = "id";
    static const QString SQL_COLUMNNAME_CATEGORY_ID = "category_id";
    static const QString SQL_COLUMNNAME_TYPE = "type";
    static const QString SQL_COLUMNNAME_NAME = "name";
    static const QString SQL_COLUMNNAME_DESCRIPTION = "description";
    static const QString SQL_COLUMNNAME_VALUE = "value";
}

#endif // GLOBALS_H
