var alljoyn_action = require('./');

var my_dev = 0;
var my_itf = 0;
var my_action = 0;

// Device Name
var MyStatus = alljoyn_action.findAlljoynServices();
if (MyStatus) {
    console.log('init: Err ' + MyStatus);
    return;
}

var dev_num = alljoyn_action.getDeviceName();
console.log('dev_num = ' + dev_num);

for (var i=0; i < dev_num; i++) {
    console.log('Device Name [' + i + '] = "' + alljoyn_action.getDeviceName(i) + '"');
}

// Interface
MyStatus = alljoyn_action.updateInterface(my_dev);
if (MyStatus) {
    console.log('updateInterface: Err ' + MyStatus);
    return;
}

var itf_num = alljoyn_action.getInterfaceName();
console.log('itf_num = ' + itf_num);

for (var i=0; i < itf_num; i++) {
    console.log('Interface Name [' + i + '] = "' + alljoyn_action.getInterfaceName(i) + '"');
}

// Action
var act_num = alljoyn_action.getActionName(my_itf);
console.log('act_num = ' + act_num);

for (var i=0; i < act_num; i++) {
    console.log('Action [' + i + '] = "' + alljoyn_action.getActionName(my_itf, i) + '"');
}

// Do Action
MyStatus = alljoyn_action.doAction(my_dev, my_itf, my_action);
if (MyStatus)
    console.log('doAction: Err ' + MyStatus);
