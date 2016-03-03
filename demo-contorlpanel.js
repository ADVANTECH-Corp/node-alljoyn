var alljoyn_action = require('./');

var my_dev = 0;
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

alljoyn_action.updateControlPanel(my_dev);
var con_num = alljoyn_action.getControlPanel();
console.log('con_num = ' + con_num);           
for (var i=0; i < con_num; i++) {
    console.log('ControlPanel [' + i + '] = "' + alljoyn_action.getControlPanel(my_dev,i) + '"');
    alljoyn_action.updateProperty(i);
		console.log('Property [' + i + '] = "' + alljoyn_action.getProperty(i) + '"');
}
//console.log('Property [' + '] = "' + alljoyn_action.getProperty(3) + '"');
//console.log('Property [' + '] = "' + alljoyn_action.setProperty("{{\"index\":\"0\", \"value\":\"0\"},{\"index\":\"2\", \"value\":\"0\"}}") + '"');