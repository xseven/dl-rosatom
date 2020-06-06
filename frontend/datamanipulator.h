#pragma once

#include <QObject>
#include <QDebug>

class DataManipulator: public QObject
{
    Q_OBJECT

    public slots:
        void serializeData(const QString &msg)
        {
            qDebug() << "Called the C++ slot with message:" << msg;
        }
};
