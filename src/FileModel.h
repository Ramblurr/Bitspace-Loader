/****************************************************************************************
 * Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>                             *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/
#ifndef FILEMODEL_H
#define FILEMODEL_H

#include "loader_global.h"
#include "TransferItem.h"

#include <QAbstractTableModel>
#include <QList>
#include <QPair>

class FileModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FileModel( QObject *parent = 0 );

    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    virtual bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );
    virtual bool insertRows( int position, int rows, const QModelIndex &index = QModelIndex() );
    virtual bool removeRows( int position, int rows, const QModelIndex &index = QModelIndex() );

    /**
      * returns a QModelIndex for the file name. the column defaults to 1, which
      * will be the index for the ItemState
      */
    QModelIndex indexOf( const TransferItem* const item, int column = 1 ) const;
    TransferItemList getAll() const;
    TransferItemList getPending() const;
    TransferItemList getInProgress() const;

    bool isTransferring() const;
    bool contains( const QString &file ) const;

signals:

public slots:
    void slotStart();
    void slotAbort();

    void slotUploadProgress( qint64, qint64 );
private slots:
    void slotStartNextJob();
    void slotJobFinished( TransferItem* );

private:
    QString stateToString( const Bitspace::ItemStates & state ) const;
    TransferItemList m_list;

    qint64 m_currentActual;
    qint64 m_currentTotal;
    bool m_uploadInProgress;

};

#endif // FILEMODEL_H
