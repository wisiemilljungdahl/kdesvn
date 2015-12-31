/*
 * Port for usage with qt-framework and development for kdesvn
 * Copyright (C) 2005-2009 by Rajko Albrecht (ral@alwins-world.de)
 * http://kdesvn.alwins-world.de
 */
/*
 * ====================================================================
 * Copyright (c) 2002-2005 The RapidSvn Group.  All rights reserved.
 * dev@rapidsvn.tigris.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library (in the file LGPL.txt); if not,
 * write to the Free Software Foundation, Inc., 51 Franklin St,
 * Fifth Floor, Boston, MA  02110-1301  USA
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://rapidsvn.tigris.org/.
 * ====================================================================
 */

#include "targets.h"

// subversion api
#include <svn_types.h>

// apr api
#include <apr_pools.h>
#include <apr_strings.h>

// svncpp
#include "path.h"
#include "pool.h"
#include "svnqt_defines.h"

#include <QStringList>

namespace svn
{
Targets::Targets(const svn::Paths &targets)
    : m_targets(targets)
{
}

Targets::Targets(const Targets &other)
    : m_targets(other.targets())
{
}

Targets::Targets(const QString &target)
{
    if (!target.isEmpty()) {
        m_targets.push_back(target);
    }
}

Targets::Targets(const Path &target)
{
    if (!target.cstr().isEmpty()) {
        m_targets.push_back(target);
    }
}

Targets::~Targets()
{
}

apr_array_header_t *
Targets::array(const Pool &pool) const
{
    Paths::const_iterator it;

    apr_pool_t *apr_pool = pool.pool();
    apr_array_header_t *apr_targets =
        apr_array_make(apr_pool,
                       m_targets.size(),
                       sizeof(const char *));

    for (it = m_targets.begin(); it != m_targets.end(); ++it) {
        QByteArray s = (*it).path().toUtf8();

        char *t2 =
            apr_pstrndup(apr_pool, s, s.size());

        (*((const char **) apr_array_push(apr_targets))) = t2;
    }

    return apr_targets;
}

const Paths &
Targets::targets() const
{
    return m_targets;
}

size_t
Targets::size() const
{
    return m_targets.size();
}

const Path &Targets::operator [](size_t which)const
{
    return m_targets[which];
}

const Path
Targets::target(Paths::size_type which) const
{
    if (m_targets.size() > which) {
        return m_targets[which];
    } else {
        return Path();
    }
}

}
