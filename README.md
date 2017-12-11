# Matrix 433Mhz Mqtt client

Mqtt client which allows to switch 433Mhz devices using RC-Switch.
Based on the following example: https://mosquitto.org/man/libmosquitto-3.html

## Usage

Run the client and send group and device codes to the configured Mqtt topic pattern.

Current topic pattern is: matrix/433/set/`<group>`/`<device>` (because I'm using it with [Matrix Creator](https://creator.matrix.one))

Example: matrix/433/set/11111/00001

Codes are as given in https://github.com/sui77/rc-switch/wiki/HowTo_OperateLowCostOutlets

## Build

* Clone [RC-Switch](https://github.com/sui77/rc-switch) or [my fork](https://github.com/draity/rc-switch) if usage with Matrix Creator is intended
* Install dependecies: mosquitto, wiringPi
* If MatrixCreator: get or build matrix_creator_hal library
* Set appropriate defines for RC-Switch, e.g. RPI (optionally MatrixCreator)
* Build using the following command:

`g++ -Wall -g -D RPI -D MatrixCreator -I ../rc-switch/ -o 433_mqtt_client  ../rc-switch/RCSwitch.cpp matrix-433-mqtt-client.cpp -L ../matrix-creator-hal/build/demos/driver -l matrix_creator_hal -l wiringPi -l mosquitto`
