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

    int rowCount(const QModelIndex& index) const override
    {
        Q_UNUSED(index)

        return entries.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if(!index.isValid()) {
            return QVariant();
        }

        if(index.row() < static_cast<int>(entries.size())) {
            const auto& entry = entries.at(index.row());
            switch (role) {
                case ResultsRoles::EntryRole:
                    return entry.value("Text").toString();

                case ResultsRoles::LinkRole:
                    return entry.value("path").toString();
            }
        }

        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[EntryRole] = "entry";
        roles[LinkRole] = "link";

        return roles;
    }

private:
    std::deque<QJsonObject> entries;
};


