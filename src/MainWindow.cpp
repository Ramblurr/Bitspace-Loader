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
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ConfigDialog.h"
#include "FileModel.h"

#include "ws.h" //libbitspace
#include "upload/Upload.h" //libbitspace

#include <QDir>
#include <QDirIterator>
#include <QIcon>
#include <QFileDialog>
#include <QDebug>
#include <QPointer>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupActions();
    m_model = new FileModel( this );
    ui->tableView->setModel( m_model );
    ui->tableView->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );

   slotOptionsChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::setupActions()
{

    m_addFiles = new QAction( QIcon::fromTheme("document-open"), tr("Add Files"), this );
    m_addFolders = new QAction( QIcon::fromTheme("document-open-folder"), tr("Add Folders"), this );
    m_upload = new QAction( QIcon::fromTheme("go-up") , tr("Start Upload"), this );

    ui->mainToolBar->addAction( m_addFiles );
    ui->mainToolBar->addAction( m_addFolders );
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction( m_upload );

    connect( m_addFiles, SIGNAL( triggered() ), SLOT( slotAddFiles() ) );
    connect( m_addFolders, SIGNAL( triggered() ), SLOT( slotAddFolders() ) );
    connect( m_upload, SIGNAL( triggered() ), SLOT( slotStartNextJob() ) );
}

void MainWindow::addFile( const QString &file )
{
    m_model->insertRows( 0, 1, QModelIndex() );
    QModelIndex index = m_model->index( 0, 0, QModelIndex() );
    m_model->setData( index, file, Qt::EditRole );
    index = m_model->index( 0, 1, QModelIndex() );
    m_model->setData( index, Bitspace::Pending, Qt::EditRole );
}

void MainWindow::slotAddFiles()
{
    QFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::ExistingFiles );
    dialog.setNameFilter( tr( "Audio files (*.mp3 *.m4a *.mp4 *.aac *.ogg *.oga *.wma *.flac)" ) );

    QStringList filenames;
    if( dialog.exec() )
    {
        filenames = dialog.selectedFiles();
    }
    if( filenames.size() <= 0 )
        return;

    QStringList current_files = m_model->getAll();
    foreach( QString file, filenames )
    {
        if( current_files.contains( file ) )
            continue;
        addFile( file );
    }
}

void MainWindow::slotAddFolders()
{
    QFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::Directory );
    dialog.setOption(QFileDialog::ShowDirsOnly, true );

    QStringList folders;
    if( dialog.exec() )
    {
        folders = dialog.selectedFiles();
    }
    if( folders.size() <= 0 )
        return;
    QString folder_name = folders.at(0);
    QDir dir( folder_name );

    if( !dir.exists() )
        return;

    QDirIterator it( folder_name, nameFilters(), QDir::Files, QDirIterator::Subdirectories);
    QStringList current_files = m_model->getAll();
    while( it.hasNext() )
    {
        it.next();
        QString file = it.filePath();
        if( current_files.contains( file ) )
            continue;
        addFile( file );
    }
}

void MainWindow::slotUploadProgress( qint64 sent, qint64 total)
{
    qDebug() << "Sent: " << sent << "Total:" << total;
    double percent = double(sent) / double(total)*100;
    qDebug() << "Percentage: " << percent;
}

QStringList MainWindow::nameFilters() const
{
    QStringList filters;
    filters << "*.mp3";
    filters << "*.m4a";
    filters << "*.mp4";
    filters << "*.aac";
    filters << "*.ogg";
    filters << "*.oga";
    filters << "*.wma";
    filters << "*.flac";
    return filters;
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionOptions_triggered()
{
    QPointer<ConfigDialog> dialog = new ConfigDialog( this );
    if( dialog->exec() == QDialog::Accepted )
    {
        slotOptionsChanged();
    }
    delete dialog;
}

void MainWindow::slotOptionsChanged()
{
    QSettings settings;
    QString apitoken = settings.value( "apitoken" ).toString();
    if( apitoken.isEmpty() )
    {
        on_actionOptions_triggered();
        return;
    }

    qDebug() << "token: " << apitoken;
    bitspace::ws::ApiToken = apitoken;
    bitspace::setNetworkAccessManager( new QNetworkAccessManager(this) );
    m_uploader = new bitspace::Upload( this );
    m_uploader->startNewSession();
}

void MainWindow::slotStartNextJob()
{
    QStringList pending_files = m_model->getPending();
    if( pending_files.size() <= 0 )
        return;

    QString file = pending_files.takeFirst();

    qDebug() << "Starting next job: " << file;

    // change the status for the item
    QModelIndex index = m_model->indexOf( file );
    m_model->setData( index, Bitspace::InProgress, Qt::EditRole );

    bool success = m_uploader->upload( file );
    if( !success  )
        return; // TODO error handling

    connect(m_uploader, SIGNAL(uploadProgress(qint64,qint64)), SLOT(slotUploadProgress( qint64, qint64 )));
    connect(m_uploader, SIGNAL(uploadFinished()), SLOT(slotUploadFinished()));
}

void MainWindow::slotUploadFinished()
{
    qDebug() << "MainWindow::slotUploadFinished()";
    slotStartNextJob();
}
