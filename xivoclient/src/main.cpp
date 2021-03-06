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

#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QStyle>
#include <QSysInfo>
#include <QTranslator>
#include <QLibraryInfo>

#include <baseengine.h>
#include <phonenumber.h>

#include "assembler.h"
#include "main_window/main_window.h"
#include "powerawareapplication.h"
#include "fileopeneventhandler.h"

#ifdef FUNCTESTS
#include "remote_control/remote_control.h"
#endif

#include "main.h"

const QString &str_socket_arg_prefix = "socket:";

// argc has to be a reference, or QCoreApplication will segfault
ExecObjects init_xivoclient(int & argc, char **argv)
{
    ExecObjects ret;
    QCoreApplication::setOrganizationName("XIVO");
    QCoreApplication::setOrganizationDomain("xivo.fr");
    QCoreApplication::setApplicationName("XIVO_Client");
    PowerAwareApplication *app = new PowerAwareApplication(argc, argv);

    FileOpenEventHandler* fileOpenHandler = new FileOpenEventHandler(app, app);
    app->installEventFilter(fileOpenHandler);

    QSettings * settings = new QSettings(QSettings::IniFormat,
                                         QSettings::UserScope,
                                         QCoreApplication::organizationName(),
                                         QCoreApplication::applicationName());
    qDebug() << "Reading configuration file" << settings->fileName();

    QString profile = "default-user";
    QString msg = "";
    for (int i = 1; i < argc; i ++) {
        QString arg_str(argv[i]);
        if (arg_str.length() == 0 || arg_str.contains(str_socket_arg_prefix)) {
            continue;
        }
        if(! PhoneNumber::isURI(arg_str)) {
            profile = arg_str;
        } else {
            msg = PhoneNumber::extract(arg_str);
        }
    }

    qDebug() << "Selected profile: " << profile;

    if (! msg.isEmpty()) {
        // send message if there is an argument.
        // see http://people.w3.org/~dom/archives/2005/09/integrating-a-new-uris-scheme-handler-to-gnome-and-firefox/
        // to learn how to handle "tel:0123456" uri scheme
        app->sendMessage(msg);
        // warning : this sends the message only to the first instance, if ever there are >1 instances running
    }

    app->setWindowIcon(QIcon(":/images/xivo-login.png"));

    QString info_osname;
    QString info_endianness = QSysInfo::ByteOrder ? "LE" : "BE";

#if defined(Q_WS_X11)
    info_osname = QString("X11-%1-%2")
        .arg(info_endianness)
        .arg(app->applicationPid());
#elif defined(Q_WS_WIN)
    info_osname = QString("WIN-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::WindowsVersion, 2, 16, QChar('0'))
        .arg(app->applicationPid());
#elif defined(Q_WS_MAC)
    info_osname = QString("MAC-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::MacintoshVersion, 2, 16, QChar('0'))
        .arg(app->applicationPid());
#else
    info_osname = QString("unknown-%1-%2")
        .arg(info_endianness)
        .arg(app->applicationPid());
#endif

    bool shallbeunique = settings->value("display/unique").toBool();
    if (shallbeunique && app->isRunning()) {
        qDebug() << Q_FUNC_INFO << "unique mode : application is already running : exiting";
        // do not create a new application, just activate the currently running one
        ret.initOK = false;
        return ret;
    }

    settings->setValue("profile/lastused", profile);

    b_engine = new BaseEngine(settings, info_osname);

    QString qsskind = b_engine->getConfig("qss").toString();

    QFile qssFile(QString(":/%1.qss").arg(qsskind));
    if(qssFile.open(QIODevice::ReadOnly)) {
        app->setStyleSheet(qssFile.readAll());
    }
    assembler = new Assembler();
    if (! assembler) {
        qDebug() << Q_FUNC_INFO << "Failed to instantiate the Assembler";
        return ret;
    }
    MainWindow *main_window = assembler->mainWindow();
    main_window->initialize();

    bool activate_on_tel = b_engine->getConfig("activate_on_tel").toBool();
    app->setActivationWindow(main_window, activate_on_tel);
    fileOpenHandler->setActivationWindow(activate_on_tel);

    app->setQuitOnLastWindowClosed(false);
    app->setProperty("stopper", "lastwindow");

    QObject::connect(app, SIGNAL(standBy()),
                     b_engine, SLOT(stop()));
    QObject::connect(app, SIGNAL(resume()),
                     b_engine, SLOT(start()));
    QObject::connect(app, SIGNAL(powerEvent(const QString &)),
                     b_engine, SLOT(powerEvent(const QString &)));
    QObject::connect(app, SIGNAL(messageReceived(const QString &)),
                     b_engine, SLOT(handleOtherInstanceMessage(const QString &)));
    QObject::connect(fileOpenHandler, SIGNAL(dialNumber(QString)),
                     b_engine, SLOT(handleOtherInstanceMessage(const QString &)));

    ret.app = app;
    ret.win = main_window;
    ret.baseengine = b_engine;
    ret.initOK = true;

#ifdef FUNCTESTS
    QString socket = "/tmp/xc-default.sock";
    for (int i = 1; i < argc; i ++) {
        QString arg_str(argv[i]);
        if (arg_str.contains(str_socket_arg_prefix)) {
            socket = arg_str.replace(str_socket_arg_prefix, "");
        }
    }
    qDebug() << "Selected RC socket: " << socket;

    ret.rc = new RemoteControl(ret, socket);
#endif

    return ret;
}

int run_xivoclient(ExecObjects exec_obj)
{
    bool should_start = exec_obj.initOK == true && exec_obj.app != NULL;
    if (should_start) {
        return exec_obj.app->exec();
    } else {
        return 1;
    }
}

void clean_xivoclient(ExecObjects exec_obj)
{
#ifdef FUNCTESTS
    delete exec_obj.rc;
    exec_obj.rc = NULL;
#endif
    delete assembler;
    assembler = NULL;
    delete exec_obj.app;
    exec_obj.app = NULL;
}

int main(int argc, char **argv)
{
    ExecObjects exec_obj= init_xivoclient(argc, argv);
    int ret = run_xivoclient(exec_obj);
    qDebug() << "Exiting";
    clean_xivoclient(exec_obj);
    return ret;
}
