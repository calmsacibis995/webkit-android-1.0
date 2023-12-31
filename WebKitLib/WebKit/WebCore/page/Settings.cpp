/*
 * Copyright (C) 2006, 2007 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "Settings.h"

#include "Frame.h"
#include "FrameTree.h"
#include "Page.h"
#include "PageCache.h"
#include "HistoryItem.h"

#if ENABLE(DATABASE)
#include "DatabaseTracker.h"
#endif

namespace WebCore {

static void setNeedsReapplyStylesInAllFrames(Page* page)
{
    for (Frame* frame = page->mainFrame(); frame; frame = frame->tree()->traverseNext())
        frame->setNeedsReapplyStyles();
}

Settings::Settings(Page* page)
    : m_page(page)
#ifdef ANDROID_LAYOUT
    , m_layoutAlgorithm(kLayoutFitColumnToScreen)
#endif
    , m_editableLinkBehavior(EditableLinkDefaultBehavior)
    , m_minimumFontSize(0)
    , m_minimumLogicalFontSize(0)
    , m_defaultFontSize(0)
    , m_defaultFixedFontSize(0)
#ifdef ANDROID_LAYOUT
    , m_useWideViewport(false)
#endif
#ifdef ANDROID_MULTIPLE_WINDOWS
    , m_supportMultipleWindows(true)
#endif
#ifdef ANDROID_BLOCK_NETWORK_IMAGE
    , m_blockNetworkImage(false)
#endif
    , m_isJavaEnabled(false)
    , m_loadsImagesAutomatically(false)
    , m_privateBrowsingEnabled(false)
    , m_arePluginsEnabled(false)
    , m_isJavaScriptEnabled(false)
    , m_javaScriptCanOpenWindowsAutomatically(false)
    , m_shouldPrintBackgrounds(false)
    , m_textAreasAreResizable(false)
    , m_usesDashboardBackwardCompatibilityMode(false)
    , m_needsAdobeFrameReloadingQuirk(false)
    , m_needsKeyboardEventDisambiguationQuirks(false)
    , m_isDOMPasteAllowed(false)
    , m_shrinksStandaloneImagesToFit(true)
    , m_usesPageCache(false)
    , m_showsURLsInToolTips(false)
    , m_forceFTPDirectoryListings(false)
    , m_developerExtrasEnabled(false)
    , m_authorAndUserStylesEnabled(true)
    , m_needsSiteSpecificQuirks(false)
    , m_fontRenderingMode(0)
{
    // A Frame may not have been created yet, so we initialize the AtomicString 
    // hash before trying to use it.
    AtomicString::init();
#ifdef ANDROID_META_SUPPORT    
    resetMetadataSettings();
#endif
}

void Settings::setStandardFontFamily(const AtomicString& standardFontFamily)
{
    if (standardFontFamily == m_standardFontFamily)
        return;

    m_standardFontFamily = standardFontFamily;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setFixedFontFamily(const AtomicString& fixedFontFamily)
{
    if (m_fixedFontFamily == fixedFontFamily)
        return;
        
    m_fixedFontFamily = fixedFontFamily;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setSerifFontFamily(const AtomicString& serifFontFamily)
{
    if (m_serifFontFamily == serifFontFamily)
        return;
        
    m_serifFontFamily = serifFontFamily;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setSansSerifFontFamily(const AtomicString& sansSerifFontFamily)
{
    if (m_sansSerifFontFamily == sansSerifFontFamily)
        return;
        
    m_sansSerifFontFamily = sansSerifFontFamily; 
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setCursiveFontFamily(const AtomicString& cursiveFontFamily)
{
    if (m_cursiveFontFamily == cursiveFontFamily)
        return;
        
    m_cursiveFontFamily = cursiveFontFamily;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setFantasyFontFamily(const AtomicString& fantasyFontFamily)
{
    if (m_fantasyFontFamily == fantasyFontFamily)
        return;
        
    m_fantasyFontFamily = fantasyFontFamily;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setMinimumFontSize(int minimumFontSize)
{
    if (m_minimumFontSize == minimumFontSize)
        return;

    m_minimumFontSize = minimumFontSize;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setMinimumLogicalFontSize(int minimumLogicalFontSize)
{
    if (m_minimumLogicalFontSize == minimumLogicalFontSize)
        return;

    m_minimumLogicalFontSize = minimumLogicalFontSize;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setDefaultFontSize(int defaultFontSize)
{
    if (m_defaultFontSize == defaultFontSize)
        return;

    m_defaultFontSize = defaultFontSize;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setDefaultFixedFontSize(int defaultFontSize)
{
    if (m_defaultFixedFontSize == defaultFontSize)
        return;

    m_defaultFixedFontSize = defaultFontSize;
    setNeedsReapplyStylesInAllFrames(m_page);
}

#ifdef ANDROID_BLOCK_NETWORK_IMAGE
void Settings::setBlockNetworkImage(bool blockNetworkImage)
{
    m_blockNetworkImage = blockNetworkImage;
}
#endif

void Settings::setLoadsImagesAutomatically(bool loadsImagesAutomatically)
{
    m_loadsImagesAutomatically = loadsImagesAutomatically;
}

void Settings::setJavaScriptEnabled(bool isJavaScriptEnabled)
{
    m_isJavaScriptEnabled = isJavaScriptEnabled;
}

void Settings::setJavaEnabled(bool isJavaEnabled)
{
    m_isJavaEnabled = isJavaEnabled;
}

void Settings::setPluginsEnabled(bool arePluginsEnabled)
{
    m_arePluginsEnabled = arePluginsEnabled;
}

#ifdef ANDROID_PLUGINS
void Settings::setPluginsPath(const String& pluginsPath)
{
    m_pluginsPath = pluginsPath;
}
#endif

void Settings::setPrivateBrowsingEnabled(bool privateBrowsingEnabled)
{
    m_privateBrowsingEnabled = privateBrowsingEnabled;
}

void Settings::setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically)
{
    m_javaScriptCanOpenWindowsAutomatically = javaScriptCanOpenWindowsAutomatically;
}

void Settings::setDefaultTextEncodingName(const String& defaultTextEncodingName)
{
    m_defaultTextEncodingName = defaultTextEncodingName;
}

void Settings::setUserStyleSheetLocation(const KURL& userStyleSheetLocation)
{
    if (m_userStyleSheetLocation == userStyleSheetLocation)
        return;

    m_userStyleSheetLocation = userStyleSheetLocation;

    m_page->userStyleSheetLocationChanged();
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setShouldPrintBackgrounds(bool shouldPrintBackgrounds)
{
    m_shouldPrintBackgrounds = shouldPrintBackgrounds;
}

void Settings::setTextAreasAreResizable(bool textAreasAreResizable)
{
    if (m_textAreasAreResizable == textAreasAreResizable)
        return;

    m_textAreasAreResizable = textAreasAreResizable;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setEditableLinkBehavior(EditableLinkBehavior editableLinkBehavior)
{
    m_editableLinkBehavior = editableLinkBehavior;
}

void Settings::setUsesDashboardBackwardCompatibilityMode(bool usesDashboardBackwardCompatibilityMode)
{
    m_usesDashboardBackwardCompatibilityMode = usesDashboardBackwardCompatibilityMode;
}

// FIXME: This quirk is needed because of Radar 4674537 and 5211271. We need to phase it out once Adobe
// can fix the bug from their end.
void Settings::setNeedsAdobeFrameReloadingQuirk(bool shouldNotReloadIFramesForUnchangedSRC)
{
    m_needsAdobeFrameReloadingQuirk = shouldNotReloadIFramesForUnchangedSRC;
}

// This is a quirk we are pro-actively applying to old applications. It changes keyboard event dispatching,
// making keyIdentifier available on keypress events, making charCode available on keydown/keyup events,
// and getting keypress dispatched in more cases.
void Settings::setNeedsKeyboardEventDisambiguationQuirks(bool needsQuirks)
{
    m_needsKeyboardEventDisambiguationQuirks = needsQuirks;
}

void Settings::setDOMPasteAllowed(bool DOMPasteAllowed)
{
    m_isDOMPasteAllowed = DOMPasteAllowed;
}

void Settings::setUsesPageCache(bool usesPageCache)
{
    if (m_usesPageCache == usesPageCache)
        return;
        
    m_usesPageCache = usesPageCache;
    if (!m_usesPageCache) {
        HistoryItemVector& historyItems = m_page->backForwardList()->entries();
        for (unsigned i = 0; i < historyItems.size(); i++)
            pageCache()->remove(historyItems[i].get());
        pageCache()->releaseAutoreleasedPagesNow();
    }
}

void Settings::setShrinksStandaloneImagesToFit(bool shrinksStandaloneImagesToFit)
{
    m_shrinksStandaloneImagesToFit = shrinksStandaloneImagesToFit;
}

void Settings::setShowsURLsInToolTips(bool showsURLsInToolTips)
{
    m_showsURLsInToolTips = showsURLsInToolTips;
}

void Settings::setFTPDirectoryTemplatePath(const String& path)
{
    m_ftpDirectoryTemplatePath = path;
}

void Settings::setForceFTPDirectoryListings(bool force)
{
    m_forceFTPDirectoryListings = force;
}

void Settings::setDeveloperExtrasEnabled(bool developerExtrasEnabled)
{
    m_developerExtrasEnabled = developerExtrasEnabled;
}

#ifdef ANDROID_META_SUPPORT
void Settings::resetMetadataSettings()
{
    m_viewport_width = -1;
    m_viewport_height = -1;
    m_viewport_initial_scale = 0;
    m_viewport_minimum_scale = 0;
    m_viewport_maximum_scale = 0;
    m_viewport_user_scalable = true;
    m_format_detection_telephone = true;    
    m_format_detection_address = true;    
    m_format_detection_email = true;    
}

void Settings::setMetadataSettings(const String& key, const String& value)
{
    if (key == "width") {
        if (value == "device-width") {
            m_viewport_width = 0;
        } else {
            int width = value.toInt();
            if (width >= 200 && width <= 10000) {
                if (width == 320) {
                    // This is a hack to accommodate the pages designed for the 
                    // original iPhone. The new version, since 10/2007, is to 
                    // use device-width which works for both prtrait and 
                    // landscape modes.
                    m_viewport_width = 0;
                } else {
                    m_viewport_width = width;
                }
            }
        }
    } else if (key == "height") {
        if (value == "device-height") {
            m_viewport_height = 0;
        } else {
            int height = value.toInt();
            if (height >= 200 && height <= 10000) {
                m_viewport_height = height;
            }
        }
    } else if (key == "initial-scale") {
        int scale = int(value.toFloat() * 100);
        if (scale >= 1 && scale <= 1000) {
            m_viewport_initial_scale = scale;
        }
    } else if (key == "minimum-scale") {
        int scale = int(value.toFloat() * 100);
        if (scale >= 1 && scale <= 1000) {
            m_viewport_minimum_scale = scale;
        }
    } else if (key == "maximum-scale") {
        int scale = int(value.toFloat() * 100);
        if (scale >= 1 && scale <= 1000) {
            m_viewport_maximum_scale = scale;
        }
    } else if (key == "user-scalable") {
        // even Apple doc says using "no", "0" is common in the real world, and
        // some sites, e.g. gomoviesapp.com, use "false".
        if (value == "no" || value == "0" || value == "false") {
            m_viewport_user_scalable = false;
        }        
    } else if (key == "telephone") {
        if (value == "no") {
            m_format_detection_telephone = false;
        }        
    } else if (key == "address") {
        if (value == "no") {
            m_format_detection_address = false;
        }        
    } else if (key == "email") {
        if (value == "no") {
            m_format_detection_email = false;
        }        
    } else if (key == "format-detection") {
        // even Apple doc says "format-detection" should be the name of the 
        // <meta> tag. In the real world, e.g. amazon.com, use 
        // "format-detection=no" in the "viewport" <meta> tag to disable all
        // format detection.
        if (value == "no") {
            m_format_detection_telephone = false;
            m_format_detection_address = false;
            m_format_detection_email = false;
        }        
    }
}
#endif

void Settings::setAuthorAndUserStylesEnabled(bool authorAndUserStylesEnabled)
{
    if (m_authorAndUserStylesEnabled == authorAndUserStylesEnabled)
        return;

    m_authorAndUserStylesEnabled = authorAndUserStylesEnabled;
    setNeedsReapplyStylesInAllFrames(m_page);
}

void Settings::setFontRenderingMode(FontRenderingMode mode)
{
    if (fontRenderingMode() == mode)
        return;
    m_fontRenderingMode = mode;
    setNeedsReapplyStylesInAllFrames(m_page);
}

FontRenderingMode Settings::fontRenderingMode() const
{
    return static_cast<FontRenderingMode>(m_fontRenderingMode);
}

void Settings::setNeedsSiteSpecificQuirks(bool needsQuirks)
{
    m_needsSiteSpecificQuirks = needsQuirks;
}

} // namespace WebCore
