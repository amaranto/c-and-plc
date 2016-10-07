#define UART_RB_SIZE 256
#define LPC_UART LPC_USART2
#define UARTx_IRQn  USART2_IRQn
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART2_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART2_Rx

STATIC RINGBUFF_T txring, rxring;

static volatile uint32_t channelTC ,
						 channelTCErr;
static FunctionalState isDMATx = ENABLE;

uint8_t dmaChannelNumTx, dmaChannelNumRx;
uint8_t rxbuff[UART_RB_SIZE], txbuff[UART_RB_SIZE];

void App_DMA_Init(void);
void App_DMA_DeInit(void);
void App_Interrupt_Test(void);
void App_DMA_Test(void);


