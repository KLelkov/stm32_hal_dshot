/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dshot.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t my_motor_value = 0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


void initSysTick(void) {
    // Assuming system clock is 72 MHz
    SysTick_Config(SystemCoreClock / 1000000); // Configure for 1 us ticks
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define DMA_SIZE 32
volatile uint32_t captureBuffer[DMA_SIZE]; // Buffer for captured values
volatile uint32_t captureIndex = 0; // Current index in buffer
volatile float dutyCycle = 0;
volatile float frequency = 0;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == B1_Pin)
	{
		HAL_GPIO_TogglePin (GPIOA, GPIO_PIN_5);
		my_motor_value = (my_motor_value + 50) % 2000;
		uint8_t MSG[65] = {'\0'};
	  sprintf(MSG, "Button pressed\n");
	  HAL_UART_Transmit(&huart6, MSG, sizeof(MSG), 30);

	}
	else if (GPIO_Pin == GPIO_PIN_0)
	{
		int a = 0;
	}
}


// uart read
//void HAL_TIM_IC_DMA_TransferComplete(TIM_HandleTypeDef *htim) {
//    if (htim->Instance == TIM5) {
//        // Process captured values
//    	char MSG[DMA_SIZE * 3 + 20] = {'\0'};
//		  char *ptr = MSG;
//        for (uint32_t i = 0; i < DMA_SIZE; i++) {
//            // Calculate duty cycle and frequency based on captured values
//            // Assuming captureBuffer[i] contains rising edges
//            // You can implement your logic here
//
//		  ptr += sprintf(ptr, "%u", captureBuffer[i]);
//		  // Optionally add a separator if it's not the last element
//		  if (i < DMA_SIZE - 1) {
//			  ptr += sprintf(ptr, " "); // Add a comma and space as a separator
//		  }
//
//        }
//        ptr += sprintf(ptr, "\n");
//        	//		  sprintf(MSG, "tick: %lu\n", duty);
//        			  HAL_UART_Transmit(&huart6, MSG, ptr - MSG, 150);
//        			memset(captureBuffer, 0, DMA_SIZE);
//    }
//}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART6_UART_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  dshot_init(DSHOT300, 0);

  uint8_t MSG[65] = {'\0'};
  sprintf(MSG, "DSHOT mode: %d\n", 300);
  HAL_UART_Transmit(&huart6, MSG, sizeof(MSG), 30);
  HAL_Delay(500);

//  HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1); // Primary channel - rising edge
//  HAL_TIM_IC_Start(&htim5, TIM_CHANNEL_2);    // Secondary channel - falling edge

//  HAL_TIM_Base_Start_IT(&htim3);
  uint32_t counter = 0;
//  HAL_UART_Receive_IT(&huart2, &received_byte, 1); // Start receiving data



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // transmit new dshot signals
	  dshot_write(my_motor_value);
//	  uint8_t MSG[65] = {'\0'};
//	  sprintf(MSG, "tick: %d\n", microseconds);
//	  HAL_UART_Transmit(&huart6, MSG, sizeof(MSG), 30);
	  HAL_Delay(1);

//	  counter++;
//	  if (counter == 1000)
//	  {
//		  counter = 0;
//		  uint8_t received_data[10] = {0}; // Adjust size as necessary
//		        HAL_UART_Receive_IT(&huart2, received_data, sizeof(received_data));
//
//		        uint8_t MSG2[20] = {'\0'};
//		            char *ptr = MSG2;
//		        for (size_t i = 0; i < 9; i++)
//		    	{
//		    	  ptr += sprintf(ptr, "%d", received_data[i]);
//		    	  ptr += sprintf(ptr, " "); // Add a comma and space as a separator
//		    	}
//		        ptr += sprintf(ptr, "\n"); // Add a comma and space as a separator
//		    	HAL_UART_Transmit(&huart6, MSG2, sizeof(MSG2), 30);
//	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
