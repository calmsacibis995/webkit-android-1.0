/*
 * Copyright (C) 2006 Trolltech ASA
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
#ifndef JSOBJECTS_H
#define JSOBJECTS_H

#include <qobject.h>
#include <qdebug.h>
#include <qpoint.h>
#include <qstringlist.h>

class QWebFrame;
namespace WebCore {
    class DumpRenderTree;
}
class LayoutTestController : public QObject
{
    Q_OBJECT
public:
    LayoutTestController(WebCore::DumpRenderTree *drt);

    bool isLoading() const { return m_isLoading; }
    void setLoading(bool loading) { m_isLoading = loading; }

    bool shouldDumpAsText() const { return m_textDump; }
    bool shouldDumpChildrenAsText() const { return m_dumpChildrenAsText; }
    bool shouldWaitUntilDone() const { return m_waitForDone; }
    bool canOpenWindows() const { return m_canOpenWindows; }
    bool shouldDumpTitleChanges() const { return m_dumpTitleChanges; }

    void reset();

protected:
    void timerEvent(QTimerEvent *);

signals:
    void done();

public slots:
    void maybeDump(bool ok);
    void dumpAsText() { m_textDump = true; }
    void dumpChildFramesAsText() { m_dumpChildrenAsText = true; }
    void setCanOpenWindows() { m_canOpenWindows = true; }
    void waitUntilDone();
    void notifyDone();
    void dumpEditingCallbacks();
    void queueReload();
    void provisionalLoad();
    void setCloseRemainingWindowsWhenComplete(bool=false) {}
    int windowCount();
    void display() {}
    void clearBackForwardList();
    void dumpTitleChanges() { m_dumpTitleChanges = true; }
    QString encodeHostName(const QString &host);
    QString decodeHostName(const QString &host);
    void dumpSelectionRect() const {}
    
private:
    bool m_isLoading;
    bool m_textDump;
    bool m_dumpChildrenAsText;
    bool m_canOpenWindows;
    bool m_waitForDone;
    bool m_dumpTitleChanges;
    int m_timeoutTimer;
    QWebFrame *m_topLoadingFrame;
    WebCore::DumpRenderTree *m_drt;
};

class QWebPage;
class QWebFrame;

class EventSender : public QObject
{
    Q_OBJECT
public:
    EventSender(QWebPage *parent);

public slots:
    void mouseDown();
    void mouseUp();
    void mouseMoveTo(int x, int y);
    void leapForward(int ms);
    void keyDown(const QString &string, const QStringList &modifiers=QStringList());
    void clearKillRing() {}

private:
    QPoint m_mousePos;
    QWebPage *m_page;
    int m_timeLeap;
    QWebFrame *frameUnderMouse() const;
};

class TextInputController : public QObject
{
    Q_OBJECT
public:
    TextInputController(QWebPage *parent);

public slots:
    void doCommand(const QString &command);
//     void setMarkedText(const QString &str, int from, int length);
//     bool hasMarkedText();
//     void unmarkText();
//     QList<int> markedRange();
//     QList<int> selectedRange();
//     void validAttributesForMarkedText();
//     void inserText(const QString &);
//     void firstRectForCharacterRange();
//     void characterIndexForPoint(int, int);
//     void substringFromRange(int, int);
//     void conversationIdentifier();
};

#endif
