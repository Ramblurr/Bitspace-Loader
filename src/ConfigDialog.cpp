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
#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

#include <QSettings>
#include <QPushButton>

ConfigDialog::ConfigDialog( QWidget *parent ) :
        QDialog( parent ),
        ui( new Ui::ConfigDialog )
{
    ui->setupUi( this );
    QSettings settings;
    QString token = settings.value( "apitoken", ui->lineEdit->text() ).toString();
    ui->lineEdit->setText( token );
    on_lineEdit_textChanged( token );
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void ConfigDialog::on_lineEdit_textChanged( QString token )
{
    Q_UNUSED( token )
    if ( ui->lineEdit->text().trimmed().isEmpty() )
        ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    else
        ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( true );

}

void ConfigDialog::on_buttonBox_accepted()
{
    QSettings settings;
    settings.setValue( "apitoken", ui->lineEdit->text().trimmed() );
    settings.sync();
}
