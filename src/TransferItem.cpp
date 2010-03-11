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
#include "TransferItem.h"

#include "loader_global.h"

#include <QTime>
#include <QFileInfo>
#include <QDebug>

TransferItem::TransferItem( QObject *parent)
    : QObject(parent)
    , m_file( "" )
    , m_runningTime()
    , m_runningSeconds( 0 )
    , m_state( Bitspace::Undefined )
    , m_errored( false )
    , m_aborted( false )
    , m_reply( 0 )
{
}

TransferItem::TransferItem(const QString &file, QObject *parent)
    : QObject( parent )
    , m_file( file )
    , m_runningTime()
    , m_runningSeconds( 0 )
    , m_state( Bitspace::Undefined )
    , m_errored( false )
    , m_aborted( false )
    , m_reply( 0 )
{
}

TransferItem::~TransferItem()
{
    m_reply->deleteLater();
}

void TransferItem::start()
{
    m_state = Bitspace::InProgress;

    if( Bitspace::Uploader )
    {
        m_reply = Bitspace::Uploader->upload( filePath() );
        connect( m_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(slotUploadProgress(qint64,qint64)));
        connect( m_reply, SIGNAL(uploadFinished()), this, SLOT(slotUploadFinished()));
        connect( m_reply, SIGNAL(uploadError(QString)), this, SLOT(slotUploadError(QString)));
    }
    m_runningTime.restart();
}

void TransferItem::setFile( const QString &file )
{
    m_file.setFile( file );
    reset();
}

void TransferItem::reset()
{
    m_state = Bitspace::Pending;
    m_uploadedSize = 0;
    m_runningSeconds = 0;
    m_errored = false;
    m_aborted = false;
}

QString TransferItem::filePath() const
{
    return m_file.filePath();
}

Bitspace::ItemStates TransferItem::status() const
{
    return m_state;
}

qint64 TransferItem::bytesTotal() const
{
    return m_file.size();
}

qint64 TransferItem::bytesProcessed() const
{
    return m_uploadedSize;
}

int TransferItem::elapsedTime() const
{
    if ( m_state == Bitspace::InProgress )
        return m_runningTime.elapsed() / 1000;
    return m_runningSeconds;
}

int TransferItem::currentSpeed() const
{
    if ( m_state == Bitspace::InProgress )
        return m_uploadedSize / elapsedTime();
    return 0;
}

int TransferItem::averageTransferSpeed() const
{
    const int runningSeconds = elapsedTime();
    if (runningSeconds)
    {
        return bytesTotal() / runningSeconds;
    }

    return 0;
}

int TransferItem::remainingTime() const
{
    int speed = averageTransferSpeed();
    int tsize = bytesTotal();
    if ( (speed != 0) && (tsize != 0) )
        return ( tsize - m_uploadedSize ) / speed;
    return 0;
}

double TransferItem::percentProgressed() const
{
    return double( bytesProcessed() ) / double( bytesTotal() ) * 100;
}

void TransferItem::slotUploadProgress( qint64 uploaded, qint64 total)
{
        qDebug() << "Sent: " << uploaded << "Total:" << total;
    m_uploadedSize = uploaded;
}

void TransferItem::slotUploadFinished()
{
    if( m_aborted )
        return;
    m_runningSeconds = m_runningTime.elapsed() / 1000;
    qDebug() << "FINISHED: " << m_file.fileName() << m_runningSeconds << " seconds";
    if( m_errored )
    {
        qDebug() << "errored :(";
        m_state = Bitspace::Errored;
    }
    else
    {
        m_state = Bitspace::Complete;
        emit jobFinished( this );
    }
    m_reply->deleteLater();
}

void TransferItem::slotUploadError( QString error )
{
    // an abort operation triggers an "Operation canceled" error which
    // we should ignore
    if( m_aborted )
        return;
    m_errored = true;
    m_state = Bitspace::Errored;
    qDebug() << "ERROR: " << m_file.fileName() << error;
}

void TransferItem::slotAbortJob()
{
    if( m_state != Bitspace::InProgress )
        return;
    m_aborted = true;
    if( m_reply )
        m_reply->slotAbort();
    m_state = Bitspace::Pending;
}
