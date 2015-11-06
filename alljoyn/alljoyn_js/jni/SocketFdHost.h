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
#ifndef _SOCKETFDHOST_H
#define _SOCKETFDHOST_H

#include "ScriptableObject.h"

class _SocketFdHost : public ScriptableObject {
  public:
    _SocketFdHost(Plugin& plugin, qcc::SocketFd& socketFd);
    virtual ~_SocketFdHost();
    qcc::SocketFd GetFd() { return socketFd; }

  private:
    qcc::SocketFd socketFd;

    bool getFd(NPVariant* result);

    bool close(const NPVariant* args, uint32_t argCount, NPVariant* result);
    bool shutdown(const NPVariant* args, uint32_t argCount, NPVariant* result);
    bool recv(const NPVariant* args, uint32_t argCount, NPVariant* result);
    bool send(const NPVariant* args, uint32_t argCount, NPVariant* result);
};

typedef qcc::ManagedObj<_SocketFdHost> SocketFdHost;

#endif // _SOCKETFDHOST_H
