#include "board.h"
#include "stdint.h"
#include "inttypes.h"
#include "../inc/dma.h"


void App_DMA_Init(void)
{
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);
}

void App_DMA_DeInit(void)
{
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumTx);
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumRx);
	NVIC_DisableIRQ(DMA_IRQn);
}

/*
void App_Interrupt_Init(void)
{
	Chip_UART_IntEnable(LPC_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	// Before using the ring buffers, initialize them using the ring
	//   buffer init function
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_RB_SIZE);

	// Enable Interrupt for UART channel
	// Priority = 1
	NVIC_SetPriority(UARTx_IRQn, 1);
	// Enable Interrupt for UART channel
	NVIC_EnableIRQ(UARTx_IRQn);
}

void App_Interrupt_DeInit(void)
{
	// Disable UART Rx & line status interrupts
	Chip_UART_IntDisable(LPC_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	// Disable Interrupt for UART channel
	NVIC_DisableIRQ(UARTx_IRQn);
} */

void App_DMA_Test(void)
{
	uint8_t receiveBuffer[16];

	App_DMA_Init();
	dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, _GPDMA_CONN_UART_Tx);

	isDMATx = ENABLE;
	channelTC = channelTCErr = 0;
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx,
					  (uint32_t) &"uar DMA test",
					  _GPDMA_CONN_UART_Tx,
					  GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
					  sizeof("uar DMA test"));
	//while (!channelTC);

	dmaChannelNumRx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, _GPDMA_CONN_UART_Rx);
	isDMATx = DISABLE;
	channelTC = channelTCErr = 0;
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumRx,
					  _GPDMA_CONN_UART_Rx,
					  (uint32_t) &receiveBuffer[0],
					  GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
					  10);
	//while (!channelTC);

	isDMATx = ENABLE;
	channelTC = channelTCErr = 0;
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx,
					  (uint32_t) &receiveBuffer[0],
					  _GPDMA_CONN_UART_Tx,
					  GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
					  10);
	//while (!channelTC);

	App_DMA_DeInit();
}

