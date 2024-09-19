#include "KeyPad.h"
#include "KeyPadConfig.h"
#include "stm32f1xx_hal_gpio.h"
#if (_KEYPAD_USE_FREERTOS==1)
#include "cmsis_os.h"
#define _KEYPAD_DELAY(x)      osDelay(x)
#else
#define _KEYPAD_DELAY(x)      HAL_Delay(x)
#endif

KeyPad_t	KeyPad;
char        KEYPAD_Val [4][3] ={
								 {'1','2','3'},
								 {'4','5','6'},
								 {'7','8','9'},
								 {'*','0','#'}
							} ;
//#############################################################################################
void KeyPad_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

//    /* Initialize KEYPAD ROW pins */
//    GPIO_InitStruct.Pin = KEYPAD_ROW1_PIN | KEYPAD_ROW2_PIN | KEYPAD_ROW3_PIN | KEYPAD_ROW4_PIN ;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//    /* Initialize KEYPAD COLUMN pins */
//    GPIO_InitStruct.Pin = KEYPAD_COL1_PIN | KEYPAD_COL2_PIN | KEYPAD_COL3_PIN  ;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    /* Set all ROW pins to High level */
//    HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW1_PIN | KEYPAD_ROW2_PIN | KEYPAD_ROW3_PIN | KEYPAD_ROW4_PIN, GPIO_PIN_SET);
//


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


      /*set all row pins to high level*/
      HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW3_PIN | KEYPAD_ROW2_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW1_PIN | KEYPAD_ROW4_PIN, GPIO_PIN_SET);
}

//#############################################################################################
char KEYPAD_GetChar(void){
	// Check ROW 1 ----------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW1_PIN, 1) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW2_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW3_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW4_PIN, 0) ;
	if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL1_PIN) == 1)return KEYPAD_Val[0][0] ;
		else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL2_PIN) == 1) return KEYPAD_Val[0][1] ;
			else if(HAL_GPIO_ReadPin(GPIOA, KEYPAD_COL3_PIN) == 1) return KEYPAD_Val[0][2] ;

	// Check ROW 2 ----------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW1_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW2_PIN, 1) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW3_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW4_PIN, 0) ;
	if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL1_PIN) == 1)return KEYPAD_Val[1][0];
		else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL2_PIN) == 1) return KEYPAD_Val[1][1] ;
			else if(HAL_GPIO_ReadPin(GPIOA, KEYPAD_COL3_PIN) == 1) return KEYPAD_Val[1][2] ;

	// Check ROW 3 ----------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW1_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW2_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW3_PIN, 1) ;
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW4_PIN, 0) ;
	if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL1_PIN) == 1)return KEYPAD_Val[2][0];
		else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL2_PIN) == 1) return KEYPAD_Val[2][1] ;
			else if(HAL_GPIO_ReadPin(GPIOA, KEYPAD_COL3_PIN) == 1) return KEYPAD_Val[2][2] ;

	// Check ROW 4 ----------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW1_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW2_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW3_PIN, 0) ;
	HAL_GPIO_WritePin(GPIOB, KEYPAD_ROW4_PIN, 1) ;
	if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL1_PIN) == 1)return KEYPAD_Val[3][0];
		else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL2_PIN) == 1) return KEYPAD_Val[3][1] ;
			else if(HAL_GPIO_ReadPin(GPIOA, KEYPAD_COL3_PIN) == 1) return KEYPAD_Val[3][2] ;

//	// Check ROW 5 ----------------------------------------------------------------------------------------
//	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW1_PIN, 0) ;
//	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW2_PIN, 0) ;
//	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW3_PIN, 0) ;
//	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW4_PIN, 0) ;
//	HAL_GPIO_WritePin(GPIOA, KEYPAD_ROW5_PIN, 1) ;
//	if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL1_PIN) == 1)return KEYPAD_Val[0][0];
//		else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL2_PIN) == 1) return KEYPAD_Val[0][1] ;
//			else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL3_PIN) == 1) return KEYPAD_Val[0][2] ;
//				else if(HAL_GPIO_ReadPin(GPIOB, KEYPAD_COL4_PIN) == 1) return KEYPAD_Val[0][3] ;
	return 0;
}

//#############################################################################################
/*char	KeyPad_WaitForKeyGetChar(uint16_t	Key)
{
  switch(Key)
  {
  	// High Byte is Row value,Low Byte is Column Value
  	case 0: return 0 ;
  	case 1: return 'L' ;
  	case 2: return '0' ;
  	case 3: return 'R' ;
  	case 4: return 'E' ;
  	case 5: return '7' ;
  	case 6: return '8' ;
  	case 7: return '9' ;
  	case 8: return 'C' ;
  	case 9: return '4' ;
  	case 10: return '5' ;
  	case 11: return '6' ;
  	case 12: return 'D' ;
  	case 13: return '1' ;
  	case 14: return '2' ;
  	case 15: return '3' ;
  	case 16: return 'U' ;
  	case 17: return 'X' ;
  	case 18: return 'Y' ;
  	case 19: return '#' ;
  	case 20: return '*' ;
  	default: return 0   ;
  }
}
*/
