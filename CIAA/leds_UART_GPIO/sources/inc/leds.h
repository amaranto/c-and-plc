#define LED1 0
#define LED2 1
#define LED3 2
#define LEDR 3
#define LEDG 4
#define LEDB 5

#define LED1_CTOUT 2
#define LED2_CTOUT 5
#define LED3_CTOUT 4

void LEDS_InitCTOUT( void );
void LED_Toggle(uint8_t LEDNumber);
bool LED_Test(uint8_t LEDNumber);
void LED_Set(uint8_t LEDNumber, bool On);
