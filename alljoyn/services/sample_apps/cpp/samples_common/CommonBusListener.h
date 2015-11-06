/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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
 ******************************************************************************/

#ifndef COMMONBUSLISTENER_H_
#define COMMONBUSLISTENER_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusListener.h>
#include <alljoyn/SessionPortListener.h>
#include <vector>

/**
 * class CommonBusListener
 */
class CommonBusListener : public ajn::BusListener, public ajn::SessionPortListener, public ajn::SessionListener {

  public:

    /**
     * Constructor of CommonBusListener
     * @param bus - used to set a session Listener
     * @param daemonDisconnectCB - used to set a callback for when the daemon is disconnected
     */
    CommonBusListener(ajn::BusAttachment* bus = NULL, void(*daemonDisconnectCB)() = NULL);

    /**
     * Destructor of CommonBusListener
     */
    ~CommonBusListener();

    /**
     * AcceptSessionJoiner - Receive request to join session and decide whether to accept it or not
     * @param sessionPort - the port of the request
     * @param joiner - the name of the joiner
     * @param opts - the session options
     * @return true/false
     */
    bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);

    /**
     * Set the Value of the SessionPort associated with this SessionPortListener
     * @param sessionPort
     */
    void setSessionPort(ajn::SessionPort sessionPort);

    /**
     * Callback when Session is joined
     * @param sessionPort - port of session
     * @param id - sessionId of session
     * @param joiner - name of joiner
     */
    void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);

    /**
     * Callback for when Session is lost
     * @param sessionId
     * @param reason for session lost
     */
    void SessionLost(ajn::SessionId sessionId, SessionLostReason reason);

    /**
     * Get the SessionPort of the listener
     * @return
     */
    ajn::SessionPort getSessionPort();

    /**
     * Get the SessionIds associated with this Listener
     * @return vector of sessionIds
     */
    const std::vector<ajn::SessionId>& getSessionIds() const;

    /**
     * Function when Bus has been disconnected
     */
    void BusDisconnected();

  private:

    /**
     * The port used as part of the join session request
     */
    ajn::SessionPort m_SessionPort;

    /**
     * The busAttachment to use
     */
    ajn::BusAttachment* m_Bus;

    /**
     * The sessionIds for the port
     */
    std::vector<ajn::SessionId> m_SessionIds;

    /**
     * Callback when daemon is disconnected
     */
    void (*m_DaemonDisconnectCB)();

};

#endif /* COMMONBUSLISTENER_H_ */
