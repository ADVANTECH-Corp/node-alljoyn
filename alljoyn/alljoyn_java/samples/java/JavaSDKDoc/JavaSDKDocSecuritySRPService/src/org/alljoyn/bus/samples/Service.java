/*
 * Copyright (c) 2010-2011, 2013, AllSeen Alliance. All rights reserved.
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
package org.alljoyn.bus.samples;

import java.util.Random;

import org.alljoyn.bus.AuthListener;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusListener;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.Mutable;
import org.alljoyn.bus.SessionListener;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.SessionPortListener;
import org.alljoyn.bus.SignalEmitter;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.AuthListener.AuthRequest;
import org.alljoyn.bus.AuthListener.PasswordRequest;

public class Service {
    static { 
        System.loadLibrary("alljoyn_java");
    }

    private static final short CONTACT_PORT=42;

    static boolean sessionEstablished = false;
    static int sessionId;
    private static String mPassword;

    private static SrpKeyXListener mAuthListener;

    public static class SecureService implements SecureInterface, BusObject {
        public String Ping(String inStr) throws BusException {
            System.out.println("Ping : " + inStr);
            System.out.println("Reply : " + inStr);
            return inStr;
        }
    }

    private static class MyBusListener extends BusListener {
        public void nameOwnerChanged(String busName, String previousOwner, String newOwner){
            if ("com.my.well.known.name".equals(busName)) {
                System.out.println("BusAttachement.nameOwnerChanged(" + busName + ", " + previousOwner + ", " + newOwner);
            }
        }
    }

    /*
     * This Secure Remote Password Key eXchange listener creates a random 
     * 6-digit pincode for each authentication attempt.   
     */
    private static class SrpKeyXListener implements AuthListener {
        private Random mPasswordGenerator;
        private boolean mGeneratePassword;

        public SrpKeyXListener() {
            mPasswordGenerator = new Random();
            mGeneratePassword = true;
        }

        public boolean requested(String mechanism, String peerName, int count,
                String userName, AuthRequest[] requests) {
            System.out.println(String.format("AuthListener.requested(%s, %s, %d, %s, %s);", 
                    mechanism ,
                    peerName,
                    count,
                    userName,
                    AuthRequestToString(requests)));
            if (mGeneratePassword) {
                mGeneratePassword = false;
                mPassword = String.format("%06d", mPasswordGenerator.nextInt(1000000));
                System.out.println("One Time Password : " + mPassword);
            }
            for (AuthRequest request : requests) {
                if (request instanceof PasswordRequest) {
                    ((PasswordRequest) request).setPassword(mPassword.toCharArray());
                }
            }
            return true;
        }

        public void completed(String mechanism, String peerName, boolean authenticated) {
            mGeneratePassword = true;
        }

        private String AuthRequestToString(AuthListener.AuthRequest[] requests) {
            String str;
            str = "[";
            for (AuthListener.AuthRequest request : requests) {
                if (request instanceof AuthListener.AuthRequest) {
                    str += "AuthRequest ";
                }
                if (request instanceof AuthListener.CertificateRequest) {
                    str += "CertificateRequest ";
                }
                if (request instanceof AuthListener.LogonEntryRequest) {
                    str += "LogonEntryRequest ";
                }
                if (request instanceof AuthListener.PrivateKeyRequest) {
                    str += "PrivateKeyRequest ";
                }
                if (request instanceof AuthListener.UserNameRequest) {
                    str += "UserNameRequest ";
                }
                if (request instanceof AuthListener.PasswordRequest) {
                    str += "PasswordRequest ";
                }
                if (request instanceof AuthListener.VerifyRequest) {
                    str += "VerifyRequest ";
                }
            }
            str += "]";
            return str;
        }
    }

    public static void main(String[] args) {

        BusAttachment mBus;
        mBus = new BusAttachment("SRPSecurityService", BusAttachment.RemoteMessage.Receive);

        Status status;

        SecureService mySecureService = new SecureService();

        status = mBus.registerBusObject(mySecureService, "/testSrpSecurity");
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.registerBusObject successful");

        BusListener listener = new MyBusListener();
        mBus.registerBusListener(listener);

        mAuthListener = new SrpKeyXListener();
        status = mBus.registerAuthListener("ALLJOYN_SRP_KEYX", mAuthListener);
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.registerAuthListener successful");

        status = mBus.connect();
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.connect successful on " + System.getProperty("org.alljoyn.bus.address"));        

        int flags = 0; //do not use any request name flags
        status = mBus.requestName("com.my.well.known.name", flags);
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.request 'com.my.well.known.name' successful");

        status = mBus.advertiseName("com.my.well.known.name", SessionOpts.TRANSPORT_ANY);
        if (status != Status.OK) {
            System.out.println("Status = " + status);
            mBus.releaseName("com.my.well.known.name");
            return;
        }
        System.out.println("BusAttachment.advertiseName 'com.my.well.known.name' successful");



        Mutable.ShortValue contactPort = new Mutable.ShortValue(CONTACT_PORT);

        SessionOpts sessionOpts = new SessionOpts();
        sessionOpts.traffic = SessionOpts.TRAFFIC_MESSAGES;
        sessionOpts.isMultipoint = false;
        sessionOpts.proximity = SessionOpts.PROXIMITY_ANY;
        sessionOpts.transports = SessionOpts.TRANSPORT_ANY;

        status = mBus.bindSessionPort(contactPort, sessionOpts, 
                new SessionPortListener() {
            public boolean acceptSessionJoiner(short sessionPort, String joiner, SessionOpts sessionOpts) {
                System.out.println("SessionPortListener.acceptSessionJoiner called");
                if (sessionPort == CONTACT_PORT) {
                    return true;
                } else {
                    return false;
                }
            }
            public void sessionJoined(short sessionPort, int id, String joiner) {
                System.out.println(String.format("SessionPortListener.sessionJoined(%d, %d, %s)", sessionPort, id, joiner));
                sessionId = id;
                sessionEstablished = true;
            }
        });
        if (status != Status.OK) {
            return;
        }
        System.out.println("BusAttachment.bindSessionPort successful");

        while (!sessionEstablished) {
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                System.out.println("Thead Exception caught");
                e.printStackTrace();
            }
        }

        System.out.println("BusAttachment session established");

        while (true) {
            try {
                Thread.sleep(10000);
            } catch (InterruptedException e) {
                System.out.println("Thead Exception caught");
                e.printStackTrace();
            }
        }
    }
}
