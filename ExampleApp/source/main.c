
#include "uart.h"
#include "wait.h"

int main(void)
{
	waitMs(100);
    char buffer[50];
    uint16_t baudrate = 115200;

    uart0Init(baudrate);  // Initialize uart0 with 115200 baud
    uart0WriteLine("TinyK22 Ready...\r\n");

    while (1)
    {
        // Read a line from Raspberry Pi
        if (uart0HasLineReceived())
        {
            uart0ReadLine(buffer, sizeof(buffer));
            uart0WriteLine("Received: ");
            uart0WriteLine(buffer);
            uart0WriteLine("\r\n");
        }
    }
}
