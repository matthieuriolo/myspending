#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QtSql>

class MessageBox
{
private:
    MessageBox();
public:

    static void errorMissingSQLDriver();
    static void errorNoWritableLocationFound();
    static void errorFailedToCreateWriteableDirectory(QString &path);
    static void errorFailedToOpenFile(QString fileName);
    static void errorSQL(const QSqlError &err, QWidget* parent = nullptr);
    static void errorException(std::exception &error);
};

#endif // MESSAGEBOX_H
