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
#ifndef REVISIONBUTTONIMPL_H
#define REVISIONBUTTONIMPL_H

#include "ui_revisionbutton.h"
#include "svnqt/revision.hpp"

class RevisionButtonImpl: public QWidget, public Ui::RevisionButton {
    Q_OBJECT

public:
    RevisionButtonImpl(QWidget *parent = 0, const char *name = 0);
    virtual ~RevisionButtonImpl();

    virtual void setRevision(const svn::Revision&aRev);

    virtual void setNoWorking(bool);
    const svn::Revision& revision()const {
        return m_Rev;
    }

protected:
    svn::Revision m_Rev;
    bool m_noWorking;

public slots:
    virtual void askRevision();
signals:
    void revisionChanged();

};

#endif