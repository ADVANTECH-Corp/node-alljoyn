/*
 * Copyright (c) 2011-2012, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "SessionJoinedListenerNative.h"

#include "TypeMapping.h"
#include <qcc/Debug.h>

#define QCC_MODULE "ALLJOYN_JS"

SessionJoinedListenerNative::SessionJoinedListenerNative(Plugin& plugin, NPObject* objectValue) :
    NativeObject(plugin, objectValue)
{
    QCC_DbgTrace(("%s", __FUNCTION__));
}

SessionJoinedListenerNative::~SessionJoinedListenerNative()
{
    QCC_DbgTrace(("%s", __FUNCTION__));
}

void SessionJoinedListenerNative::onJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const qcc::String& joiner)
{
    QCC_DbgTrace(("%s(sessionPort=%d,id=%u,joiner=%s)", __FUNCTION__, sessionPort, id, joiner.c_str()));

    NPVariant npargs[3];
    ToUnsignedShort(plugin, sessionPort, npargs[0]);
    ToUnsignedLong(plugin, id, npargs[1]);
    ToDOMString(plugin, joiner, npargs[2]);

    NPVariant result = NPVARIANT_VOID;
    NPN_InvokeDefault(plugin->npp, objectValue, npargs, 3, &result);
    NPN_ReleaseVariantValue(&result);

    NPN_ReleaseVariantValue(&npargs[2]);
}
