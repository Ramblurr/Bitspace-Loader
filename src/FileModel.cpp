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
#include "FileModel.h"
#include "TransferItem.h"

#include <QApplication>
#include <QIcon>
#include <QPalette>
#include <QDebug>

FileModel::FileModel( QObject *parent ) :
        QAbstractTableModel( parent ),
        m_currentActual( 0 ),
        m_currentTotal( 0 ),
        m_uploadInProgress( 0 )
{
}

QVariant FileModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= m_list.size() || index.row() < 0 )
        return QVariant();


    TransferItem* item = m_list.at( index.row() );
    switch ( role )
    {
    case Qt::DisplayRole:
        if ( index.column() == 0 )
            return item->filePath();
        else if ( index.column() == 1 )
            return stateToString( item->status() );
    case Qt::DecorationRole:
        if ( index.column() == 0 )
            return QIcon::fromTheme( "audio-x-generic" );
    case Qt::BackgroundRole:
        if ( item->status() == Bitspace::Complete )
            return QApplication::palette().color( QPalette::Disabled, QPalette::Base );
        else
            return QApplication::palette().color( QPalette::Active, QPalette::Base );
    case Qt::ForegroundRole:
        if ( item->status() == Bitspace::Complete )
            return QApplication::palette().color( QPalette::Disabled, QPalette::Text );
        else
            return QApplication::palette().color( QPalette::Active, QPalette::Text );
    case Qt::ToolTipRole:
        if ( item->status() == Bitspace::InProgress )
        {
            QString tp = tr( "%1 / %2 KB" ).arg( QString::number( m_currentActual / 1024 ), QString::number( m_currentTotal / 1024 ) );
            return tp;
        }
        else
            return tr( "Pending" );
    case Bitspace::StateRole:
        return item->status();
    case Bitspace::ProgressRole:
        return item->percentProgressed();
    case Bitspace::ElapsedTimeRole:
        return item->elapsedTime();
    case Bitspace::TotalBytesRole:
        return item->bytesTotal();
    case Bitspace::ProcessedBytesRole:
        return item->bytesProcessed();
    case Bitspace::RemainingTimeRole:
        return item->remainingTime();
    case Bitspace::SpeedRole:
        return item->currentSpeed();
    default:
        return QVariant();
    }
}

int FileModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent )
    return m_list.size();
}

int FileModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent )
    return 3;
}

QVariant FileModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();
    if ( orientation == Qt::Horizontal )
    {
        switch ( section )
        {
        case 0:
            return tr( "Name" );
        case 1:
            return tr( "Transfer" );
        case 2:
            return tr( "Progress" );
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool FileModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( index.isValid() && role == Qt::EditRole )
    {
        int row = index.row();
        TransferItem *p = m_list.value( row );

        if ( index.column() == 0 )
            p->setFile( value.toString() );
//        else if ( index.column() == 1 )
//            p->status() = ( Bitspace::ItemStates ) value.toInt();

        m_list.replace( row, p );
        emit( dataChanged( index, index ) );

        return true;
    }
    return false;
}

bool FileModel::insertRows( int position, int rows, const QModelIndex &index )
{
    Q_UNUSED( index );
    beginInsertRows( QModelIndex(), position, position + rows - 1 );

    for ( int row = 0; row < rows; row++ )
    {
        TransferItem*  pair = new TransferItem();
        m_list.insert( position, pair );
    }

    endInsertRows();
    return true;
}

bool FileModel::removeRows( int position, int rows, const QModelIndex &index )
{
    Q_UNUSED( index );
    beginRemoveRows( QModelIndex(), position, position + rows - 1 );

    for ( int row = 0; row < rows; ++row )
    {
        TransferItem* item = m_list.takeAt( position );
        delete item;
    }

    endRemoveRows();
    return true;
}

QModelIndex FileModel::indexOf( const TransferItem* const item, int column ) const
{
    for ( int i = 0; i < m_list.size(); i++ )
    {
        if ( m_list.at( i )->filePath() == item->filePath() )
            return index( i , column, QModelIndex() );
    }
    return QModelIndex();
}

TransferItemList FileModel::getAll() const
{
    TransferItemList files;
    foreach( TransferItem* item, m_list )
    {
        files << item;
    }
    return files;
}

TransferItemList FileModel::getPending() const
{
    TransferItemList files;
    foreach( TransferItem* item, m_list )
    {
        if ( item->status() == Bitspace::Pending )
            files << item;
    }
    return files;
}

TransferItemList FileModel::getInProgress() const
{
    TransferItemList files;
    foreach( TransferItem* item, m_list )
    {
        if ( item->status() == Bitspace::InProgress )
            files << item;
    }
    return files;
}

bool FileModel::isTransferring() const
{
    return m_uploadInProgress;
}

bool FileModel::contains( const QString &file ) const
{
    foreach(TransferItem* item, m_list )
    {
        if(item->filePath() == file)
            return true;
    }
    return false;
}

QString FileModel::stateToString( const Bitspace::ItemStates & state ) const
{
    switch ( state )
    {
    case Bitspace::Pending:
        return tr( "Pending" );
    case Bitspace::InProgress:
        return tr( "In Progress" );
    case Bitspace::Complete:
        return tr( "Complete" );
    default:
        return QString();
    }
}

void FileModel::slotUploadProgress( qint64 current, qint64 total )
{
    m_currentActual = current;
    m_currentTotal = total;
}

void FileModel::slotStart()
{
    qDebug() << "FileModel::slotStart()";
    TransferItemList inprogress_files = getInProgress();
    qDebug() << inprogress_files.size();
    if( inprogress_files.size() > 0 )
        return;
    slotStartNextJob();
}

void FileModel::slotAbort()
{
    qDebug() << "FileModel::slotStart()";
    m_uploadInProgress = false;

    TransferItemList inprogress_files = getInProgress();;
    foreach( TransferItem* item, inprogress_files )
    {
        // change the status for the item
        item->slotAbortJob();
        QModelIndex index = indexOf( item );
        emit dataChanged(index, index);
    }
}

void FileModel::slotStartNextJob()
{
    qDebug() << "FileModel::slotStartNextJob()";
    TransferItemList pending_files = getPending();
    if ( pending_files.size() <= 0 )
    {
        m_uploadInProgress = false;
        qDebug() << "No more Pending files. Done.";
        return;
    }
    m_uploadInProgress = true;

    TransferItem* item = pending_files.takeFirst();
    connect(item, SIGNAL(jobFinished(TransferItem*)), this, SLOT(slotJobFinished(TransferItem*)));
    qDebug() << "Starting next job: " << item->filePath();

    // change the status for the item
    QModelIndex index = indexOf( item );
    item->start();
    emit dataChanged(index, index);
}

void FileModel::slotJobFinished( TransferItem* job )
{
    QModelIndex index = indexOf( job );
    emit dataChanged(index, index);
    slotStartNextJob();
}
