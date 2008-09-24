/***************************************************************************
 *   Copyright (C) 2005-2007 by Rajko Albrecht                             *
 *   ral@alwins-world.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "src/settings/kdesvnsettings.h"
#include "ssltrustprompt_impl.h"

#include <klocale.h>
#include <qlabel.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kapplication.h>
#include <kconfigbase.h>
#include <kconfig.h>
#include <KDialog>
#include <KVBox>
#include <ktextbrowser.h>

SslTrustPrompt_impl::SslTrustPrompt_impl(const QString&host,QWidget *parent, const char *name)
//     :SslTrustPrompt(parent, name)
    : QWidget(parent)
{
    setupUi(this);
    setObjectName(name);

    m_MainLabel->setText("<p align=\"center\"><b>"+
        i18n("Error validating server certificate for '%1'").arg(host)+
        QString("</b></p>"));
}


/*!
    \fn SslTrustPrompt_impl::sslTrust(const QString&host,const QString&fingerprint,const QString&validFrom,const QString&validUntil,const QString&issuerName,const QString&realm,bool*ok,bool*saveit)
 */
bool SslTrustPrompt_impl::sslTrust(const QString&host,const QString&fingerprint,const QString&validFrom,const QString&validUntil,const QString&issuerName,const QString&realm,const QStringList&reasons,bool*ok,bool*saveit)
{
    SslTrustPrompt_impl*ptr=0;
//     KDialogBase dlg(i18n("Trust ssl certificate"));
    KDialog dlg(0);
    dlg.setCaption(i18n("Trust ssl certificate"));
    dlg.setButtonText(KDialog::Yes,i18n("Accept permanently"));
    dlg.setButtonText(KDialog::No,i18n("Accept temporarily"));
//     dlg.setButtonCancel(KGuiItem(i18n("Reject")));
    dlg.setButtonText(KDialog::Cancel, i18n("Reject"));

    static QString rb = "<tr><td>";
    static QString rs = "</td><td>";
    static QString re = "</td></tr>";
    QString text = "<html><body>";
    if (reasons.count()>0) {
        text+="<p align=\"center\">";
        text+="<h2>"+i18n("Failure reasons")+"</h2><hline>";
        for (int i = 0; i < reasons.count();++i) {
            text+=reasons[i]+"<br><hline>";
        }
        text+="</p>";
    }

    text+="<p align=\"center\"><table>";
    text+=rb+i18n("Realm")+rs+realm+re;
    text+=rb+i18n("Host")+rs+host+re;
    text+=rb+i18n("Valid from")+rs+validFrom+re;
    text+=rb+i18n("Valid until")+rs+validUntil+re;
    text+=rb+i18n("Issuer name")+rs+issuerName+re;
    text+=rb+i18n("Fingerprint")+rs+fingerprint+re;
    text+="</table></p></body></html>";

//     QWidget* Dialog1Layout = dlg.makeVBoxMainWidget();
    KVBox *Dialog1Layout = new KVBox(&dlg);
    dlg.setMainWidget(Dialog1Layout);

// KDE4 port - pv    dlg.resize(dlg.configDialogSize(*(Kdesvnsettings::self()->config()),"trustssldlg"));
    ptr = new SslTrustPrompt_impl(host,Dialog1Layout);
    ptr->m_ContentText->setText(text);
    int i = dlg.exec();
// KDE4 port - pv    dlg.saveDialogSize(*(Kdesvnsettings::self()->config()),"trustssldlg",false);
    *saveit = false;
    *ok = true;
    if (i == KDialog::Yes) {
        *saveit = true;
    } else if (i==KDialog::Cancel) {
        *ok = false;
    }
    return *ok;
}

#include "ssltrustprompt_impl.moc"

