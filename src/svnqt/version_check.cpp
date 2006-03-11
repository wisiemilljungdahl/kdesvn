#include "version_check.hpp"

#include <svn_version.h>
#include <svn_client.h>

#include <qstring.h>

namespace svn {
    static const svn_version_t Linkedtag = {
        SVN_VER_MAJOR,
        SVN_VER_MINOR,
        SVN_VER_PATCH,
        SVN_VER_NUMTAG
    };

    static QString curr_version_string;

    bool Version::client_version_compatible()
    {
        return svn_ver_compatible(svn_client_version(),&Linkedtag);
    }

    const QString Version::linked_version()
    {
        return QString( SVN_VERSION );
    }

    const QString Version::running_version()
    {
        if (curr_version_string.length()==0) {
            curr_version_string =
                QString("%1.%2.%3.%4").arg(svn_client_version()->major).arg(svn_client_version()->minor)
                    .arg(svn_client_version()->patch).arg(svn_client_version()->tag);
        }
        return curr_version_string;
    }

    int Version::version_major()
    {
      return svn_client_version()->major;
    }

    int Version::version_minor()
    {
      return svn_client_version()->minor;
    }
}