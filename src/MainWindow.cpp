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
#include "FileDelegate.h"

#include "ws.h" //libbitspace
#include "upload/Upload.h" //libbitspace

#include <QtAlgorithms>
#include <QCloseEvent>
#include <QDir>
#include <QDirIterator>
#include <QIcon>
#include <QFileDialog>
#include <QDebug>
#include <QPointer>
#include <QSettings>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_uploadInProgress( false )
{
    ui->setupUi(this);
    setupActions();
    createTrayIcon();

    m_model = new FileModel( this );
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotItemsChanged()));
    ui->tableView->setModel( m_model );
    ui->tableView->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );

    slotOptionsChanged();
    FileDelegate* delegate = new FileDelegate( this );
    ui->tableView->setItemDelegate( delegate );

    slotItemsChanged();
    m_trayIcon->show();
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Systray"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}

void MainWindow::setupActions()
{

    m_addFiles = new QAction( QIcon::fromTheme("document-open"), tr("Add Files"), this );
    m_addFolders = new QAction( QIcon::fromTheme("document-open-folder"), tr("Add Folders"), this );
    m_uploadAction = new QAction( QIcon::fromTheme("go-up") , tr("Start Upload"), this );
    m_quitAction = new QAction(tr("&Quit"), this);

    ui->mainToolBar->addAction( m_addFiles );
    ui->mainToolBar->addAction( m_addFolders );
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction( m_uploadAction );

    connect( m_addFiles, SIGNAL( triggered() ), SLOT( slotAddFiles() ) );
    connect( m_addFolders, SIGNAL( triggered() ), SLOT( slotAddFolders() ) );
    connect( m_uploadAction, SIGNAL( triggered() ), SLOT( on_m_uploadAction_triggered() ) );
    connect(m_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    m_trayIconMenu = new QMenu( this );
    m_trayIconMenu->addAction(m_uploadAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon( this );
    m_trayIcon->setContextMenu(m_trayIconMenu);
    m_trayIcon->setToolTip( tr( "Bitspace Loader: Waiting" ) );
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::addFile( const QString &file )
{
    int new_row = m_model->rowCount();
    m_model->insertRows( new_row, 1, QModelIndex() );
    QModelIndex index = m_model->index( new_row, 0, QModelIndex() );
    m_model->setData( index, file, Qt::EditRole );
    index = m_model->index( new_row, 1, QModelIndex() );
    m_model->setData( index, Bitspace::Pending, Qt::EditRole );
}

void MainWindow::slotIconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch ( reason )
    {
    case QSystemTrayIcon::Trigger:
        showNormal();
        break;
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
    default:
        break;
    }
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

    //sort ascending
    qSort( filenames );
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
    QStringList filenames;
    //create list of files
    while( it.hasNext() )
    {
        it.next();
        QString file = it.filePath();
        filenames << file;
    }
    //sort files
    qSort( filenames );

    // ad them to the model
    QStringList current_files = m_model->getAll();
    foreach( QString file, filenames )
    {
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
    m_progress = percent;
    m_trayIcon->setToolTip( tr( "Bitspace Loader: Uploading %1%" ).arg( QString::number( (int) m_progress ) ) );
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
    connect(m_uploader, SIGNAL(uploadProgress(qint64,qint64)), SLOT(slotUploadProgress( qint64, qint64 )));
    connect(m_uploader, SIGNAL(uploadProgress(qint64,qint64)), m_model, SLOT(slotUploadProgress( qint64, qint64 )));
    connect(m_uploader, SIGNAL(uploadFinished()), SLOT(slotUploadFinished()));
    connect(m_uploader, SIGNAL(uploadError(QString)), SLOT(slotUploadError(QString)));
    connect(this, SIGNAL(abort()), m_uploader, SLOT(slotAbort()));
}

void MainWindow::on_m_uploadAction_triggered()
{
    if( m_uploadInProgress )
    {
        slotAbortUpload();
    } else
        slotStartNextJob();
}

void MainWindow::slotStartNextJob()
{
    qDebug() << "MainWindow::slotStartNextJob()";
    QStringList pending_files = m_model->getPending();
    if( pending_files.size() <= 0 )
    {
        m_uploadInProgress = false;
        qDebug() << "No more Pending files. Done.";
        return;
    }
    m_uploadInProgress = true;
    m_progress = 0;

    QString file = pending_files.takeFirst();

    qDebug() << "Starting next job: " << file;

    // change the status for the item
    QModelIndex index = m_model->indexOf( file );
    m_model->setData( index, Bitspace::InProgress, Qt::EditRole );

    setUploadIcon();

    bool success = m_uploader->upload( file );
    if( !success  )
        return; // TODO error handling
}

void MainWindow::slotUploadFinished()
{
    qDebug() << "MainWindow::slotUploadFinished()";

    QStringList inprogress_files = m_model->getInProgress();
    foreach(QString file, inprogress_files)
    {
        // change the status for the item
        QModelIndex index = m_model->indexOf( file );
        m_model->setData( index, Bitspace::Complete, Qt::EditRole );
    }

    if( m_uploadInProgress )
        slotStartNextJob();
    setUploadIcon();
}

void MainWindow::slotUploadError( QString error )
{
    qDebug() << "slotUploadError" << error;
    slotAbortUpload();

    QMessageBox msgBox;
    msgBox.setText( error );
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

double MainWindow::progress() const
{
    return m_progress;
}

bool MainWindow::isOperationRunning() const
{
    return m_uploadInProgress;
}

void MainWindow::setUploadIcon()
{
    if( m_uploadInProgress )
    {
        m_uploadAction->setText( tr("Stop Upload") );
        m_uploadAction->setIcon( QIcon::fromTheme("process-stop") );
    }
    else
    {
        m_uploadAction->setText( tr("Start Upload") );
        m_uploadAction->setIcon( QIcon::fromTheme("go-up") );
        m_trayIcon->setToolTip( tr( "Bitspace Loader: Waiting" ) );
    }
}

void MainWindow::slotAbortUpload()
{
    qDebug() << "MainWindow::slotAbortUpload()";
    m_uploadInProgress = false;

    QStringList inprogress_files = m_model->getInProgress();
    foreach(QString file, inprogress_files)
    {
        // change the status for the item
        QModelIndex index = m_model->indexOf( file );
        m_model->setData( index, Bitspace::Pending, Qt::EditRole );
    }
    setUploadIcon();
    emit abort();
}

void MainWindow::slotItemsChanged()
{
    if( m_model->rowCount() > 0 )
        m_uploadAction->setEnabled( true );
    else
        m_uploadAction->setEnabled( false );
}
