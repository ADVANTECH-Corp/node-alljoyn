var alljoyn_action = require('./');

var my_dev = 1;
var my_itf = 0;
var my_action = 0;

// Device Name
var MyStatus = alljoyn_action.findControlPanelServices();
if (MyStatus) {
    console.log('init: Err ' + MyStatus);
    return;
}

var dev_num = alljoyn_action.getControlPanelDeviceName();
console.log('dev_num = ' + dev_num);
for (var i=0; i < dev_num; i++) {
    console.log('Device Name [' + i + '] = "' + alljoyn_action.getControlPanelDeviceName(i) + '"');
}

var con_num = alljoyn_action.getControlPanel(my_dev);
console.log('con_num = ' + con_num);
for (var i=0; i < con_num; i++) {
    //console.log('ControlPanel [' + i + '] = "' + alljoyn_action.getControlPanel(my_dev,i) + '"');
		console.log('Property [' + i + '] = "' + alljoyn_action.getProperty(i) + '"');
}
console.log('Property [' + '] = "' + alljoyn_action.getProperty(3) + '"');
console.log('Property [' + '] = "' + alljoyn_action.setProperty("{{\"index\":\"0\", \"value\":\"0\"},{\"index\":\"2\", \"value\":\"0\"}}") + '"');
/*
var con_num1 = alljoyn_action.getControlPanel(my_dev);
console.log('con_num1 = ' + con_num1);
for (var i=0; i < con_num1; i++) {
    //console.log('ControlPanel [' + i + '] = "' + alljoyn_action.getControlPanel(my_dev,i) + '"');
		console.log('Property [' + i + '] = "' + alljoyn_action.setProperty(i) + '"');
}
*/
//console.log('ControlPanel [' + i + '] = "' + alljoyn_action.getControlPanel(my_dev,4) + '"');

//console.log('Property [' + i + '] = "' + alljoyn_action.getProperty(3) + '"');
/*
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
*/