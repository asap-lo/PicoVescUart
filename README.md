# PicoVescUart

This is an Raspberry Pi Pico specific implementation of the
[bldc](http://vedder.se/2015/10/communicating-with-the-vesc-using-uart/) interface,
which is a programmatical interface for the VESC motor controller.

## Dev Setup

Here are some platform specific setup instructions:

### Windows (11)

TODO

### Linux (Ubuntu)

TODO

### MacOS (Sonoma 14.1.2)

Requires [Homebrew](https://brew.sh)

```sh
brew install arm-none-eabi-gcc cmake picotool
```

## Build Instructions

### First time setup

Open a terminal in the repository root, and run the following commands:

```sh
mkdir build
cd build
cmake ..
```

### Building Software

After above setup, you should be able to run `make` within the build directory, and
your executable will be built.

## Initializing

Below is a small guide on how to initialize this interface in your own code.

Use the following recipes in some `*.c` file in the repository root:

### Initializing without callback function

```c
#include "PicoVescUart.h"

int main() {
    comm_uart_init(NULL);

    // Your application main loop/other code here!
}
```

### Initializing with packet receive callback function

This function is called by the `bldc_interface` on every packet receipt. This is not a
requirement, but will likely be useful in many cases for handling data coming from the
motor controller.

Make your callback function like this:

```c
#include "PicoVescUart.h"

void on_value_received(mc_values *val)
{
    current_data_values = *val;
    
    // Handle values here! See struct below.
}

int main() {
    comm_uart_init(on_value_received); // pass function as a parameter. 

    // Your application main loop/other code here!
}
```

<details>
  <summary>mc_values struct from vesc/datatypes.h</summary>

```c
typedef struct {
	float v_in;
	float temp_mos;
	float temp_motor;
	float current_motor;
	float current_in;
	float id;
	float iq;
	float rpm;
	float duty_now;
	float amp_hours;
	float amp_hours_charged;
	float watt_hours;
	float watt_hours_charged;
	int tachometer;
	int tachometer_abs;
	mc_fault_code fault_code;
	float pid_pos;
	uint8_t vesc_id;
} mc_values;
```

</details>

## Usage

There are also many getter and setter methods, which are listed below and in
`(vesc/bldc_interface.h)`

### Getters

From `(vesc/bldc_interface.h)`

```c
// Getters
void bldc_interface_get_fw_version(void);
void bldc_interface_get_values(void);
void bldc_interface_get_mcconf(void);
void bldc_interface_get_appconf(void);
void bldc_interface_get_decoded_ppm(void);
void bldc_interface_get_decoded_adc(void);
void bldc_interface_get_decoded_chuk(void);
```

### Setters

From `(vesc/bldc_interface.h)`

```c
// Setters
void bldc_interface_terminal_cmd(char* cmd);
void bldc_interface_set_duty_cycle(float dutyCycle);
void bldc_interface_set_current(float current);
void bldc_interface_set_current_brake(float current);
void bldc_interface_set_rpm(int rpm);
void bldc_interface_set_pos(float pos);
void bldc_interface_set_handbrake(float current);
void bldc_interface_set_servo_pos(float pos);
void bldc_interface_set_mcconf(const mc_configuration *mcconf);
void bldc_interface_set_appconf(const app_configuration *appconf);
```
