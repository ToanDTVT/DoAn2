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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include "KeyPad.h"
#include "KeyPadConfig.h"
#include "EEPROM.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_PASSWORD 5
#define MAX_FINGERPRINT 10
#define PASSWORD_SIZE 4
#define MAX_FINGERPRINT 10
#define PAGE_SIZE 16     // in Bytes
#define PAGE_NUM  2048    // number of pages
#define START_PAGE_FINGERPRINT 30
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
enum statemachine {
	STATE_IDLE,
	STATE_OPENDOOR,
	STATE_DISABLE,
	STATE_SETTING
};
volatile enum statemachine currentstate = STATE_IDLE;

enum option{
	SETTING_PASSWORD,
	SETTING_FINGERPRINT,
	ADD_PASSWORD,
	DELETE_PASSWORD,
	ADD_FINGERPRINT,
	DELETE_FINGERPRINT
};
volatile enum option setting_password;
volatile enum option setting_fingerprint;

volatile uint8_t interrupt_flag1 = 0;
volatile uint8_t interrupt_flag10 = 0;
volatile uint8_t interrupt_uart2 = 0;
volatile int number_of_failure = 0;

uint8_t password[3];
uint8_t new_password[3];
uint8_t old_password[3];
uint8_t new_fingerprint[10];
uint8_t old_fingerprint[10];
uint8_t fingerprint[10];

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int check_pass_to_enter(uint8_t *password){
	uint8_t passwword_eeprom[3];
	for(int page = 0; page <= MAX_PASSWORD; page++){
		EEPROM_Read(page, 0, passwword_eeprom, 10);
		for(int i = 0; i < PASSWORD_SIZE; i++){
			if(password[i] != passwword_eeprom[i]){
				break;
			}
			if(i == PASSWORD_SIZE - 1){
				return 1;
				break;
			}
		}
	}
	return 0;
}

int check_slot_password(uint8_t *password){
	uint8_t buffer[PAGE_SIZE - 1];
	int empty_page;
	if(check_pass_to_enter(password) == 1){
		lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" ALREADY EXIST !!");
		HAL_Delay(500);
		return 2;
	}
	for(int page = 0; page <= MAX_PASSWORD; page++){
		EEPROM_Read(page, 0, buffer, PAGE_SIZE - 1);
		for(int x = 0; x <= PAGE_SIZE - 1; x++){
			if(buffer[x] != 0xFF){
				break;
			}
			if(x == (PAGE_SIZE - 1)){
				empty_page = page;
				EEPROM_Write(empty_page, 0, password, 4);
				return 1;
			}
		}
	}
	return 0;
}

int delete_password(uint8_t *password){
	uint8_t passwword_eeprom[3];
	for(int page = 0; page <= MAX_PASSWORD; page++){
		EEPROM_Read(page, 0, passwword_eeprom, 10);
		for(int i = 0; i < PASSWORD_SIZE; i++){
			if(password[i] != passwword_eeprom[i]){
				break;
			}
			if(i == PASSWORD_SIZE - 1){
				EEPROM_PageErase(page);
				return 1;
				break;
			}
		}
	}
	lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string(" NOT EXIST !!");
    HAL_Delay(500);
	return 0;
}
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
  lcd_init();
  lcd_clear();
  KeyPad_Init();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  unsigned char press_keypad = KEYPAD_GetChar();
	  	  HAL_Delay(300);
	  	  switch(currentstate){
	  	  case STATE_IDLE:
	  		  lcd_clear();
	  		  lcd_put_cur(0, 0);
	  		  lcd_send_string("  PASSWORD");
	  		  lcd_put_cur(1, 0);
	  		  int i = 0;
	  		  while(1){
	  			  if(interrupt_flag1 == 1) break;												//�?ỌC CỜ NGẮT
	  			  if(interrupt_flag10 == 1) break;												//�?ỌC CỜ NGẮT
	  			  press_keypad = KEYPAD_GetChar();
	  			  if(press_keypad != 0){
	  				  lcd_send_data(press_keypad);
	  				  HAL_Delay(300);
	  				  password[i] = press_keypad;
	  				  i++;
	  				  if(i == 4){
	  					  if(check_pass_to_enter(password) == 1){
	  					   currentstate = STATE_OPENDOOR;                           //NẾU �?ÚNG THÌ MỞ CỬA
	  					   HAL_Delay(100);
	  					   lcd_clear();
	  					   lcd_put_cur(0, 0);
	  					   lcd_send_string("  SUCCESS !!");
	  					   lcd_put_cur(1, 0);
	  					   lcd_send_string("OPEN THE DOOR");
	  					   HAL_Delay(1000);
	  					   number_of_failure = 0;
	  					   lcd_clear();
	  					 }
	  					 if(check_pass_to_enter(password) == 0){
	  					   currentstate = STATE_IDLE;                           //NẾU SAI THÌ THÔI
	  					   HAL_Delay(100);
	  					   lcd_clear();
	  					   lcd_put_cur(0, 0);
	  					   lcd_send_string("  FAILED !!");
	  					   HAL_Delay(1000);
	  					   number_of_failure++;
	  					   if(number_of_failure == 5){
	  						   currentstate = STATE_DISABLE;
	  					   }
	  					   lcd_clear();
	  					   break;
	  					 }
	  				  }
	  			  }
	  		  }
	  		  break;

	  	  case STATE_DISABLE:
	  		  int y = 30;
	  		  for(int x = 0; x <= 30; x++){
	  			  y--;
	  			  lcd_clear();
	  			  lcd_put_cur(0, 0);
	  			  lcd_send_string("DISABLE");
	  			  lcd_put_cur(1, 0);
	  			  lcd_send_data(y + '0');
	  			  HAL_Delay(300);
	  		  }
	  		  currentstate = STATE_IDLE;
	  		  break;

	  	  case STATE_SETTING:
	  		  HAL_Delay(100);
	  			 interrupt_flag1 = 0;
	  			 lcd_put_cur(0, 0);
	  			 lcd_send_string("SETTING: 1.PASSWORD");
	  			 lcd_put_cur(1, 0);
	  			 lcd_send_string("         2.FINGERPRINT");
	  			 if(press_keypad == '1'){
	  				 HAL_Delay(200);  lcd_clear(); setting_password = SETTING_PASSWORD;
	  				 while(1){
	  					 if(interrupt_flag1 == 1) break;												//�?ỌC CỜ NGẮT
	  					 if(interrupt_flag10 == 1) break;												//�?ỌC CỜ NGẮT
	  					 switch(setting_password){
	  					 case SETTING_PASSWORD:
	  						 HAL_Delay(500);
	  						 lcd_put_cur(0, 0);
	  						 lcd_send_string("1. ADD PASS" );
	  						 lcd_put_cur(1, 0);
	  						 lcd_send_string("2. DELETE PASS");
	  						 press_keypad = KEYPAD_GetChar();
	  						 if(press_keypad == '1'){HAL_Delay(500);  lcd_clear(); setting_password = ADD_PASSWORD;}
	  						 if(press_keypad == '2'){HAL_Delay(500);  lcd_clear(); setting_password = DELETE_PASSWORD;}
	  						 break;

	  					 case ADD_PASSWORD:
	  						 try_again1:
	  						 lcd_clear();
	  						 lcd_put_cur(0, 0);
	  						 lcd_send_string("NEW PASSWORD");
	  						 lcd_put_cur(1, 0);
	  						 int t = 0;
	  						 while(1){
	  							 if(interrupt_flag1 == 1) break;												//�?ỌC CỜ NGẮT
	  							 if(interrupt_flag10 == 1) break;												//�?ỌC CỜ NGẮT
	  							 press_keypad = KEYPAD_GetChar();
	  							 HAL_Delay(500);
	  							 if(press_keypad != 0){
	  								 lcd_send_data(press_keypad);
	  								 new_password[t] = press_keypad;
	  								 t++;
	  								 press_keypad = KEYPAD_GetChar();
	  								 if(press_keypad == '#'){
	  									 if(t <= 4){
	  										 lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" TRY AGAIN !!");
	  										 HAL_Delay(500);
	  										 goto try_again1;
	  									 }
	  									 if(t == 5){
	  										 if(check_slot_password(new_password) == 1){
	  											 lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" SUCCESS !!");
	  											 HAL_Delay(500);
	  											 break;
	  										 }
	  										 else {
	  											 lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" UNSUCCESS !!");
	  											 HAL_Delay(500);
	  											 break;
	  										 }
	  									 }
	  								 }

	  							 }
	  						 }

	  						 break;

	  					 case DELETE_PASSWORD:
	  						 try_again2:
	  						 lcd_clear();
	  						 lcd_put_cur(0, 0);
	  						 lcd_send_string("OLD PASSWORD");
	  						 lcd_put_cur(1, 0);
	  						 int e = 0;
	  						 while(1){
	  							 if(interrupt_flag1 == 1) break;												//�?ỌC CỜ NGẮT
	  							 if(interrupt_flag10 == 1) break;												//�?ỌC CỜ NGẮT
	  							 if(press_keypad != 0){
	  								 lcd_send_data(press_keypad);
	  								 old_password[e] = press_keypad;
	  								 e++;
	  								 if((press_keypad == '#') && (e<4)){
	  									 lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" TRY AGAIN !!");
	  									 HAL_Delay(500);
	  									 goto try_again2;
	  								 }
	  								 if((press_keypad == '#') && (e == 4)){
	  									 if(delete_password(old_password) == 1){
	  										 lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" SUCCESS !!");
	  										 HAL_Delay(500);
	  										 break;
	  									 }
	  									 else{
	  										 lcd_clear(); lcd_put_cur(0, 0); lcd_send_string(" UNSUCCESS !!");
	  										 HAL_Delay(500);
	  										 break;
	  									 }
	  								 }
	  							 }
	  						 }
	  						 break;
	  					 }
	  				 }

	  			 }
	  			 if(press_keypad == '2'){
	  				 HAL_Delay(200);  lcd_clear(); setting_fingerprint = SETTING_FINGERPRINT;
	  				 while(1){
	  					 if(interrupt_flag1 == 1) break;												//�?ỌC CỜ NGẮT
	  					 if(interrupt_flag10 == 1) break;												//�?ỌC CỜ NGẮT
	  					 switch(setting_fingerprint){
	  					 case SETTING_FINGERPRINT:
	  						 HAL_Delay(100);
	  						 lcd_put_cur(0, 0);
	  						 lcd_send_string("1. ADD FINGERPRINT" );
	  						 lcd_put_cur(1, 0);
	  						 lcd_send_string("2. DELETE FINGERPRINT");
	  						 if(press_keypad == '1'){HAL_Delay(200);  lcd_clear(); setting_fingerprint = ADD_FINGERPRINT;}
	  						 if(press_keypad == '2'){HAL_Delay(200);  lcd_clear(); setting_fingerprint = DELETE_FINGERPRINT;}
	  						 break;

	  					 case ADD_FINGERPRINT:
	  						 lcd_clear();
	  						 lcd_put_cur(0, 0);
	  						 lcd_send_string("NEW FINGERPRINT" );
	  						 lcd_put_cur(1, 0);
	  						 int m = 0;
	  						 while(1){

	  						 }
	  						 break;

	  					 case DELETE_FINGERPRINT:
	  						 break;
	  					 }
	  				 }
	  			 }
	  		  break;

	  	  case STATE_OPENDOOR:
	  		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
	  		  HAL_Delay(500);
	  		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
	  		  HAL_Delay(100);
	  		  currentstate = STATE_IDLE;
	  		  interrupt_flag10 = 0;
	  		  break;
	  	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB13 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_1){
		currentstate = STATE_SETTING;
		interrupt_flag1 = 1;
		lcd_clear();
	}
	if(GPIO_Pin == GPIO_PIN_10){
		currentstate = STATE_OPENDOOR;
		interrupt_flag10 = 1;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	uint8_t fingerprint_eeprom[10];
	if(huart -> Instance == huart2.Instance){
		HAL_UART_Receive_IT(&huart2, fingerprint, sizeof(fingerprint));
		if(currentstate == STATE_IDLE){
			interrupt_uart2 = 0;
			for(int page = START_PAGE_FINGERPRINT; page <= MAX_FINGERPRINT; page++){
				EEPROM_Read(page, 0, fingerprint_eeprom, sizeof(fingerprint_eeprom));
				for(int f = 0; f <= sizeof(fingerprint); f++){
					if(fingerprint[f] != fingerprint_eeprom[f]){
						break;
					}
					if(f == sizeof(fingerprint)){
						lcd_clear(); lcd_put_cur(0, 0); lcd_send_string("OPEN DOOR");
						currentstate = STATE_OPENDOOR;
						interrupt_uart2 = 1;
						break;
					}
				}
				if(interrupt_uart2 == 1){
					break;
				}
			}
			if(interrupt_uart2 == 0){
				lcd_clear(); lcd_put_cur(0, 0); lcd_send_string("FAILURE");
				number_of_failure++;
				if(number_of_failure >= 5){
					currentstate = STATE_DISABLE;
					number_of_failure = 0;
				}
			}

		}
		if(setting_fingerprint == ADD_FINGERPRINT){

		}
	}
}
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
