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

// svncpp
#include "status.h"
#include "svnqt/svnqt_defines.h"
#include "svnqt/path.h"
#include "svnqt/url.h"

#include "svn_path.h"

namespace svn
{
class SVNQT_NOEXPORT Status_private
{
public:
    Status_private();
    virtual ~Status_private();
    /**
     * Initialize structures
     *
     * @param path
     * @param status if NULL isVersioned will be false
     */
#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
    void init(const QString &path, const svn_client_status_t *status);
#else
    void init(const QString &path, const svn_wc_status2_t *status);
#endif
    void
    init(const QString &path, const Status_private &src);
    void
    init(const QString &url, const DirEntry &src);
    void
    init(const QString &url, const InfoEntry &src);

    void setPath(const QString &);

    QString m_Path;
    bool m_isVersioned;
    bool m_hasReal;
    LockEntry m_Lock;
    Entry m_entry;

    svn_wc_status_kind m_text_status, m_prop_status, m_repos_text_status, m_repos_prop_status;
    bool m_copied, m_switched;
};

Status_private::Status_private()
    : m_Path()
    , m_isVersioned(false)
    , m_hasReal(false)
    , m_text_status(svn_wc_status_none)
    , m_prop_status(svn_wc_status_none)
    , m_repos_text_status(svn_wc_status_none)
    , m_repos_prop_status(svn_wc_status_none)
    , m_copied(false)
    , m_switched(false)
{
}

Status_private::~ Status_private()
{
}

void Status_private::setPath(const QString &aPath)
{
    Pool pool;
    if (!Url::isValid(aPath)) {
        m_Path = aPath;
    } else {
        const char *int_path = svn_path_uri_decode(aPath.toUtf8(), pool.pool());
        m_Path = QString::fromUtf8(int_path);
    }
}

#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
void Status_private::init(const QString &path, const svn_client_status_t *status)
#else
void Status_private::init(const QString &path, const svn_wc_status2_t *status)
#endif
{
    setPath(path);
    if (!status) {
        m_isVersioned = false;
        m_hasReal = false;
        m_entry = Entry();
        m_Lock = LockEntry();
    } else {
        m_isVersioned = status->text_status > svn_wc_status_unversioned || status->repos_text_status > svn_wc_status_unversioned;
        m_hasReal = m_isVersioned &&
                    status->text_status != svn_wc_status_ignored;
        // now duplicate the contents
#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
        m_entry = Entry(status);
#else
        if (status->entry) {
            m_entry = Entry(status->entry);
        } else {
            m_entry = Entry();
        }
#endif
        m_text_status = status->text_status;
        m_prop_status = status->prop_status;
        m_copied = status->copied != 0;
        m_switched = status->switched != 0;
        m_repos_text_status = status->repos_text_status;
        m_repos_prop_status = status->repos_prop_status;
        if (status->repos_lock) {
            m_Lock.init(status->repos_lock->creation_date,
                        status->repos_lock->expiration_date,
                        status->repos_lock->owner,
                        status->repos_lock->comment,
                        status->repos_lock->token);
        } else {
            m_Lock = LockEntry();
        }
    }
}

void
Status_private::init(const QString &path, const Status_private &src)
{
    setPath(path);
    m_Lock = src.m_Lock;
    m_entry = src.m_entry;
    m_isVersioned = src.m_isVersioned;
    m_hasReal = src.m_hasReal;
    m_text_status = src.m_text_status;
    m_prop_status = src.m_prop_status;
    m_repos_text_status = src.m_repos_text_status;
    m_repos_prop_status = src.m_repos_prop_status;
    m_copied = src.m_copied;
    m_switched = src.m_switched;
}

void Status_private::init(const QString &url, const DirEntry &src)
{
    m_entry = Entry(url, src);
    setPath(url);
    m_text_status = svn_wc_status_normal;
    m_prop_status = svn_wc_status_normal;
    if (!src.isEmpty()) {
        m_Lock = src.lockEntry();
        m_isVersioned = true;
        m_hasReal = true;
    }
    m_switched = false;
    m_repos_text_status = svn_wc_status_normal;
    m_repos_prop_status = svn_wc_status_normal;
}

void Status_private::init(const QString &url, const InfoEntry &src)
{
    m_entry = Entry(url, src);
    setPath(url);
    m_Lock = src.lockEntry();
    m_text_status = svn_wc_status_normal;
    m_prop_status = svn_wc_status_normal;
    m_repos_text_status = svn_wc_status_normal;
    m_repos_prop_status = svn_wc_status_normal;
    m_isVersioned = true;
    m_hasReal = true;
}

Status::Status(const Status &src)
    : m_Data(new Status_private())
{
    if (&src != this) {
        if (src.m_Data) {
            m_Data->init(src.m_Data->m_Path, *(src.m_Data));
        } else {
            m_Data->init(src.m_Data->m_Path, 0);
        }
    }
}

#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
Status::Status(const char *path, const svn_client_status_t *status)
  : m_Data(new Status_private())
{
    m_Data->init(path, status);
}
#else
Status::Status(const char *path, const svn_wc_status2_t *status)
  : m_Data(new Status_private())
{
    m_Data->init(path, status);
}
#endif
Status::Status(const QString &path)
    : m_Data(new Status_private())
{
    m_Data->init(path, NULL);
}

Status::Status(const QString &url, const DirEntry &src)
    : m_Data(new Status_private())
{
    m_Data->init(url, src);
}

Status::Status(const QString &url, const InfoEntry &src)
    : m_Data(new Status_private())
{
    m_Data->init(url, src);
}

Status::~Status()
{
    delete m_Data;
}

Status &
Status::operator=(const Status &status)
{
    if (this == &status) {
        return *this;
    }
    if (status.m_Data) {
        m_Data->init(status.m_Data->m_Path, *(status.m_Data));
    } else {
        m_Data->init(status.m_Data->m_Path, 0);
    }
    return *this;
}

const LockEntry &
Status::lockEntry() const
{
    return m_Data->m_Lock;
}
svn_wc_status_kind
Status::reposPropStatus() const
{
    return m_Data->m_repos_prop_status;
}
svn_wc_status_kind
Status::reposTextStatus() const
{
    return m_Data->m_repos_text_status;
}
bool
Status::isSwitched() const
{
    return m_Data->m_switched != 0;
}
bool
Status::isCopied() const
{
    return m_Data->m_copied;
}

bool
Status::isLocked() const
{
    return m_Data->m_Lock.Locked();
}

bool
Status::isModified()const
{
    return textStatus() == svn_wc_status_modified || propStatus() == svn_wc_status_modified
           || textStatus() == svn_wc_status_replaced;
}

bool
Status::isRealVersioned()const
{
    return m_Data->m_hasReal;
}

bool
Status::isVersioned() const
{
    return m_Data->m_isVersioned;
}

svn_wc_status_kind
Status::propStatus() const
{
    return m_Data->m_prop_status;
}

svn_wc_status_kind
Status::textStatus() const
{
    return m_Data->m_text_status;
}

const Entry &
Status::entry() const
{
    return m_Data->m_entry;
}

const QString &
Status::path() const
{
    return m_Data->m_Path;
}

bool
Status::validReposStatus()const
{
    return reposTextStatus() != svn_wc_status_none || reposPropStatus() != svn_wc_status_none;
}

bool
Status::validLocalStatus()const
{
    return textStatus() != svn_wc_status_none || propStatus() != svn_wc_status_none;
}
}
