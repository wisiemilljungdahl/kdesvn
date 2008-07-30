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
#include "revertform_impl.h"
#include "depthselector.h"

#include <qstringlist.h>
#include <q3listbox.h>


/*!
    \fn RevertFormImpl::RevertFormImpl(QWidget*parent,const char*name)
 */
 RevertFormImpl::RevertFormImpl(QWidget*parent,const char*name)
//     :RevertForm(parent,name)
    : QWidget(parent)
{
    setupUi(this);
    setObjectName(name);

    setMinimumSize(minimumSizeHint());
}

/*!
    \fn RevertFormImpl::~RevertFormImpl()
 */
RevertFormImpl::~RevertFormImpl()
{
    /// @todo implement me
}

svn::Depth RevertFormImpl::getDepth()const
{
    return m_DepthSelect->getDepth();
}


/*!
    \fn RevertFormImpl::setDispList(const QStringList&_list)
 */
void RevertFormImpl::setDispList(const QStringList&_list)
{
    m_ItemsList->insertStringList(_list);
}

#include "revertform_impl.moc"