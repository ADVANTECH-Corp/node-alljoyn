# node-alljoyn
Node.js Integration for Alljoyn Service Functions

## Installation
``` bash
$ npm install node-alljoyn
```

## Usage
We plan to provide three Alljoyn functions in this module.
 - Action Handler
 - Event Reader & Rule Engine
 - ControlPanel Client

Now, we only support Action.

### Example

This sample prints the device name from your Alljoyn network.

``` javascript
var alljoyn = require('node-alljoyn');

var result = alljoyn.findAlljoynServices();
if (result == 0) {
  var dev_num = alljoyn.getDeviceName();
  
  for (var i=0; i < dev_num; i++) {
    console.log('Device Name [' + i + '] = "' + alljoyn.getDeviceName(i) + '"');
  }
}
```

## Test Platform
- Linux (Yocto 1.5)

## Alljoyn Version
**v14.06**  
* The Alljoyn API integration is leveraged from [alljoyn](https://www.npmjs.com/package/alljoyn).

## License
Copyright 2015 ADVANTECH Corp. under [the Apache 2.0 license](LICENSE).
