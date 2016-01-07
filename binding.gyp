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
       'alljoyn/services/about/cpp/inc',
       'alljoyn/services/controlpanel/cpp/inc',
       #'alljoyn/services/notification/cpp/inc',
       'alljoyn/services/sample_apps/cpp/samples_common',
       'alljoyn/services/services_common/cpp/inc',
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
        'alljoyn',
        'alljoyn_services'
      ],
      'sources': [
        'src/ControllerUtil.cc',
        'src/ControlPanelListenerImpl.cc',
        'src/ControlPanelHandler.cc',
        'src/EventHandler.cc',
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
      'target_name': 'alljoyn_services',
      'product_prefix': 'lib',
      'type': 'static_library',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        # About
        'alljoyn/services/about/cpp/src/AboutClient.cc',
        'alljoyn/services/about/cpp/src/AboutIconClient.cc',
        'alljoyn/services/about/cpp/src/AboutIconService.cc',
        'alljoyn/services/about/cpp/src/AboutPropertyStoreImpl.cc',
        'alljoyn/services/about/cpp/src/AboutServiceApi.cc',
        'alljoyn/services/about/cpp/src/AboutService.cc',
        'alljoyn/services/about/cpp/src/AnnounceHandler.cc',
        'alljoyn/services/about/cpp/src/AnnouncementRegistrar.cc',
        'alljoyn/services/about/cpp/src/InternalAnnounceHandler.cc',
        'alljoyn/services/about/cpp/src/PropertyStoreProperty.cc',
        # Notification
        #'alljoyn/services/notification/cpp/src/NotificationAnnounceListener.cc',
        #'alljoyn/services/notification/cpp/src/NotificationAsyncTaskEvents.cc',
        #'alljoyn/services/notification/cpp/src/Notification.cc',
        #'alljoyn/services/notification/cpp/src/NotificationConstants.cc',
        #'alljoyn/services/notification/cpp/src/NotificationDismisser.cc',
        #'alljoyn/services/notification/cpp/src/NotificationDismisserReceiver.cc',
        #'alljoyn/services/notification/cpp/src/NotificationDismisserSender.cc',
        #'alljoyn/services/notification/cpp/src/NotificationEnums.cc',
        #'alljoyn/services/notification/cpp/src/NotificationProducer.cc',
        #'alljoyn/services/notification/cpp/src/NotificationProducerListener.cc',
        #'alljoyn/services/notification/cpp/src/NotificationProducerReceiver.cc',
        #'alljoyn/services/notification/cpp/src/NotificationProducerSender.cc',
        #'alljoyn/services/notification/cpp/src/NotificationReceiver.cc',
        #'alljoyn/services/notification/cpp/src/NotificationSender.cc',
        #'alljoyn/services/notification/cpp/src/NotificationService.cc',
        #'alljoyn/services/notification/cpp/src/NotificationText.cc',
        #'alljoyn/services/notification/cpp/src/NotificationTransport.cc',
        #'alljoyn/services/notification/cpp/src/NotificationTransportConsumer.cc',
        #'alljoyn/services/notification/cpp/src/NotificationTransportProducer.cc',
        #'alljoyn/services/notification/cpp/src/NotificationTransportSuperAgent.cc',
        #'alljoyn/services/notification/cpp/src/PayloadAdapter.cc',
        #'alljoyn/services/notification/cpp/src/RichAudioUrl.cc',
        #'alljoyn/services/notification/cpp/src/SuperAgentBusListener.cc',
        #'alljoyn/services/notification/cpp/src/Transport.cc',
        # ControlPanel
        'alljoyn/services/controlpanel/cpp/src/BusObjects/ActionBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/ContainerBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/ControlPanelBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/DialogBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/HttpControlBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/IntrospectionNode.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/LabelBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/NotificationActionBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/PropertyBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/WidgetBusObject.cc',
        'alljoyn/services/controlpanel/cpp/src/BusObjects/WidgetProxyBusObjectListener.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelBusListener.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanel.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelControllee.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelControlleeUnit.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelController.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelControllerUnit.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelDevice.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelService.cc',
        'alljoyn/services/controlpanel/cpp/src/ControlPanelSessionHandler.cc',
        'alljoyn/services/controlpanel/cpp/src/LanguageSet.cc',
        'alljoyn/services/controlpanel/cpp/src/LanguageSets.cc',
        'alljoyn/services/controlpanel/cpp/src/NotificationAction.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/Action.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/ActionWithDialog.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/ConstraintList.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/ConstraintRange.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/Container.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/CPSDate.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/CPSTime.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/Dialog.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/ErrorWidget.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/HttpControl.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/Label.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/Property.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/RootWidget.cc',
        'alljoyn/services/controlpanel/cpp/src/Widgets/Widget.cc',
        # Common
        'alljoyn/services/sample_apps/cpp/samples_common/AboutDataStore.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/AboutObjApi.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/AnnounceHandlerImpl.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/AsyncSessionJoiner.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/CommonBusListener.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/CommonSampleUtil.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/SessionListenerImpl.cc',
        'alljoyn/services/sample_apps/cpp/samples_common/SrpKeyXListener.cc',
        'alljoyn/services/services_common/cpp/src/AsyncTaskQueue.cc',
        'alljoyn/services/services_common/cpp/src/GuidUtil.cc'
        
        # Event
        'alljoyn/services/config/cpp/src/'
      ],
    },
    {
      'target_name': 'ControlPanelController',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn',
        'alljoyn_services'
      ],
      'sources': [
        'src/ControllerUtil.cc',
        'src/ControlPanelListenerImpl.cc',
        'src/ControlPanelControllerSample.cc'
      ],
    },
    {    
      'target_name': 'demo_action',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        'src/demo_action.cc'
      ],
    },
    {    
      'target_name': 'demo_event',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        'src/demo_event.cc'
      ],
    },    
    {    
      'target_name': 'demo_eventHandler',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        'src/Rule.cc',
        'src/demo_eventHandler.cc'
      ],
    },
    {    
      'target_name': 'EventDaemon',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        'src/Rule.cc',
        'src/EventDaemon.cc'
      ],
    }    
  ]
}
