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

#include <QtTest/QtTest>
#include <QObject>
#include <QIcon>
#include <QSystemTrayIcon>

#include <gmock/gmock.h>

#include <application_status_icon_manager.h>

using namespace testing;

class TestApplicationStatusIconManager: public QObject
{
    Q_OBJECT

    private slots:
    void testGetApplicationStatusIconBlack()
    {
        ApplicationStatusIconManager application_status_icon_manager;

        QIcon new_icon = application_status_icon_manager.getApplicationStatusIcon(icon_disconnected);

        QCOMPARE(new_icon.pixmap(1000).toImage(),
                 QIcon(":images/xivoicon-black.png").pixmap(1000).toImage());
    }

    void testGetApplicationStatusIconGreen()
    {
        ApplicationStatusIconManager application_status_icon_manager;

        QIcon new_icon = application_status_icon_manager.getApplicationStatusIcon(icon_agent_logged);

        QCOMPARE(new_icon.pixmap(1000).toImage(),
                 QIcon(":images/xivoicon-green.png").pixmap(1000).toImage());
    }
};

int main (int argc, char *argv[])
{
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);
    QApplication app(argc, argv);
    TestApplicationStatusIconManager test;
    QTest::qExec(&test, argc, argv);
}

#include <test_application_status_icon_manager.moc>
