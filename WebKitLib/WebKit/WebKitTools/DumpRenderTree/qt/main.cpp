/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Trolltech ASA
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "DumpRenderTree.h"
#include "testplugin.h"

#include <qstringlist.h>
#include <qapplication.h>
#include <qurl.h>
#include <qdir.h>
#include <qdebug.h>
#include <qfont.h>

#ifdef Q_WS_X11
#include <qx11info_x11.h>
#include <fontconfig/fontconfig.h>
#endif

#include <limits.h>
#include <signal.h>

#if defined(__GLIBC__)
#include <execinfo.h>
#endif

#if QT_VERSION < 0x040400
Q_IMPORT_PLUGIN(testplugin)
#endif

void messageHandler(QtMsgType type, const char *message)
{
    if (type == QtCriticalMsg) {
        fprintf(stderr, "%s\n", message);
        return;
    }
    // do nothing
}

QString get_backtrace() {
    QString s;

#if defined(__GLIBC__)
    void* array[256];
    size_t size; /* number of stack frames */

    size = backtrace(array, 256);

    if (!size)
        return s;

    char** strings = backtrace_symbols(array, size);
    for (int i = 0; i < size; ++i) {
        s += QString::number(i) +
             QLatin1String(": ") +
             QLatin1String(strings[i]) + QLatin1String("\n");
    }

    if (strings)
        free (strings);
#endif

    return s;
}

static void crashHandler(int sig)
{
    fprintf(stderr, "%s\n", strsignal(sig));
    fprintf(stderr, "%s\n", get_backtrace().toLatin1().constData());
    exit(128 + sig);
}

int main(int argc, char* argv[])
{
#ifdef Q_WS_X11
    FcInit();
    FcConfig *config = FcConfigCreate();
    QByteArray fontDir = getenv("WEBKIT_TESTFONTS");
    if (fontDir.isEmpty() || !QDir(fontDir).exists()) {
        fprintf(stderr,
                "\n\n"
                "--------------------------------------------------------------------\n"
                "WEBKIT_TESTFONTS environment variable is not set correctly.\n"
                "This variable has to point to the directory containing the fonts\n"
                "you can checkout from svn://labs.trolltech.com/svn/webkit/testfonts\n"
                "--------------------------------------------------------------------\n"
);
        exit(1);
    }
    char currentPath[PATH_MAX+1];
    getcwd(currentPath, PATH_MAX);
    QByteArray configFile = currentPath;
    configFile += "/WebKitTools/DumpRenderTree/qt/fonts.conf";
    if (!FcConfigParseAndLoad (config, (FcChar8*) configFile.data(), true))
        qFatal("Couldn't load font configuration file");
    if (!FcConfigAppFontAddDir (config, (FcChar8*) fontDir.data()))
        qFatal("Couldn't add font dir!");
    FcConfigSetCurrent(config);
#endif
    QApplication app(argc, argv);
#ifdef Q_WS_X11
    QX11Info::setAppDpiY(0, 96);
    QX11Info::setAppDpiX(0, 96);
#endif

    QFont f("Sans Serif");
    f.setPointSize(9);
    f.setWeight(QFont::Normal);
    f.setStyle(QFont::StyleNormal);
    app.setFont(f);
    app.setStyle(QLatin1String("Plastique"));


    signal(SIGILL, crashHandler);    /* 4:   illegal instruction (not reset when caught) */
    signal(SIGTRAP, crashHandler);   /* 5:   trace trap (not reset when caught) */
    signal(SIGFPE, crashHandler);    /* 8:   floating point exception */
    signal(SIGBUS, crashHandler);    /* 10:  bus error */
    signal(SIGSEGV, crashHandler);   /* 11:  segmentation violation */
    signal(SIGSYS, crashHandler);    /* 12:  bad argument to system call */
    signal(SIGPIPE, crashHandler);   /* 13:  write on a pipe with no reader */
    signal(SIGXCPU, crashHandler);   /* 24:  exceeded CPU time limit */
    signal(SIGXFSZ, crashHandler);   /* 25:  exceeded file size limit */

    QStringList args = app.arguments();
    if (args.count() < 2) {
        qDebug() << "Usage: DumpRenderTree [-v] filename";
        exit(0);
    }

    // supress debug output from Qt if not started with -v
    if (!args.contains(QLatin1String("-v")))
        qInstallMsgHandler(messageHandler);

    WebCore::DumpRenderTree dumper;

    if (args.last() == QLatin1String("-")) {
        dumper.open();
    } else {
        if (!args.last().startsWith("/")
            && !args.last().startsWith("file:")) {
            QString path = QDir::currentPath();
            if (!path.endsWith('/'))
                path.append('/');
            args.last().prepend(path);
        }
        dumper.open(QUrl(args.last()));
    }
    return app.exec();
#ifdef Q_WS_X11
    FcConfigSetCurrent(0);
#endif
}
