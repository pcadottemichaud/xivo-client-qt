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

#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__

#include <QtGui>

#include "xletlib_export.h"
#include "basepeerwidget.h"
#include "taintedpixmap.h"
#include "chitchat.h"

class UserInfo;
class ChitchatButton;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class XLETLIB_EXPORT PeerWidget : public BasePeerWidget
{
    Q_OBJECT

    public:
        PeerWidget(const UserInfo *);
        void setName(const QString &);
        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);
        void removePhoneConfig(const QString &);
        void setMobileState(const QString &color);
        void updatePresence();  //!< update presence information displayed
        void updateAgentToolTip();
    public slots:
        void updateAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void updateQueueMemberConfig(const QString &);
        void removeQueueMemberConfig(const QString &);
    protected:
        void updateChitChatButton();
    private:
        QString m_xagentid;
        QHash<QString, QLabel *> m_lblphones; //!< phone labels

        ChitchatButton *m_user_status;  //!< Peer state display from XiVO CTI Client
        QLabel *m_agentlbl;       //!< agent state label
        QLabel *m_mobilelbl;      //!< mobile phone label
        QLabel *m_textlbl;        //!< text label : to display peer name
        QHBoxLayout * m_hLayout;
        QWidget * m_peer;
        static const int m_iconsize = 25;

    public:
        static const unsigned max_width = 200;
};

class ChitchatButton : public QPushButton
{
    Q_OBJECT

    public:
        ChitchatButton(QWidget *parent, const UserInfo * * m_ui);

    protected:
        bool event(QEvent *event);

    private:
        const UserInfo **m_ui;

};

#endif
