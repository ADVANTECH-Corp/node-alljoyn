{
  'variables': { 'target_arch%': 'x86' },
  'target_defaults': {
    'default_configuration': 'debug',
    'configurations': {
      'debug': {
        'defines': [ 'VARIANT=debug' ]
      },
      'release': {
        'defines': [ 'VARIANT=release' ]
      }
    },
    'include_dirs': [
      'gen',
      'gen/alljoyn',
      'alljoyn/alljoyn_core/inc',
      'alljoyn/alljoyn_core/inc/alljoyn',
      'alljoyn/alljoyn_core/src',
      'alljoyn/common/inc',
      'alljoyn/services/services_common/cpp/inc',
      'alljoyn/services/about/cpp/inc',
      'alljoyn/services/notification/cpp/inc',
      "<!(node -e \"require('nan')\")"
    ],
    'cflags':[
      '-Wno-deprecated',
      '-Wno-ignored-qualifiers'
    ],
    'xcode_settings': {
        'OTHER_CFLAGS': [
          '-Wno-error=non-pod-varargs',
          '-Wno-ignored-qualifiers'
         ]
    },
    'link_settings': {
      'libraries': [
      '-lstdc++',
      '-lcrypto',
      '-lpthread'
      ]
    },
    'conditions': [
      ['OS=="mac"', {
        'defines': [
          'MECHANISM_PIPE',
          'QCC_OS_GROUP_POSIX',
          'QCC_OS_DARWIN'
        ]
      }],
      ['OS=="linux"', {
        'defines': [
          'QCC_OS_GROUP_POSIX',
          'QCC_OS_LINUX'
        ],
        'link_settings': {
          'libraries': [
          '-lrt'
          ]
        },
      }]
    ]
  },
  'targets': [
    {
      'target_name': 'node-alljoyn',
      'dependencies': [
        'alljoyn'
      ],
      'sources': [
        'src/ActionHandler.cc',
        'src/bindings.cc'
      ]
    },
    {
      'target_name': 'alljoynstatus',
      'product_prefix': 'lib',
      'type': 'static_library',
      'sources': [
        'gen/alljoyn/version.cc',
        'gen/alljoyn/Status.cc'
      ],
    },
    {
      'target_name': 'alljoyn',
      'product_prefix': 'lib',
      'type': 'static_library',
      'dependencies': [
        'alljoynstatus'
      ],
      'sources': [
        'alljoyn/alljoyn_core/src/AboutData.cc',
        'alljoyn/alljoyn_core/src/AboutIcon.cc',
        'alljoyn/alljoyn_core/src/AboutIconObj.cc',
        'alljoyn/alljoyn_core/src/AboutIconProxy.cc',
        'alljoyn/alljoyn_core/src/AboutObj.cc',
        'alljoyn/alljoyn_core/src/AboutObjectDescription.cc',
        'alljoyn/alljoyn_core/src/AboutProxy.cc',
        'alljoyn/alljoyn_core/src/AllJoynCrypto.cc',
        'alljoyn/alljoyn_core/src/AllJoynPeerObj.cc',
        'alljoyn/alljoyn_core/src/AllJoynStd.cc',
        'alljoyn/alljoyn_core/src/AuthMechLogon.cc',
        'alljoyn/alljoyn_core/src/AuthMechPIN.cc',
        'alljoyn/alljoyn_core/src/AuthMechRSA.cc',
        'alljoyn/alljoyn_core/src/AuthMechSRP.cc',
        'alljoyn/alljoyn_core/src/AutoPinger.cc',
        'alljoyn/alljoyn_core/src/AutoPingerInternal.cc',
        'alljoyn/alljoyn_core/src/BusAttachment.cc',
        'alljoyn/alljoyn_core/src/BusEndpoint.cc',
        'alljoyn/alljoyn_core/src/BusObject.cc',
        'alljoyn/alljoyn_core/src/BusUtil.cc',
        'alljoyn/alljoyn_core/src/ClientRouter.cc',
        'alljoyn/alljoyn_core/src/ClientTransportShared.cc', #renamed to avoid conflict
        'alljoyn/alljoyn_core/src/CompressionRules.cc',
        'alljoyn/alljoyn_core/src/CredentialAccessor.cc',
        'alljoyn/alljoyn_core/src/DBusCookieSHA1.cc',
        'alljoyn/alljoyn_core/src/DBusStd.cc',
        'alljoyn/alljoyn_core/src/EndpointAuth.cc',
        'alljoyn/alljoyn_core/src/InterfaceDescription.cc',
        'alljoyn/alljoyn_core/src/KeyExchanger.cc',
        'alljoyn/alljoyn_core/src/KeyStore.cc',
        'alljoyn/alljoyn_core/src/LocalTransport.cc',
        'alljoyn/alljoyn_core/src/Message.cc',
        'alljoyn/alljoyn_core/src/Message_Gen.cc',
        'alljoyn/alljoyn_core/src/Message_Parse.cc',
        'alljoyn/alljoyn_core/src/MethodTable.cc',
        'alljoyn/alljoyn_core/src/MsgArg.cc',
        'alljoyn/alljoyn_core/src/NullTransport.cc',
        'alljoyn/alljoyn_core/src/PasswordManager.cc',
        'alljoyn/alljoyn_core/src/PeerState.cc',
        'alljoyn/alljoyn_core/src/ProtectedAuthListener.cc',
        'alljoyn/alljoyn_core/src/ProxyBusObject.cc',
        'alljoyn/alljoyn_core/src/RemoteEndpoint.cc',
        'alljoyn/alljoyn_core/src/Rule.cc',
        'alljoyn/alljoyn_core/src/SASLEngine.cc',
        'alljoyn/alljoyn_core/src/SessionOpts.cc',
        'alljoyn/alljoyn_core/src/SignalTable.cc',
        'alljoyn/alljoyn_core/src/SignatureUtils.cc',
        'alljoyn/alljoyn_core/src/SimpleBusListener.cc',
        'alljoyn/alljoyn_core/src/Transport.cc',
        'alljoyn/alljoyn_core/src/TransportList.cc',
        'alljoyn/alljoyn_core/src/XmlHelper.cc',
        'alljoyn/common/os/posix/atomic.cc',
        'alljoyn/common/os/posix/Condition.cc',
        'alljoyn/common/os/posix/Environ.cc',
        'alljoyn/common/os/posix/Event.cc',
        'alljoyn/common/os/posix/FileStream.cc',
        'alljoyn/common/os/posix/IfConfigDarwin.cc',
        'alljoyn/common/os/posix/IfConfigLinux.cc',
        'alljoyn/common/os/posix/Mutex.cc',
        'alljoyn/common/os/posix/OSLogger.cc',
        'alljoyn/common/os/posix/osUtil.cc',
        'alljoyn/common/os/posix/RWLock.cc',
        'alljoyn/common/os/posix/Socket.cc',
        'alljoyn/common/os/posix/Thread.cc',
        'alljoyn/common/os/posix/time.cc',
        'alljoyn/common/os/posix/UARTStreamDarwin.cc',
        'alljoyn/common/os/posix/UARTStreamLinux.cc',
        'alljoyn/common/src/ASN1.cc',
        'alljoyn/common/src/BigNum.cc',
        'alljoyn/common/src/BufferedSink.cc',
        'alljoyn/common/src/BufferedSource.cc',
        'alljoyn/common/src/CertificateECC.cc',
        'alljoyn/common/src/CommonGlobals.cc',
        'alljoyn/common/src/Config.cc',
        'alljoyn/common/src/Crypto.cc',
        'alljoyn/common/src/CryptoECC.cc',
        'alljoyn/common/src/CryptoSRP.cc',
        'alljoyn/common/src/Debug.cc',
        'alljoyn/common/src/GUID.cc',
        'alljoyn/common/src/IODispatch.cc',
        'alljoyn/common/src/IPAddress.cc',
        'alljoyn/common/src/KeyBlob.cc',
        'alljoyn/common/src/Logger.cc',
        'alljoyn/common/src/Pipe.cc',
        'alljoyn/common/src/SLAPPacket.cc',
        'alljoyn/common/src/SLAPStream.cc',
        'alljoyn/common/src/SocketStream.cc',
        'alljoyn/common/src/Stream.cc',
        'alljoyn/common/src/StreamPump.cc',
        'alljoyn/common/src/String.cc',
        'alljoyn/common/src/StringSource.cc',
        'alljoyn/common/src/StringUtil.cc',
        'alljoyn/common/src/ThreadPool.cc',
        'alljoyn/common/src/Timer.cc',
        'alljoyn/common/src/Util.cc',
        'alljoyn/common/src/XmlElement.cc',
        'alljoyn/common/crypto/openssl/CryptoAES.cc',
        'alljoyn/common/crypto/openssl/CryptoHash.cc',
        'alljoyn/common/crypto/openssl/CryptoRand.cc',
        'alljoyn/common/crypto/openssl/CryptoRSA.cc',
        'alljoyn/common/crypto/openssl/OpenSsl.cc'
      ],
      'conditions': [
        ['OS=="mac"', {
          'sources': [
            'alljoyn/alljoyn_core/src/darwin/ClientTransport.cc',
            'alljoyn/alljoyn_core/src/darwin/NamedPipeClientTransport.cc'
          ]
        }],
        ['OS=="linux"', {
          'sources': [
            'alljoyn/alljoyn_core/src/posix/ClientTransport.cc',
            'alljoyn/alljoyn_core/src/posix/NamedPipeClientTransport.cc'
          ]
        }]
      ]
    },
    {
      'target_name': 'demo_action',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        'src/demo_action.cc',
      ],
    }
  ]
}
