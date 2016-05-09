/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Rajko Albrecht <ral@alwins-world.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "svnqt/reposnotify.h"
#include "svnqt/svnqt_defines.h"
#include "svnqt/revision.h"
#include "svnqt/path.h"
#include "svnqt/helper.h"

#include <svn_props.h>

#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
#include <svn_repos.h>
#endif

namespace svn
{
namespace repository
{

class ReposNotifyData
{
    QString _warning_msg;

#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
    /// TODO own datatype
    svn_repos_notify_action_t _action;
    svn::Revision _rev;
    /// TODO own datatype
    svn_repos_notify_warning_t _warning;
    qlonglong _shard;

    svn::Revision _oldrev;
    svn::Revision _newrev;

    /// TODO own datatype
    svn_node_action _node_action;

    svn::Path _path;
#endif

    mutable QString _msg;

public:
    ReposNotifyData(const svn_repos_notify_t *notify)
        : _warning_msg(QString()), _msg(QString())
    {
#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
        if (!notify) {
            return;
        }
        _action = notify->action;
        _rev = notify->revision;
        if (notify->warning_str) {
            _warning_msg = QString::fromUtf8(notify->warning_str);
        }
        _warning = notify->warning;
        _shard = notify->shard;
        _oldrev = notify->old_revision;
        _newrev = notify->new_revision;
        _node_action = notify->node_action;
        if (notify->path != 0L) {
            _path = svn::Path(QString::fromUtf8(notify->path));
        }
#endif
    }

    ~ReposNotifyData()
    {
    }

    const QString &toString()const
    {
#if SVN_API_VERSION >= SVN_VERSION_CHECK(1,7,0)
        if (_msg.isEmpty()) {
            switch (_action) {
            case svn_repos_notify_warning: {
                switch (_warning) {
                case svn_repos_notify_warning_found_old_reference:
                    _msg = QLatin1String("Old Reference: ");
                    break;
                case svn_repos_notify_warning_found_old_mergeinfo:
                    _msg = QLatin1String("Old mergeinfo found: ");
                    break;
                case svn_repos_notify_warning_invalid_fspath:
                    _msg = QLatin1String("Invalid path: ");
                    break;
                default:
                    _msg.clear();
                }
                _msg += _warning_msg;
            }
            break;
            case svn_repos_notify_dump_rev_end:
            case svn_repos_notify_verify_rev_end: {
                _msg = QLatin1String("Revision ") + _rev.toString() + QLatin1String(" finished.");
            }
            break;
            case svn_repos_notify_dump_end: {
                _msg = QLatin1String("Dump finished");
            }
            break;
            case svn_repos_notify_verify_end: {
                _msg = QLatin1String("Verification finished");
            }
            break;
            case svn_repos_notify_pack_shard_start: {
                _msg = QString(QLatin1String("Packing revisions in shard %ul")).arg(_shard);
            }
            break;
            case svn_repos_notify_pack_shard_end_revprop:
            case svn_repos_notify_pack_shard_end:
            case svn_repos_notify_load_node_done: {
                _msg = QLatin1String("Done");
            }
            break;
            case svn_repos_notify_pack_shard_start_revprop: {
                _msg = QString(QLatin1String("Packing revsion properties in shard %ul")).arg(_shard);
            }
            break;
            case svn_repos_notify_load_txn_start: {
                _msg = QLatin1String("Start loading old revision ") + _oldrev.toString();
            }
            break;
            case svn_repos_notify_load_txn_committed: {
                _msg = QLatin1String("Committed new revision ") + _newrev.toString();
                if (_oldrev.isValid()) {
                    _msg.append(QLatin1String(" loaded from original revision ")).append(_oldrev.toString());
                }
            }
            break;
            case svn_repos_notify_load_node_start: {
                QString action;
                switch (_node_action) {
                case svn_node_action_change:
                    action = QLatin1String("changing");
                    break;
                case svn_node_action_add:
                    action = QLatin1String("adding");
                    break;
                case svn_node_action_delete:
                    action = QLatin1String("deletion");
                    break;
                case svn_node_action_replace:
                    action = QLatin1String("replacing");
                    break;
                }
                _msg = QLatin1String("Start ") + action + QLatin1String(" on node ") + _path.native();
            }
            break;
            case svn_repos_notify_load_copied_node: {
                _msg = QLatin1String("Copied");
            }
            break;
            case svn_repos_notify_load_normalized_mergeinfo: {
                _msg = QLatin1String("Removing \\r from ") + QLatin1String(SVN_PROP_MERGEINFO);
            }
            break;
            case svn_repos_notify_mutex_acquired: {
            }
            break;
            case svn_repos_notify_recover_start: {
            }
            break;
            case svn_repos_notify_upgrade_start: {
            }
            break;
            }
        }
#endif
        return _msg;
    }
};

ReposNotify::ReposNotify(const svn_repos_notify_t *notify)
{
    m_data = new ReposNotifyData(notify);
}

ReposNotify::~ReposNotify()
{
    delete m_data;
}

ReposNotify::operator const QString &()const
{
    return m_data->toString();
}

}
}

