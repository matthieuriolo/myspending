#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

namespace GlobalValues {
    static const QString APP_NAME = "MySpending";

    static const QString SQL_TYPE = "QSQLITE";
    static const QString SQL_CONNECTION_DEFAULT_NAME = "QSQLITE";
    static const QString SQL_DB_PATH = "myspending-data-v1.sqlite";

    static const QString SQL_TABLENAME_CATEGORY = "category";
    static const QString SQL_TABLENAME_ENTRY = "entry";

    static const QString SQL_COLUMNNAME_ID = "id";
    static const QString SQL_COLUMNNAME_CATEGORY_ID = "category_id";
    static const QString SQL_COLUMNNAME_TYPE = "type";
    static const QString SQL_COLUMNNAME_NAME = "name";
    static const QString SQL_COLUMNNAME_DESCRIPTION = "description";
    static const QString SQL_COLUMNNAME_VALUE = "value";
    static const QString SQL_COLUMNNAME_DAILY = "daily";
    static const QString SQL_COLUMNNAME_WEEKLY = "weekly";
    static const QString SQL_COLUMNNAME_MONTHLY = "monthly";
    static const QString SQL_COLUMNNAME_YEARLY = "yearly";

    static const qlonglong SQL_VALUE_NONEXISTING_PK = -1;
}

#endif // GLOBALS_H
