#pragma once

#include <deque>

#include <QAbstractListModel>
#include <QJsonObject>

class ResultsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ResultsRoles {
        EntryRole = Qt::UserRole + 1,
        LinkRole
    };

    ResultsModel(QObject *parent = 0)
        : QAbstractListModel(parent)
    {}

    QHash<int, QByteArray> roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[EntryRole] = "entry";
        roles[LinkRole] = "link";

        return roles;
    }

private:
    std::deque<QJsonObject> entries;
};


