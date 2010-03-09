#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QPair>

namespace Bitspace
{
    enum ItemRoles
    {
        State = Qt::UserRole
    };

    enum ItemStates
    {
        Pending = 0,
        InProgress,
        Complete,

        Undefined
    };
}

typedef QPair<QString, Bitspace::ItemStates> FileItem;
class FileModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit FileModel(QObject *parent = 0);
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setData( const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    virtual bool insertRows( int position, int rows, const QModelIndex &index=QModelIndex() );
    virtual bool removeRows( int position, int rows, const QModelIndex &index=QModelIndex() );

    QStringList getList() const;

signals:

public slots:

private:
    QString stateToString( const Bitspace::ItemStates & state ) const;
    QList<FileItem> m_list;

};

#endif // FILEMODEL_H
