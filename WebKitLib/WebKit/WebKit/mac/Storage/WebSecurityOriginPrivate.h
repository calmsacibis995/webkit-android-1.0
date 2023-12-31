/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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

@class WebSecurityOriginPrivate;

@interface WebSecurityOrigin : NSObject {
    WebSecurityOriginPrivate *_private;
}

- (id)initWithProtocol:(NSString *)protocol domain:(NSString *)domain;
- (id)initWithProtocol:(NSString *)protocol domain:(NSString *)domain port:(unsigned short)port;

- (NSString*)protocol;
- (NSString*)domain;

// Returns zero if the port is the default port for the protocol, non-zero otherwise
- (unsigned short)port;

// Returns the current total usage of all databases in this security origin in bytes
- (unsigned long long)usage;

- (unsigned long long)quota;
// Sets the storage quota (in bytes)
// If the quota is set to a value lower than the current usage, that quota will "stick" but no data will be purged to meet the new quota.  
// This will simply prevent new data from being added to databases in that origin
- (void)setQuota:(unsigned long long)quota;

@end
