/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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

#include "config.h"
#include "FontCache.h"
#include "FontPlatformData.h"
#include "Font.h"

#include "SkPaint.h"
#include "SkTypeface.h"
#include "SkUtils.h"

namespace WebCore {

void FontCache::platformInit()
{
}

const SimpleFontData* FontCache::getFontDataForCharacters(const Font& font, const UChar* characters, int length)
{
    return font.primaryFont();  // do I need to make a copy (i.e. does the caller delete what I return?

#if 0
    // IMLangFontLink::MapFont Method does what we want.
    IMLangFontLink2* langFontLink = getFontLinkInterface();
    if (!langFontLink)
        return 0;

    FontData* fontData = 0;
    HDC hdc = GetDC(0);
    DWORD fontCodePages;
    langFontLink->GetFontCodePages(hdc, font.primaryFont()->m_font.hfont(), &fontCodePages);

    DWORD actualCodePages;
    long cchActual;
    langFontLink->GetStrCodePages(characters, length, fontCodePages, &actualCodePages, &cchActual);
    if (cchActual) {
        HFONT result;
        if (langFontLink->MapFont(hdc, actualCodePages, characters[0], &result) == S_OK) {
            fontData = new FontData(FontPlatformData(result, font.fontDescription().computedPixelSize()));
            fontData->setIsMLangFont();
        }
    }

    ReleaseDC(0, hdc);
    return fontData;
#endif
}

FontPlatformData* FontCache::getSimilarFontPlatformData(const Font& font)
{
    return 0;
}

FontPlatformData* FontCache::getLastResortFallbackFont(const FontDescription& font)
{
    static AtomicString str("sans-serif");
    return getCachedFontPlatformData(font, str);
}

static char* AtomicStringToUTF8String(const AtomicString& utf16)
{
    SkASSERT(sizeof(uint16_t) == sizeof(utf16.characters()[0]));
    const uint16_t* uni = (uint16_t*)utf16.characters();

    size_t bytes = SkUTF16_ToUTF8(uni, utf16.length(), NULL);
    char*  utf8 = (char*)sk_malloc_throw(bytes + 1);

    (void)SkUTF16_ToUTF8(uni, utf16.length(), utf8);
    utf8[bytes] = 0;
    return utf8;
}

bool FontCache::fontExists(const FontDescription& fontDescription, const AtomicString& family)
{
    ASSERT(0); // FIXME HACK unimplemented
    return false;
}

FontPlatformData* FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& family)
{
    char*       storage = 0;
    const char* name = 0;
    
    if (family.length() == 0) {
        static const struct {
            FontDescription::GenericFamilyType  mType;
            const char*                         mName;
        } gNames[] = {
            { FontDescription::SerifFamily,     "serif" },
            { FontDescription::SansSerifFamily, "sans-serif" },
            { FontDescription::MonospaceFamily, "monospace" },
            { FontDescription::CursiveFamily,   "cursive" },
            { FontDescription::FantasyFamily,   "fantasy" }
        };

        FontDescription::GenericFamilyType type = fontDescription.genericFamily();
        for (unsigned i = 0; i < SK_ARRAY_COUNT(gNames); i++)
        {
            if (type == gNames[i].mType)
            {
                name = gNames[i].mName;
                break;
            }
        }
        // if we fall out of the loop, its ok for name to still be 0
    }
    else {    // convert the name to utf8
        storage = AtomicStringToUTF8String(family);
        name = storage;
    }
    
    int style = SkTypeface::kNormal;
    if (fontDescription.weight() >= cBoldWeight)
        style |= SkTypeface::kBold;
    if (fontDescription.italic())
        style |= SkTypeface::kItalic;

    SkTypeface* tf = SkTypeface::Create(name, (SkTypeface::Style)style);
    
    FontPlatformData* result = new FontPlatformData(tf,
                                                    fontDescription.computedSize(),
                                                    (style & SkTypeface::kBold) && !tf->isBold(),
                                                    (style & SkTypeface::kItalic) && !tf->isItalic());
    tf->unref();
    sk_free(storage);
    return result;
}

}

