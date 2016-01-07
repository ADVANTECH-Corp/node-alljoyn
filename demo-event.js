var alljoyn_event = require('./');

var my_dev = 0;
var my_itf = 0;
var my_event = 2;
var STATUS = {ERR : -1,	OK : 0} ;
//Socket
var net = require('net');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var exec = require('child_process').exec;
var HOST = "127.0.0.1";
var PORT_1 = 5000;
var PORT_2 = 5050;
///////////////////////////

function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}

function RestRespEmitter() 
{
    EventEmitter.call(this);
}
util.inherits(RestRespEmitter, EventEmitter);

RestRespEmitter.prototype.msgJsonObj = null;
RestRespEmitter.prototype.sigRecv = function sigRecv() 
{
    //console.log("fire Rest Recv");
    this.emit('recv');
};

function connect_socket(Data, port)
{
    var status = STATUS.OK; 
    var restRespEmitter = null;
    var client = null;
    var hasRestAnswer = -1;
    var msgJsonObj = null;
    
    restRespEmitter = new RestRespEmitter();
    client = new net.Socket();
    if (typeof(client) == "undefined" || client == null)
    {
       status = STATUS.ERR;
       console.log("connect_socket: no client");
    }
    else
    {
        function checkRestResult() {
      			if (hasRestAnswer == 1) {
                console.log("Send msg to next node !!");
      				hasRestAnswer = -1;
      			}
      
      			setImmediate(checkRestResult);
  		  }
                
        client.connect(port, HOST, function() {
            console.log("CONNECTED TO: " + HOST + ":" + port);
            client.write(Data);

            hasRestAnswer = 0;
            setImmediate(checkRestResult);
        }); 
        
        client.on('data', function(data) {
        	console.log('DATA from Server: ' + data);
          status =  data;
          //if (!status) //success
          //  client.write('CMD, '+'0001 '+'CLOSE');
        });
        
        client.on('error', function(e) {
        	console.log("!!! socket client error: " + e.code + "!!!");
        });
        
        client.on('close', function() {
            console.log("Connection closed");
        	//client.destroy();
        });                
      }  
    return status;
}

// Device Name
var MyStatus = alljoyn_event.findAlljoynEventServices();
if (MyStatus) {
    console.log('init: Err ' + MyStatus);
    return;
}

var dev_num = alljoyn_event.getEventDeviceName();
console.log('dev_num = ' + dev_num);

for (var i=0; i < dev_num; i++) {
    console.log('Device Name [' + i + '] = "' + alljoyn_event.getEventDeviceName(i) + '"');
}

// Interface
MyStatus = alljoyn_event.updateEventInterface(my_dev);
if (MyStatus) {
    console.log('updateInterface: Err ' + MyStatus);
    return;
}

var itf_num = alljoyn_event.getEventInterfaceName();
console.log('itf_num = ' + itf_num);

for (var i=0; i < itf_num; i++) {
    console.log('Interface Name [' + i + '] = "' + alljoyn_event.getEventInterfaceName(i) + '"');
}

// Event
var event_num = alljoyn_event.getEventName(my_itf);
console.log('event_num = ' + event_num);

for (var i=0; i < event_num; i++) {
    console.log('Event [' + i + '] = "' + alljoyn_event.getEventName(my_itf, i) + '"');
}

// Do Event
var JassonData = alljoyn_event.doEvent(my_dev, my_itf, my_event);
console.log('doEvent: ' + JassonData);

//MyStatus = connect_socket('GET, '+ '0001', PORT_2);

//Add Rule
MyStatus = connect_socket('REG, '+ '0001 ' + JassonData, PORT_1);
if (!MyStatus) {
    console.log('connect_socket: Err ' + MyStatus);
}

/*
//Query event-triger
console.log("Start Query...");
var interval = setInterval(function(){
connect_socket('GET, '+ '0001', PORT_2)
}, 2000);
*/