/*
 * Copyright (C) 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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
#include "UIDelegate.h"

#include "DraggingInfo.h"
#include "EventSender.h"
#include "LayoutTestController.h"

#include <WebCore/COMPtr.h>
#include <wtf/Platform.h>
#include <wtf/Vector.h>
#include <JavaScriptCore/Assertions.h>
#include <JavaScriptCore/JavaScriptCore.h>
#include <WebKit/IWebFramePrivate.h>
#include <WebKit/IWebViewPrivate.h>
#include <stdio.h>

using std::wstring;

class DRTUndoObject {
public:
    DRTUndoObject(IWebUndoTarget* target, BSTR actionName, IUnknown* obj)
        : m_target(target)
        , m_actionName(SysAllocString(actionName))
        , m_obj(obj)
    {
    }

    ~DRTUndoObject()
    {
        SysFreeString(m_actionName);
    }

    void invoke()
    {
        m_target->invoke(m_actionName, m_obj.get());
    }

private:
    IWebUndoTarget* m_target;
    BSTR m_actionName;
    COMPtr<IUnknown> m_obj;
};

class DRTUndoStack {
public:
    ~DRTUndoStack() { deleteAllValues(m_undoVector); }

    bool isEmpty() const { return m_undoVector.isEmpty(); }
    void clear() { deleteAllValues(m_undoVector); m_undoVector.clear(); }

    void push(DRTUndoObject* undoObject) { m_undoVector.append(undoObject); }
    DRTUndoObject* pop() { DRTUndoObject* top = m_undoVector.last(); m_undoVector.removeLast(); return top; }

private:
    Vector<DRTUndoObject*> m_undoVector;
};

class DRTUndoManager {
public:
    DRTUndoManager();

    void removeAllActions();
    void registerUndoWithTarget(IWebUndoTarget* target, BSTR actionName, IUnknown* obj);
    void redo();
    void undo();
    bool canRedo() { return !m_redoStack->isEmpty(); }
    bool canUndo() { return !m_undoStack->isEmpty(); }

private:
    OwnPtr<DRTUndoStack> m_redoStack;
    OwnPtr<DRTUndoStack> m_undoStack;
    bool m_isRedoing;
    bool m_isUndoing;
};

DRTUndoManager::DRTUndoManager()
    : m_redoStack(new DRTUndoStack)
    , m_undoStack(new DRTUndoStack)
    , m_isRedoing(false)
    , m_isUndoing(false)
{
}

void DRTUndoManager::removeAllActions()
{
    m_redoStack->clear();
    m_undoStack->clear();
}

void DRTUndoManager::registerUndoWithTarget(IWebUndoTarget* target, BSTR actionName, IUnknown* obj)
{
    if (!m_isUndoing && !m_isRedoing)
        m_redoStack->clear();

    DRTUndoStack* stack = m_isUndoing ? m_redoStack.get() : m_undoStack.get();
    stack->push(new DRTUndoObject(target, actionName, obj));
}

void DRTUndoManager::redo()
{
    if (!canRedo())
        return;

    m_isRedoing = true;

    DRTUndoObject* redoObject = m_redoStack->pop();
    redoObject->invoke();
    delete redoObject;

    m_isRedoing = false;
}

void DRTUndoManager::undo()
{
    if (!canUndo())
        return;

    m_isUndoing = true;

    DRTUndoObject* undoObject = m_undoStack->pop();
    undoObject->invoke();
    delete undoObject;

    m_isUndoing = false;
}

UIDelegate::UIDelegate()
    : m_refCount(1)
    , m_undoManager(new DRTUndoManager)
{
    m_frame.bottom = 0;
    m_frame.top = 0;
    m_frame.left = 0;
    m_frame.right = 0;
}

void UIDelegate::resetUndoManager()
{
    m_undoManager.set(new DRTUndoManager);
}

HRESULT STDMETHODCALLTYPE UIDelegate::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualGUID(riid, IID_IUnknown))
        *ppvObject = static_cast<IWebUIDelegate*>(this);
    else if (IsEqualGUID(riid, IID_IWebUIDelegate))
        *ppvObject = static_cast<IWebUIDelegate*>(this);
    else if (IsEqualGUID(riid, IID_IWebUIDelegatePrivate))
        *ppvObject = static_cast<IWebUIDelegatePrivate*>(this);
    else if (IsEqualGUID(riid, IID_IWebUIDelegatePrivate2))
        *ppvObject = static_cast<IWebUIDelegatePrivate2*>(this);
    else if (IsEqualGUID(riid, IID_IWebUIDelegatePrivate3))
        *ppvObject = static_cast<IWebUIDelegatePrivate3*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE UIDelegate::AddRef()
{
    return ++m_refCount;
}

ULONG STDMETHODCALLTYPE UIDelegate::Release()
{
    ULONG newRef = --m_refCount;
    if (!newRef)
        delete(this);

    return newRef;
}

HRESULT STDMETHODCALLTYPE UIDelegate::hasCustomMenuImplementation( 
        /* [retval][out] */ BOOL *hasCustomMenus)
{
    *hasCustomMenus = TRUE;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::trackCustomPopupMenu( 
        /* [in] */ IWebView *sender,
        /* [in] */ OLE_HANDLE menu,
        /* [in] */ LPPOINT point)
{
    // Do nothing
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::registerUndoWithTarget(
        /* [in] */ IWebUndoTarget* target,
        /* [in] */ BSTR actionName,
        /* [in] */ IUnknown* actionArg)
{
    m_undoManager->registerUndoWithTarget(target, actionName, actionArg);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::removeAllActionsWithTarget(
        /* [in] */ IWebUndoTarget*)
{
    m_undoManager->removeAllActions();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::setActionTitle(
        /* [in] */ BSTR actionTitle)
{
    // It is not neccessary to implement this for DRT because there is
    // menu to write out the title to.
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::undo()
{
    m_undoManager->undo();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::redo()
{
    m_undoManager->redo();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::canUndo(
        /* [retval][out] */ BOOL* result)
{
    if (!result)
        return E_POINTER;

    *result = m_undoManager->canUndo();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::canRedo(
        /* [retval][out] */ BOOL* result)
{
    if (!result)
        return E_POINTER;

    *result = m_undoManager->canRedo();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::setFrame( 
        /* [in] */ IWebView* /*sender*/,
        /* [in] */ RECT* frame)
{
    m_frame = *frame;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::webViewFrame( 
        /* [in] */ IWebView* /*sender*/,
        /* [retval][out] */ RECT* frame)
{
    *frame = m_frame;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::runJavaScriptAlertPanelWithMessage( 
        /* [in] */ IWebView* /*sender*/,
        /* [in] */ BSTR message)
{
    printf("ALERT: %S\n", message ? message : L"");

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::runJavaScriptConfirmPanelWithMessage( 
    /* [in] */ IWebView* sender,
    /* [in] */ BSTR message,
    /* [retval][out] */ BOOL* result)
{
    printf("CONFIRM: %S\n", message ? message : L"");
    *result = TRUE;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::runJavaScriptTextInputPanelWithPrompt( 
    /* [in] */ IWebView *sender,
    /* [in] */ BSTR message,
    /* [in] */ BSTR defaultText,
    /* [retval][out] */ BSTR *result)
{
    printf("PROMPT: %S, default text: %S\n", message ? message : L"", defaultText ? defaultText : L"");
    *result = SysAllocString(defaultText);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::webViewAddMessageToConsole( 
    /* [in] */ IWebView* sender,
    /* [in] */ BSTR message,
    /* [in] */ int lineNumber,
    /* [in] */ BSTR url,
    /* [in] */ BOOL isError)
{
    wstring newMessage;
    if (message) {
        newMessage = message;
        size_t fileProtocol = newMessage.find(L"file://");
        if (fileProtocol != wstring::npos)
            newMessage = newMessage.substr(0, fileProtocol) + urlSuitableForTestResult(newMessage.substr(fileProtocol));
    }

    printf("CONSOLE MESSAGE: line %d: %S\n", lineNumber, newMessage.c_str());
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::doDragDrop( 
    /* [in] */ IWebView* sender,
    /* [in] */ IDataObject* object,
    /* [in] */ IDropSource* source,
    /* [in] */ DWORD okEffect,
    /* [retval][out] */ DWORD* performedEffect)
{
    if (!performedEffect)
        return E_POINTER;

    *performedEffect = 0;

    draggingInfo = new DraggingInfo(object, source);
    replaySavedEvents();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::webViewGetDlgCode( 
    /* [in] */ IWebView* /*sender*/,
    /* [in] */ UINT /*keyCode*/,
    /* [retval][out] */ LONG_PTR *code)
{
    if (!code)
        return E_POINTER;
    *code = 0;
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE UIDelegate::createWebViewWithRequest( 
        /* [in] */ IWebView *sender,
        /* [in] */ IWebURLRequest *request,
        /* [retval][out] */ IWebView **newWebView)
{
    if (!::layoutTestController->canOpenWindows())
        return E_FAIL;
    *newWebView = createWebViewAndOffscreenWindow();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::webViewClose( 
        /* [in] */ IWebView *sender)
{
    HWND hostWindow;
    sender->hostWindow(reinterpret_cast<OLE_HANDLE*>(&hostWindow));
    DestroyWindow(hostWindow);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::webViewPainted( 
        /* [in] */ IWebView *sender)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIDelegate::exceededDatabaseQuota( 
        /* [in] */ IWebView *sender,
        /* [in] */ IWebFrame *frame,
        /* [in] */ IWebSecurityOrigin *origin,
        /* [in] */ BSTR databaseIdentifier)
{
    static const unsigned long long defaultQuota = 5 * 1024 * 1024;
    origin->setQuota(defaultQuota);

    return S_OK;
}
