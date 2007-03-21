/***************************************************************************
 *   Copyright (C) 2005 by Rajko Albrecht                                  *
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
#include "logmsg_impl.h"
#include "src/settings/kdesvnsettings.h"

#include <ktextedit.h>
#include <kcombobox.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kapp.h>
#include <kconfigbase.h>
#include <kconfig.h>

#include <qvbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qpushbutton.h>

#define MAX_MESSAGE_HISTORY 10

QValueList<QString> Logmsg_impl::sLogHistory = QValueList<QString>();
const QString Logmsg_impl::groupName("logmsg_dlg_size");

unsigned int Logmsg_impl::smax_message_history = 0xFFFF;

class SvnCheckListItem:public QCheckListItem
{
protected:
    Logmsg_impl::logActionEntry m_Content;
public:
    SvnCheckListItem(QListView*,const Logmsg_impl::logActionEntry&);
    const Logmsg_impl::logActionEntry&data(){return m_Content;}
    virtual int rtti()const{return 1000;}
};

Logmsg_impl::Logmsg_impl(QWidget *parent, const char *name)
    :LogmessageData(parent, name)
{
    m_LogEdit->setFocus();
    m_Reviewlabel->hide();
    m_ReviewList->hide();
    m_hidden=true;
    hideButtons(true);
    m_MainSplitter->moveToFirst(m_EditFrame);
    delete m_ReviewFrame;
    m_Reviewlabel=0;
    m_ReviewList=0;
    m_MarkUnversioned=0;
    m_UnmarkUnversioned=0;
    m_DiffItem=0;
}

Logmsg_impl::Logmsg_impl(const svn::CommitItemList&_items,QWidget *parent, const char *name)
    :LogmessageData(parent, name)
{
    m_LogEdit->setFocus();
    m_ReviewList->setColumnText(1,i18n("Items to commit"));
    m_ReviewList->setColumnText(0,i18n("Action"));
    m_ReviewList->setSortColumn(1);
    hideButtons(true);
    if (_items.count()>0) {
        for (unsigned i = 0;i<_items.count();++i) {
            QListViewItem*item = new QListViewItem(m_ReviewList);
            if (_items[i].path().isEmpty()) {
                item->setText(1,_items[i].url());
            } else {
                item->setText(1,_items[i].path());
            }
            item->setText(0,QChar(_items[i].actionType()));
        }
        m_hidden=false;
    } else {
        m_Reviewlabel->hide();
        m_ReviewList->hide();
        m_hidden=true;
    }
    checkSplitterSize();
}

Logmsg_impl::Logmsg_impl(const QMap<QString,QString>&_items,QWidget *parent, const char *name)
    :LogmessageData(parent, name)
{
    m_LogEdit->setFocus();
    m_ReviewList->setColumnText(1,i18n("Items to commit"));
    m_ReviewList->setColumnText(0,i18n("Action"));
    m_ReviewList->setSortColumn(1);
    hideButtons(true);
    if (_items.count()>0) {
        QMap<QString,QString>::ConstIterator it = _items.begin();
        for (;it!=_items.end();++it) {
            QListViewItem*item = new QListViewItem(m_ReviewList);
            item->setText(1,it.key());
            item->setText(0,it.data());
        }
        m_hidden=false;
    } else {
        m_Reviewlabel->hide();
        m_ReviewList->hide();
        m_hidden=true;
    }
    checkSplitterSize();
}

Logmsg_impl::Logmsg_impl(const logActionEntries&_activatedList,
        const logActionEntries&_notActivatedList,
        QWidget *parent, const char *name)
    :LogmessageData(parent, name)
{
    m_LogEdit->setFocus();
    m_hidden=false;
    for (unsigned j = 0; j<_activatedList.count();++j) {
        SvnCheckListItem * item = new SvnCheckListItem(m_ReviewList,_activatedList[j]);
        item->setState(QCheckListItem::On);
    }
    for (unsigned j = 0; j<_notActivatedList.count();++j) {
        SvnCheckListItem * item = new SvnCheckListItem(m_ReviewList,_notActivatedList[j]);
        item->setState(QCheckListItem::Off);
    }
    checkSplitterSize();
}

Logmsg_impl::~Logmsg_impl()
{
    QValueList<int> list = m_MainSplitter->sizes();
    if (!m_hidden && list.count()==2) {
        Kdesvnsettings::setCommit_splitter_height(list);
        Kdesvnsettings::writeConfig();
    }
}

void Logmsg_impl::checkSplitterSize()
{
    QValueList<int> list = Kdesvnsettings::commit_splitter_height();
    if (list.count()!=2) {
        return;
    }
    if (m_hidden) {
        list[1]=list[0]+list[1];
        list[0]=0;
    }
    if (m_hidden || (list[0]>0||list[1]>0)) {
        m_MainSplitter->setSizes(list);
    }
}

void Logmsg_impl::slotHistoryActivated(int number)
{
    if (number < 1||(unsigned)number>sLogHistory.size()) {
        m_LogEdit->setText("");
    } else {
        m_LogEdit->setText(sLogHistory[number-1]);
    }
}

/*!
    \fn Logmsg_impl::getMessage()const
 */
QString Logmsg_impl::getMessage()const
{
    return m_LogEdit->text();
}


/*!
    \fn Logmsg_impl::isRecursive()const
 */
bool Logmsg_impl::isRecursive()const
{
    return m_RecursiveButton->isChecked();
}

/*!
    \fn Logmsg_impl::isRecursive()const
 */
bool Logmsg_impl::isKeeplocks()const
{
    return m_keepLocksButton->isChecked();
}


/*!
    \fn Logmsg_impl::initHistory()
 */
void Logmsg_impl::initHistory()
{
    if (smax_message_history==0xFFFF) {
        smax_message_history = Kdesvnsettings::max_log_messages();
        KConfigGroup cs(Kdesvnsettings::self()->config(),"log_messages");
        QString s = QString::null;
        unsigned int current = 0;
        QString key = QString("log_%0").arg(current);
        s = cs.readEntry(key,QString::null);
        while (s!=QString::null) {
            if (current<smax_message_history) {
                sLogHistory.push_back(s);
            } else {
                cs.deleteEntry(key);
            }
            ++current;
            key = QString("log_%0").arg(current);
            s = cs.readEntry(key,QString::null);
        }
    }
    QValueList<QString>::const_iterator it;
    for (it=sLogHistory.begin();it!=sLogHistory.end();++it) {
        if ((*it).length()<=40) {
            m_LogHistory->insertItem((*it));
        } else {
            m_LogHistory->insertItem((*it).left(37)+"...");
        }
    }
}


/*!
    \fn Logmsg_impl::saveHistory()
 */
void Logmsg_impl::saveHistory()
{
    if (m_LogEdit->text().length()==0) return;
    /// @todo make static threadsafe
    QValueList<QString>::iterator it;
    if ( (it=sLogHistory.find(m_LogEdit->text()))!=sLogHistory.end()) {
        sLogHistory.erase(it);
    }
    sLogHistory.push_front(m_LogEdit->text());
    if (sLogHistory.size()>smax_message_history) {
        sLogHistory.erase(sLogHistory.fromLast());
    }
    KConfigGroup cs(Kdesvnsettings::self()->config(),"log_messages");
    for (unsigned int i = 0; i < sLogHistory.size();++i) {
        cs.writeEntry(QString("log_%0").arg(i),sLogHistory[i]);
    }
    cs.sync();
}

QString Logmsg_impl::getLogmessage(bool*ok,bool*rec,bool*keep_locks,QWidget*parent,const char*name)
{
    bool _ok,_rec,_keep_locks;
    QString msg("");

    Logmsg_impl*ptr=0;
    KDialogBase dlg(parent,name,true,i18n("Commit log"),
            KDialogBase::Ok|KDialogBase::Cancel,
            KDialogBase::Ok,true);
    QWidget* Dialog1Layout = dlg.makeVBoxMainWidget();

    ptr = new Logmsg_impl(Dialog1Layout);
    if (!rec) {
        ptr->m_RecursiveButton->hide();
    }
    if (!keep_locks) {
        ptr->m_keepLocksButton->hide();
    }
    ptr->initHistory();
    dlg.resize(dlg.configDialogSize(*(Kdesvnsettings::self()->config()),groupName));
    if (dlg.exec()!=QDialog::Accepted) {
        _ok = false;
        /* avoid compiler warnings */
        _rec = false;
        _keep_locks = false;
    } else {
        _ok = true;
        _rec = ptr->isRecursive();
        _keep_locks = ptr->isKeeplocks();
        msg=ptr->getMessage();
        ptr->saveHistory();
    }
    dlg.saveDialogSize(*(Kdesvnsettings::self()->config()),groupName,false);
    if (ok) *ok = _ok;
    if (rec) *rec = _rec;
    return msg;
}

QString Logmsg_impl::getLogmessage(const svn::CommitItemList&items,bool*ok,bool*rec,bool*keep_locks,QWidget*parent,const char*name)
{
    bool _ok,_rec,_keep_locks;
    QString msg("");

    Logmsg_impl*ptr=0;
    KDialogBase dlg(parent,name,true,i18n("Commit log"),
            KDialogBase::Ok|KDialogBase::Cancel,
            KDialogBase::Ok,true);
    QWidget* Dialog1Layout = dlg.makeVBoxMainWidget();

    ptr = new Logmsg_impl(items,Dialog1Layout);
    if (!rec) {
        ptr->m_RecursiveButton->hide();
    }
    if (!keep_locks) {
        ptr->m_keepLocksButton->hide();
    }

    ptr->initHistory();
    dlg.resize(dlg.configDialogSize(*(Kdesvnsettings::self()->config()),groupName));
    if (dlg.exec()!=QDialog::Accepted) {
        _ok = false;
        /* avoid compiler warnings */
        _rec = false;
        _keep_locks = false;
    } else {
        _ok = true;
        _rec = ptr->isRecursive();
        _keep_locks = ptr->isKeeplocks();
        msg=ptr->getMessage();
        ptr->saveHistory();
    }
    dlg.saveDialogSize(*(Kdesvnsettings::self()->config()),groupName,false);
    if (ok) *ok = _ok;
    if (rec) *rec = _rec;
    if (keep_locks) *keep_locks = _keep_locks;
    return msg;
}

QString Logmsg_impl::getLogmessage(const QMap<QString,QString>&items,
    bool*ok,bool*rec,bool*keep_locks,QWidget*parent,const char*name)
{
    bool _ok,_rec,_keep_locks;
    QString msg("");

    Logmsg_impl*ptr=0;
    KDialogBase dlg(parent,name,true,i18n("Commit log"),
            KDialogBase::Ok|KDialogBase::Cancel,
            KDialogBase::Ok,true);
    QWidget* Dialog1Layout = dlg.makeVBoxMainWidget();

    ptr = new Logmsg_impl(items,Dialog1Layout);
    if (!rec) {
        ptr->m_RecursiveButton->hide();
    }
    if (!keep_locks) {
        ptr->m_keepLocksButton->hide();
    }
    ptr->initHistory();
    dlg.resize(dlg.configDialogSize(*(Kdesvnsettings::self()->config()),groupName));
    if (dlg.exec()!=QDialog::Accepted) {
        _ok = false;
        /* avoid compiler warnings */
        _rec = false;
        _keep_locks=false;
    } else {
        _ok = true;
        _rec = ptr->isRecursive();
        msg=ptr->getMessage();
        ptr->saveHistory();
        _keep_locks = ptr->isKeeplocks();
    }
    dlg.saveDialogSize(*(Kdesvnsettings::self()->config()),groupName,false);
    if (ok) *ok = _ok;
    if (rec) *rec = _rec;
    if (keep_locks) *keep_locks = _keep_locks;
    return msg;
}

QString Logmsg_impl::getLogmessage(const logActionEntries&_on,
            const logActionEntries&_off,
            QObject*callback,
            logActionEntries&_result,
            bool*ok,bool*keep_locks,QWidget*parent,const char*name)
{
    bool _ok,_keep_locks;
    QString msg("");

    Logmsg_impl*ptr=0;
    KDialogBase dlg(parent,name,true,i18n("Commit log"),
            KDialogBase::Ok|KDialogBase::Cancel,
            KDialogBase::Ok,true);
    QWidget* Dialog1Layout = dlg.makeVBoxMainWidget();
    ptr = new Logmsg_impl(_on,_off,Dialog1Layout);
    ptr->m_RecursiveButton->hide();
    if (!keep_locks) {
        ptr->m_keepLocksButton->hide();
    }
    ptr->initHistory();
    if (callback)
    {
        connect(ptr,SIGNAL(makeDiff(const QString&,const svn::Revision&,const QString&,const svn::Revision&,QWidget*)),
                callback,SLOT(makeDiff(const QString&,const svn::Revision&,const QString&,const svn::Revision&,QWidget*)));
    }
    dlg.resize(dlg.configDialogSize(*(Kdesvnsettings::self()->config()),groupName));
    if (dlg.exec()!=QDialog::Accepted) {
        _ok = false;
        /* avoid compiler warnings */
        _keep_locks=false;
    } else {
        _ok = true;
        msg=ptr->getMessage();
        ptr->saveHistory();
        _keep_locks = ptr->isKeeplocks();
    }
    dlg.saveDialogSize(*(Kdesvnsettings::self()->config()),groupName,false);
    if (ok) *ok = _ok;
    _result = ptr->selectedEntries();
    if (keep_locks) *keep_locks = _keep_locks;
    return msg;
}

/*!
    \fn Logmsg_impl::setRecCheckboxtext(const QString&what)
 */
void Logmsg_impl::setRecCheckboxtext(const QString&what,bool checked)
{
    m_RecursiveButton->setText(what);
    m_RecursiveButton->setChecked(checked);
}

Logmsg_impl::logActionEntries Logmsg_impl::selectedEntries()
{
    logActionEntries _result;
    if (m_ReviewList) {
        QListViewItemIterator it( m_ReviewList );
        while ( it.current() ) {
            if (it.current()->rtti()==1000) {
                SvnCheckListItem *item = static_cast<SvnCheckListItem*>(it.current());
                if (item->isOn()) {
                    _result.append(item->data());
                }
            }
            ++it;
        }
    }
    return _result;
}

Logmsg_impl::logActionEntry::logActionEntry(const QString&name,const QString&action,int kind)
    : _name(name),_actionDesc(action),_kind(kind)
{
}

Logmsg_impl::logActionEntry::logActionEntry()
    : _name(""),_actionDesc(""),_kind(0)
{
}

SvnCheckListItem::SvnCheckListItem(QListView*parent,const Logmsg_impl::logActionEntry&content)
    :QCheckListItem(parent,content._name,QCheckListItem::CheckBox),m_Content(content)
{
    setTristate(FALSE);
    setText(1,m_Content._actionDesc);
}

void Logmsg_impl::slotUnmarkUnversioned()
{
    markUnversioned(false);
}

void Logmsg_impl::slotMarkUnversioned()
{
    markUnversioned(true);
}

void Logmsg_impl::slotDiffSelected()
{
    QListViewItem * it=0;
    if (!m_ReviewList || !(it=m_ReviewList->selectedItem()))
    {
        return;
    }
    if (it->rtti()==1000)
    {
        SvnCheckListItem *item = static_cast<SvnCheckListItem*>(it);
        QString what = item->data()._name;
        emit makeDiff(what,svn::Revision::BASE,what,svn::Revision::WORKING,parentWidget());
    }
}

void Logmsg_impl::hideButtons(bool how)
{
    if (!m_MarkUnversioned)return;
    if (how)
    {
        m_MarkUnversioned->hide();
        m_UnmarkUnversioned->hide();
        m_DiffItem->hide();
    }
    else
    {
        m_MarkUnversioned->show();
        m_UnmarkUnversioned->show();
        m_DiffItem->show();
    }
}

/*!
    \fn Logmsg_impl::markUnversioned(bool mark)
 */
void Logmsg_impl::markUnversioned(bool mark)
{
    if (!m_ReviewList)return;
    QListViewItemIterator it( m_ReviewList );
    while ( it.current() ) {
        if (it.current()->rtti()==1000) {
            SvnCheckListItem *item = static_cast<SvnCheckListItem*>(it.current());
            if (item->data()._kind==1) {
                item->setOn(mark);
            }
        }
        ++it;
    }
}

#include "logmsg_impl.moc"
