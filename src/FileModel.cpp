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

#include <QIcon>
FileModel::FileModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

QVariant FileModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if( index.row() >= m_list.size() || index.row() < 0 )
        return QVariant();


    FileItem item = m_list.at( index.row() );
    switch ( role )
    {
    case Qt::DisplayRole:
        if( index.column() == 0 )
            return item.first;
        else
            return stateToString( item.second);
    case Qt::DecorationRole:
        return QIcon::fromTheme("audio-x-generic");
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
    return 2;
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Filename");

            case 1:
                return tr("Status");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

bool FileModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        FileItem p = m_list.value( row );

        if ( index.column() != 0 )
            return false;
        p.first = value.toString();
        p.second = p.second == Bitspace::Undefined ? Bitspace::Pending : p.second;

        m_list.replace(row, p);
        emit( dataChanged( index, index ) );

        return true;
    }
    return false;
}

bool FileModel::insertRows( int position, int rows, const QModelIndex &index )
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        FileItem pair( "", Bitspace::Undefined );
        m_list.insert(position, pair);
    }

    endInsertRows();
    return true;
}

bool FileModel::removeRows( int position, int rows, const QModelIndex &index )
{
    Q_UNUSED( index );
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        m_list.removeAt(position);
    }

    endRemoveRows();
    return true;
}

QStringList FileModel::getList() const
{
    QStringList files;
    foreach( FileItem item, m_list )
    {
        files << item.first;
    }
    return files;
}

QString FileModel::stateToString(  const Bitspace::ItemStates & state ) const
{
    switch( state )
    {
    case Bitspace::Pending:
        return tr("Pending");
    case Bitspace::InProgress:
        return tr("In Progress");
    case Bitspace::Complete:
        return tr("Complete");
    default:
        return QString();
    }
}
