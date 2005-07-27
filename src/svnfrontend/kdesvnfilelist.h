/***************************************************************************
 *   Copyright (C) 2005 by Rajko Albrecht   *
 *   ral@alwins-world.de   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KDESVNFILELIST_H
#define KDESVNFILELIST_H

#include "filelistviewitem.h"
#include "svncpp/status.hpp"
#include "svncpp/client.hpp"

#include <klistview.h>
#include <kurl.h>
#include <qmap.h>
#include <qptrlist.h>

class KAction;
class KActionMenu;
class KActionCollection;
class KDialog;
class DirNotify;
class KdesvnFileListPrivate;

/**
@author Rajko Albrecht
*/
class kdesvnfilelist : public KListView
{
    Q_OBJECT
    friend class FileListViewItem;
    friend class SvnActions;
public:
    kdesvnfilelist(QWidget *parent = 0, const char *name = 0);
    virtual ~kdesvnfilelist();

    bool openURL( const KURL &url,bool noReinit=false );
    const QString&lastError()const{return m_LastException;}
    const svn::Status&maindir()const{return m_mainEntry;}
    const QString&baseUri()const{return m_baseUri;}
    bool isLocal()const{return m_isLocal;}

    KActionCollection*filesActions();

protected:
    svn::Status m_mainEntry;
    bool m_isLocal;

    QString m_LastException,m_baseUri;
    QMap<QString,bool> m_Dirsread;

    KActionCollection* m_filesAction;
    KAction*m_LogFullAction,*m_LogRangeAction,*m_BlameAction,*m_BlameRangeAction,*m_CatAction,*m_MkdirAction;
    KAction*m_InfoAction,*m_propertyAction,*m_commitAction,*m_simpleDiffHead,*m_UpdateHead,*m_UpdateRev;
    KAction*m_AddCurrent,*m_DelCurrent,*m_CheckoutAction,*m_CheckoutCurrentAction,*m_RevertAction;
    KAction*m_changeToRepository,*m_switchRepository,*m_ExportAction,*m_ExportCurrentAction;
    KAction*m_CleanupAction,*m_ResolvedAction,*m_ImportDirsIntoCurrent,*m_RefreshViewAction,*m_MergeRevisionAction;

    SvnActions*m_SvnWrapper;

    /* the parent entry must removed from list before */
    void insertDirs(FileListViewItem * _parent,svn::StatusEntries&);
    bool checkDirs(const QString&,FileListViewItem * _parent);
    void setupActions();
    svn::Client*svnclient();

    void enableActions();

    FileListViewItem* singleSelected();
    FileListViewItemList* allSelected();

    template<class T> KDialog* createDialog(T**ptr,const QString&_head,bool OkCance=false);

    FileListViewItemList* m_SelectedItems;
    virtual void refreshRecursive(FileListViewItem*);
    DirNotify*m_DirNotify;

    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void contentsDragEnterEvent( QDragEnterEvent* );
    virtual void contentsDragLeaveEvent( QDragLeaveEvent* );
    virtual void contentsDragMoveEvent( QDragMoveEvent* );
    virtual void contentsDropEvent( QDropEvent* );
    virtual bool acceptDrag(QDropEvent *event)const;
private:
    KdesvnFileListPrivate*m_pList;
    void cleanHighLighter();

protected slots:
    virtual void slotItemClicked(QListViewItem*);
    virtual void slotRightButton(QListViewItem *, const QPoint &, int);

    virtual void slotSelectionChanged();
    virtual void slotClientException(const QString&);
    virtual void slotNotifyMessage(const QString&);
    virtual void slotDirAdded(const QString&,FileListViewItem*);
    virtual void slotReinitItem(FileListViewItem*);
    virtual void slotItemDoubleClicked(QListViewItem*);
    virtual void slotImportIntoCurrent(bool);
    virtual void slotImportDirsIntoCurrent();
    virtual void slotImportIntoDir(const KURL&,const QString&,bool);

    /* subversion slots */
    virtual void slotChangeToRepository();
    virtual void slotCleanupAction();
    virtual void slotResolved();
    virtual void slotMergeRevisions();
    virtual void slotDropped(QDropEvent *,QListViewItem*);
    virtual void viewportPaintEvent(QPaintEvent *);

signals:
    void sigLogMessage(const QString&);
    void changeCaption(const QString&);
    void sigShowPopup(const QString&);
    void sigUrlOpend(bool);

public slots:
    virtual void refreshCurrentTree();
    virtual void refreshCurrent(FileListViewItem*);
    virtual void closeMe();
};

#endif
