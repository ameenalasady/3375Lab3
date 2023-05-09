#define SW_BASE 0xFF200040
#define JP1_BASE 0xFF200060
#define ADC_BASE 0xFF204000

// Declare pointers for IO register
volatile unsigned int *SW_switch_ptr = (unsigned int *)SW_BASE;   // SW slider switch address  0xFF200040
volatile unsigned int *const GPIO_ptr = (unsigned int *)JP1_BASE; // Get GPIO (JP1 BASE, 0xFF200060)
volatile unsigned int *const ADC_ptr = (unsigned int *)ADC_BASE;  // Get ADC port (0xFF204000)
volatile int DELAY_LENGTH;

// Read the switches
int readSW(void)
{
    volatile unsigned int switchState = *SW_switch_ptr; // get the values stored in the switch
    switchState &= 0x1;                                 // AND Mask, only lowest bit
    return switchState;
}

void displayGPIO(int val)
{
    int lookupTable[10];

    // GPIO Pin numbering, seleect appropriate pin
    lookupTable[0] = 0x1;   // 0001
    lookupTable[1] = 0x3;   // 0011
    lookupTable[2] = 0x7;   // 0111
    lookupTable[3] = 0xF;   //        1111
    lookupTable[4] = 0x1F;  // 0001 1111
    lookupTable[5] = 0x3F;  // 0011 1111
    lookupTable[6] = 0x7F;  // 0111 1111
    lookupTable[7] = 0xFF;  // 1111 1111
    lookupTable[8] = 0x1FF; // 0001 1111 1111
    lookupTable[9] = 0x3FF; // 0011 1111 1111

    // Input value (translate voltage)
    if (val < 10)
    {
        *(GPIO_ptr) = lookupTable[val];
    }
    // Value loops back
    else
    {
        *(GPIO_ptr) &= 0x0;
    }
}

// Start of program
int main(void)
{

    // Set pin directions
    GPIO_ptr[1] |= 0x3FF; // Set as output bits (pins 0-9, lowest 10 bits are outputs)
    int delay;

    DELAY_LENGTH = 100; // Delay to prevent bouncing

    while (1)
    {
        if (readSW()) // if switch is on, read from first potentiometer
        {
            ADC_ptr[0] = 0x1; // Refresh channel

            // Read current ADC value (channel 0)
            volatile int value = ADC_ptr[0];
            value &= 0xFFF; // Only need lowest 12 bits

            // Display on GPIO pins (output)
            displayGPIO(value * 10 / 4096); // ADC 12 bits (4096 values), GPIO is 10 pins
        }

        else // read from the second potentiometer
        {
            ADC_ptr[1] = 0x1; // Refresh channel

            // Read current ADC value (channel 1)
            volatile int value = ADC_ptr[1];
            value &= 0xFFF; // Only need lowest 12 bits

            // Display on GPIO pins (output)
            displayGPIO(value * 10 / 4096); // ADC 12 bits (4096 values), GPIO is 10 pins
        }

        // Delay loop
        for (delay = DELAY_LENGTH; delay != 0; --delay)
        {
        };
    }
}