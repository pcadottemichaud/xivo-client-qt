/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CHANNELINFO_H__
#define __CHANNELINFO_H__

#include "baselib_export.h"
#include "xinfo.h"

class BASELIB_EXPORT ChannelInfo : public XInfo
{
    public:
        ChannelInfo(const QString &, const QString &);
        bool updateStatus(const QVariantMap &);

        const QString & talkingto_kind() const;
        const QString & talkingto_id() const;
        const QString & direction() const;
        const QString & commstatus() const;
        double timestamp() const;
        int linenumber() const;
        bool isholded() const;
        bool canBeTransferred() const;
        bool isTalking() const;

    private:

        QString m_direction;
        QString m_commstatus;
        QString m_talkingto_kind;
        QString m_talkingto_id;
        QString m_state;
        double m_timestamp;

        int m_linenumber;
        bool m_isholded;
};

#endif
