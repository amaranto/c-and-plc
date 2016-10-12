
#define LPC_UART LPC_USART2
#define UARTx_IRQn  USART2_IRQn

int UART_Read(LPC_USART_T *pUART, void *data, int numBytes);
int UART_ReadBlocking(LPC_USART_T *pUART, void *data, int numBytes);
int UART_SendBlocking(LPC_USART_T *pUART, void *data, int numBytes);

void UART_Init (void);
