/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "CSSStyleSheet.h"

#include "CSSImportRule.h"
#include "CSSNamespace.h"
#include "CSSParser.h"
#include "CSSRuleList.h"
#include "Document.h"
#include "ExceptionCode.h"
#include "Node.h"

#ifdef ANDROID_INSTRUMENT
#undef LOG
#include <utils/Log.h>
#include "SystemTime.h"
#include "Frame.h"
#endif

namespace WebCore {

CSSStyleSheet::CSSStyleSheet(CSSStyleSheet* parentSheet, const String& href, const String& charset)
    : StyleSheet(parentSheet, href)
    , m_doc(parentSheet ? parentSheet->doc() : 0)
    , m_namespaces(0)
    , m_charset(charset)
    , m_loadCompleted(false)
{
}

CSSStyleSheet::CSSStyleSheet(Node *parentNode, const String& href, const String& charset)
    : StyleSheet(parentNode, href)
    , m_doc(parentNode->document())
    , m_namespaces(0)
    , m_charset(charset)
    , m_loadCompleted(false)
{
}

CSSStyleSheet::CSSStyleSheet(CSSRule *ownerRule, const String& href, const String& charset)
    : StyleSheet(ownerRule, href)
    , m_doc(0)
    , m_namespaces(0)
    , m_charset(charset)
    , m_loadCompleted(false)
{
}

CSSStyleSheet::~CSSStyleSheet()
{
    delete m_namespaces;
}

CSSRule *CSSStyleSheet::ownerRule() const
{
    return (parent() && parent()->isRule()) ? static_cast<CSSRule*>(parent()) : 0;
}

unsigned CSSStyleSheet::insertRule(const String& rule, unsigned index, ExceptionCode& ec)
{
    ec = 0;
    if (index > length()) {
        ec = INDEX_SIZE_ERR;
        return 0;
    }
    CSSParser p(useStrictParsing());
    RefPtr<CSSRule> r = p.parseRule(this, rule);

    if (!r) {
        ec = SYNTAX_ERR;
        return 0;
    }

    // ###
    // HIERARCHY_REQUEST_ERR: Raised if the rule cannot be inserted at the specified index e.g. if an
    //@import rule is inserted after a standard rule set or other at-rule.
    insert(index, r.release());
    
    styleSheetChanged();
    
    return index;
}

int CSSStyleSheet::addRule(const String& selector, const String& style, int index, ExceptionCode& ec)
{
    insertRule(selector + " { " + style + " }", index, ec);

    // As per Microsoft documentation, always return -1.
    return -1;
}

int CSSStyleSheet::addRule(const String& selector, const String& style, ExceptionCode& ec)
{
    return addRule(selector, style, length(), ec);
}


CSSRuleList* CSSStyleSheet::cssRules(bool omitCharsetRules)
{
    return new CSSRuleList(this, omitCharsetRules);
}

void CSSStyleSheet::deleteRule(unsigned index, ExceptionCode& ec)
{
    if (index >= length()) {
        ec = INDEX_SIZE_ERR;
        return;
    }

    ec = 0;
    remove(index);
    styleSheetChanged();
}

void CSSStyleSheet::addNamespace(CSSParser* p, const AtomicString& prefix, const AtomicString& uri)
{
    if (uri.isEmpty())
        return;

    m_namespaces = new CSSNamespace(prefix, uri, m_namespaces);
    
    if (prefix.isEmpty())
        // Set the default namespace on the parser so that selectors that omit namespace info will
        // be able to pick it up easily.
        p->defaultNamespace = uri;
}

const AtomicString& CSSStyleSheet::determineNamespace(const AtomicString& prefix)
{
    if (prefix.isEmpty())
        return nullAtom; // No namespace. If an element/attribute has a namespace, we won't match it.
    else if (prefix == starAtom)
        return starAtom; // We'll match any namespace.
    else if (m_namespaces) {
        CSSNamespace* ns = m_namespaces->namespaceForPrefix(prefix);
        if (ns)
            return ns->uri();
    }
    return nullAtom; // Assume we wont match any namespaces.
}

#ifdef ANDROID_INSTRUMENT
static uint32_t sTotalTimeUsed = 0;
static uint32_t sCounter = 0;
    
void Frame::resetCSSTimeCounter()
{
    sTotalTimeUsed = 0;
    sCounter = 0;
}

void Frame::reportCSSTimeCounter()
{
    LOG(LOG_DEBUG, "WebCore", "*-* Total css parsing time: %d ms called %d times\n", 
            sTotalTimeUsed, sCounter);   
}
#endif

bool CSSStyleSheet::parseString(const String &string, bool strict)
{
#ifdef ANDROID_INSTRUMENT
    uint32_t startTime = 0;
    startTime = get_thread_msec();
#endif    
    setStrictParsing(strict);
    CSSParser p(strict);
    p.parseSheet(this, string);
#ifdef ANDROID_INSTRUMENT
    sTotalTimeUsed += get_thread_msec() - startTime;
    sCounter++;
#endif    
    return true;
}

bool CSSStyleSheet::isLoading()
{
    unsigned len = length();
    for (unsigned i = 0; i < len; ++i) {
        StyleBase* rule = item(i);
        if (rule->isImportRule() && static_cast<CSSImportRule*>(rule)->isLoading())
            return true;
    }
    return false;
}

void CSSStyleSheet::checkLoaded()
{
    if (isLoading())
        return;
    if (parent())
        parent()->checkLoaded();
    m_loadCompleted = m_parentNode ? m_parentNode->sheetLoaded() : true;
}

DocLoader *CSSStyleSheet::docLoader()
{
    if (!m_doc) // doc is 0 for the user- and default-sheet!
        return 0;

    // ### remove? (clients just use sheet->doc()->docLoader())
    return m_doc->docLoader();
}

void CSSStyleSheet::styleSheetChanged()
{
    StyleBase* root = this;
    while (StyleBase* parent = root->parent())
        root = parent;
    Document* documentToUpdate = (root && root->isCSSStyleSheet()) ? static_cast<CSSStyleSheet*>(root)->doc() : 0;
    
    /* FIXME: We don't need to do everything updateStyleSelector does,
     * basically we just need to recreate the document's selector with the
     * already existing style sheets.
     */
    if (documentToUpdate)
        documentToUpdate->updateStyleSelector();
}

}
