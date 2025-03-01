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