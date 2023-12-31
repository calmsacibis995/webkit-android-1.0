/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
#include "Screen.h"

#include "FloatRect.h"
#include "Widget.h"

#define LOG_TAG "WebCore"
#include "utils/Log.h"

#include "ui/SurfaceComposerClient.h"
#include "ui/PixelFormat.h"
#include "ui/DisplayInfo.h"

namespace WebCore {

int screenDepth(Widget* page)
{
    android::DisplayInfo info;
    android::SurfaceComposerClient::getDisplayInfo(android::DisplayID(0), &info);
    return info.pixelFormatInfo.bitsPerPixel;
}

int screenDepthPerComponent(Widget* page)
{
    android::DisplayInfo info;
    android::SurfaceComposerClient::getDisplayInfo(android::DisplayID(0), &info);
    return info.pixelFormatInfo.bitsPerPixel;
}

bool screenIsMonochrome(Widget* page)
{
    return false;
}

#ifdef ANDROID_ORIENTATION_SUPPORT
int Screen::orientation() const
{
    android::DisplayInfo info;
    android::SurfaceComposerClient::getDisplayInfo(android::DisplayID(0), &info);
    return info.orientation;
}
#endif

// The only place I have seen these values used is
// positioning popup windows. If we support multiple windows
// they will be most likely full screen. Therefore,
// the accuracy of these number are not too important.
FloatRect screenRect(Widget* page)
{
    android::DisplayInfo info;
    android::SurfaceComposerClient::getDisplayInfo(android::DisplayID(0), &info);
    return FloatRect(0.0, 0.0, info.w, info.h);
}

/* 
Scale functions don't seem to be needed. There is no code that
call them, and they seem to only exist in ports and not in the core
WebKit code - davidc
FloatRect usableScreenRect(Widget* page)
{
    return FloatRect();;
}

FloatRect scaleScreenRectToPageCoordinates(const FloatRect&, Widget*)
{
    return FloatRect();;
}

FloatRect scalePageRectToScreenCoordinates(const FloatRect&, Widget*)
{
    return FloatRect();;
}

float scaleFactor(Widget*)
{
    return 1.0f;
}
*/

// Similar screenRect, this function is commonly used by javascripts
// to position and resize windows (usually to full screen). 
FloatRect screenAvailableRect(Widget*)
{
    android::DisplayInfo info;
    android::SurfaceComposerClient::getDisplayInfo(android::DisplayID(0), &info);
    return FloatRect(0.0, 0.0, info.w, info.h);
}

}
