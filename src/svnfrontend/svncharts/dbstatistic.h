/***************************************************************************
 *   Copyright (C) 2006-2010 by Rajko Albrecht                             *
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
#ifndef DB_STATISTIC_H
#define DB_STATISTIC_H

#include "src/svnqt/cache/ReposLog.h"

#include <QMap>

class SimpleChartModel;

class DbStatistic
{

    protected:
        QString _reposName;

    public:
        typedef QMap<QString,unsigned> Usermap;
        DbStatistic(const QString&reposName);
        virtual ~DbStatistic();

        SimpleChartModel*getUserCommits()const;
        bool getUserCommits(Usermap&)const;
        const QString&repository()const{return _reposName;}
        void repository(const QString&repository){_reposName = repository;}
};

#endif