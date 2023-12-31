/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
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
#include "PluginInfoStore.h"
#include "PluginDatabaseAndroid.h"
#include "PluginPackageAndroid.h"

namespace WebCore {

PluginInfo* PluginInfoStore::createPluginInfoForPluginAtIndex(unsigned i) 
{ 
    PluginDatabaseAndroid *db = PluginDatabaseAndroid::installedPlugins();
    PluginInfo* info = new PluginInfo;
    PluginPackageAndroid* package = db->plugins()[i];

    info->name = package->name();
    info->file = package->fileName();
    info->desc = package->description();

    const MIMEToDescriptionsMap& mimeToDescriptions = package->mimeToDescriptions();
    MIMEToDescriptionsMap::const_iterator end = mimeToDescriptions.end();
    for (MIMEToDescriptionsMap::const_iterator it = mimeToDescriptions.begin(); it != end; ++it) {
        MimeClassInfo* mime = new MimeClassInfo;
        info->mimes.append(mime);
        
        mime->type = it->first;
        mime->desc = it->second;
        mime->plugin = info;

        Vector<String> extensions = package->mimeToExtensions().get(mime->type);
        
        for (unsigned i = 0; i < extensions.size(); i++) {
            if (i > 0)
                mime->suffixes += ",";

            mime->suffixes += extensions[i];
        }
    }

    return info;
}

unsigned PluginInfoStore::pluginCount() const
{
    return PluginDatabaseAndroid::installedPlugins()->plugins().size();
}

bool PluginInfoStore::supportsMIMEType(const WebCore::String& mimeType) 
{
    return PluginDatabaseAndroid::installedPlugins()->isMIMETypeRegistered(mimeType);
}

void refreshPlugins(bool reloadOpenPages)
{
    PluginDatabaseAndroid::installedPlugins()->refresh();

    if (reloadOpenPages) {
        // FIXME: reload open pages
    }
}

}
