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

#include <QGridLayout>
#include <QPixmap>
#include <QLabel>

#include <baseengine.h>
#include <storage/userinfo.h>
#include <storage/voicemailinfo.h>

#include "identityvoicemail.h"

/*! \brief Constructor
 */
IdentityVoiceMail::IdentityVoiceMail(QWidget * parent)
    : QWidget(parent), m_initialized(false), m_voicemailinfo(NULL)
{
    m_layout = new QGridLayout( this );

    m_iconButton = new QPushButton(this);
    m_iconButton->setFocusPolicy(Qt::NoFocus);
    m_iconButton->setToolTip(tr("call your voicemail"));
    QPixmap icon = QPixmap(":/images/identity/kthememgr.png");
    m_iconButton->setObjectName("voicemail_button");
    m_iconButton->setIcon(icon);
    m_iconButton->setFlat(true);
    m_iconButton->setIconSize(icon.size());
    m_iconButton->setEnabled(false);
    m_layout->addWidget(m_iconButton, 0, 0, 3, 1, Qt::AlignHCenter|Qt::AlignTop);
    connect(m_iconButton, SIGNAL(clicked()), this, SLOT(callVoiceMail()));

    m_name = new QLabel(this);
    m_name->setObjectName("voicemail_num");
    m_layout->addWidget(m_name, 0, 1, Qt::AlignLeft|Qt::AlignVCenter);

    m_old = new QLabel(this);
    m_layout->addWidget(m_old, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);

    m_new = new QLabel(this);
    m_layout->addWidget(m_new, 2, 1, Qt::AlignLeft|Qt::AlignVCenter);
    m_layout->setColumnStretch(2, 1);
}

void IdentityVoiceMail::setVoiceMailId(const QString & xvoicemailid)
{
    m_xvoicemailid = xvoicemailid;
    m_voicemailinfo = b_engine->voicemail(xvoicemailid);
}

/*! \brief update voicemail box name.
 */
void IdentityVoiceMail::svcSummary(QVariantMap &svcstatus, const UserInfo * ui)
{
    bool has_phone = (ui && ui->phonelist().size());
    m_iconButton->setEnabled(has_phone);
    if (! m_voicemailinfo && ui) {
        setVoiceMailId(ui->xvoicemailid());
        updateVoiceMailStatus(m_xvoicemailid);
    }
    if (m_voicemailinfo == NULL)
        return;
    if(svcstatus["enablevoicemail"].toBool()) {
        m_name->setText(tr("<b>VoiceMailBox %1</b>").arg(m_voicemailinfo->mailbox()));
        m_name->setToolTip(tr("VoiceMail activated on %1").arg(m_voicemailinfo->mailbox()));
    } else {
        m_name->setText(tr("VoiceMailBox %1").arg(m_voicemailinfo->mailbox()));
        m_name->setToolTip(tr("VoiceMail not activated on %1").arg(m_voicemailinfo->mailbox()));
    }
}

void IdentityVoiceMail::callVoiceMail()
{
    if (! m_voicemailinfo) {
        return;
    }

    b_engine->actionDial(QString("vm_consult:%1").arg(m_voicemailinfo->xid()));
}

/*! \brief Send an event to get complete voicemail status */
void IdentityVoiceMail::queryVM()
{
    if (! m_initialized && m_voicemailinfo) {
        QVariantMap command;
        command["command"] = "mailboxcount";
        command["mailbox"] = m_voicemailinfo->mailbox();
        command["context"] = m_voicemailinfo->context();
        b_engine->ipbxCommand(command);
        m_initialized = true;
    }
}

void IdentityVoiceMail::updateVoiceMailConfig(const QString & xvoicemailid)
{
    if (xvoicemailid != m_xvoicemailid)
        return;
    m_voicemailinfo = b_engine->voicemail(xvoicemailid);
    if (m_voicemailinfo == NULL)
        return;
    queryVM();
    m_name->setText(tr("VoiceMailBox %1").arg(m_voicemailinfo->mailbox()));
}

void IdentityVoiceMail::updateVoiceMailStatus(const QString & xvoicemailid)
{
    if (xvoicemailid != m_xvoicemailid)
        return;
    if (m_voicemailinfo == NULL)
        return;
    m_old->setText(tr("%1 old").arg(m_voicemailinfo->oldMessages()));
    m_new->setText(tr("%1 new").arg(m_voicemailinfo->newMessages()));
}
