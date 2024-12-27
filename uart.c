#include "uart.h"

#ifdef _USE_UART

#ifdef _USE_USB_CDC
#include "cdc.h"
#endif

#include "usart.h"
#include "qbuffer.h"

static bool is_open[UART_MAX_CH];
static qbuffer_t qbuffer[UART_MAX_CH];

#ifdef _UART_CH1
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
static uint8_t rx_buf1[256];
#endif

#ifdef _UART_CH2
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
static uint8_t rx_buf2[256];
#endif

#ifdef _UART_CH3
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
static uint8_t rx_buf3[256];
#endif

#ifdef _UART_CH4
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart4_rx;
static uint8_t rx_buf4[256];
#endif

bool uartInit(void)
{
   for (int i = 0; i < UART_MAX_CH; i++)
   {
      is_open[i] = false;
   }

   return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
   if (ch >= UART_MAX_CH)
      return false;

   bool ret = false;
   is_open[ch] = true;

   UART_HandleTypeDef *huart = NULL;
   DMA_HandleTypeDef *hdma_usart_rx = NULL;
   uint8_t *rx_buf;

   switch (ch)
   {
#ifdef _UART_CH1
   case _UART_CH1:
      huart = &huart1;
      hdma_usart_rx = &hdma_usart1_rx;
      rx_buf = &rx_buf1;
      break;
#endif
#ifdef _UART_CH2
   case _UART_CH2:
      huart = &huart2;
      hdma_usart_rx = &hdma_usart2_rx;
      rx_buf = &rx_buf2;
      break;
#endif
#ifdef _UART_CH3
   case _UART_CH3:
      huart = &huart3;
      hdma_usart_rx = &hdma_usart3_rx;
      rx_buf = &rx_buf3;
      break;
#endif
#ifdef _UART_CH4
   case _UART_CH4:
      huart = &huart4;
      hdma_usart_rx = &hdma_usart4_rx;
      rx_buf = &rx_buf4;
      break;
#endif
   default:
      return false;
   }

   qbufferCreate(&qbuffer[ch], &rx_buf, 256);

   if (HAL_UART_Receive_DMA(huart, (uint8_t *)&rx_buf, 256) != HAL_OK)
   {
      ret = false;
   }
   else
   {
      ret = true;
   }

   qbuffer[ch].in = qbuffer[ch].len - hdma_usart_rx->Instance->NDTR;
   qbuffer[ch].out = qbuffer[ch].in;

   return ret;
}

uint32_t uartAvailable(uint8_t ch)
{
   uint32_t ret = 0;

   DMA_HandleTypeDef *hdma_usart_rx = NULL;

   switch (ch)
   {
#ifdef _UART_CH1
   case _UART_CH1:
      hdma_usart_rx = &hdma_usart1_rx;
      break;
#endif
#ifdef _UART_CH2
   case _UART_CH2:
      hdma_usart_rx = &hdma_usart2_rx;
      break;
#endif
#ifdef _UART_CH3
   case _UART_CH3:
      hdma_usart_rx = &hdma_usart3_rx;
      break;
#endif
#ifdef _UART_CH4
   case _UART_CH4:
      hdma_usart_rx = &hdma_usart4_rx;
      break;
#endif
   default:
      return false;
   }

   qbuffer[ch].in = (qbuffer[ch].len - hdma_usart_rx->Instance->NDTR);
   ret = qbufferAvailable(&qbuffer[ch]);

   return ret;
}

uint8_t uartRead(uint8_t ch)
{
   uint8_t ret = 0;

   qbufferRead(&qbuffer[ch], &ret, 1);

   return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
   uint32_t ret = 0;

   UART_HandleTypeDef *huart = NULL;

   switch (ch)
   {
#ifdef _UART_CH1
   case _UART_CH1:
      huart = &huart1;
      break;
#endif
#ifdef _UART_CH2
   case _UART_CH2:
      huart = &huart2;
      break;
#endif
#ifdef _UART_CH3
   case _UART_CH3:
      huart = &huart3;
      break;
#endif
#ifdef _UART_CH4
   case _UART_CH4:
      huart = &huart4;
      break;
#endif
   default:
      return false;
   }

   if (HAL_UART_Transmit(huart, p_data, length, 100) == HAL_OK)
   {
      ret = length;
   }

   return ret;
}

uint32_t uartPrintf(uint8_t ch, char *fmt, ...)
{
   char buf[256];
   va_list args;
   int len;
   uint32_t ret;

   va_start(args, fmt);
   len = vsnprintf(buf, 256, fmt, args);

   ret = uartWrite(ch, (uint8_t *)buf, len);

   va_end(args);

   return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
   uint32_t ret = 0;

   UART_HandleTypeDef *huart = NULL;

   switch (ch)
   {
#ifdef _UART_CH1
   case _UART_CH1:
      huart = &huart1;
      break;
#endif
#ifdef _UART_CH2
   case _UART_CH2:
      huart = &huart2;
      break;
#endif
#ifdef _UART_CH3
   case _UART_CH3:
      huart = &huart3;
      break;
#endif
#ifdef _UART_CH4
   case _UART_CH4:
      huart = &huart4;
      break;
#endif
   default:
      return false;
   }

   ret = huart->Init.BaudRate;

   return ret;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
#if 0
   if (huart->Instance == USART1)
   {
   }
#endif
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if 0
  if (huart->Instance == USART1)
  {
    qbufferWrite(&qbuffer[_DEF_UART2], &rx_data[_DEF_UART2], 1);

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_data[_DEF_UART2], 1);
  }
#endif
}

#endif