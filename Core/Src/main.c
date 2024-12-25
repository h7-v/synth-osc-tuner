/* USER CODE BEGIN Header */
/**
 *
 * STM32 based tuner
 * Works best with sawtooth waveform.
 *
 *
 *
 * MAKE SURE TO USE THE CORRECT CLOCK SOURCE:
 * DEV BOARD: HSE EXTERNAL CLOCK
 * MCA BOARD: HSI INTERNAL CLOCK
 *
 *
 *
 *
 *
 *
 *
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
#include "../User/Example/test.h"
#include "../User/OLED/OLED_0in96_rgb.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Precomputed frequency values scaled by 10 to avoid floating point
int note_frequencies[] = {
    163, 173, 183, 194, 206, 218, 231, 245, 259, 275, 291, 308,    // Octave 0
    327, 346, 367, 388, 412, 436, 462, 490, 519, 550, 582, 617,    // Octave 1
    654, 693, 734, 777, 824, 873, 925, 980, 1038, 1100, 1165, 1234, // Octave 2
    1308, 1386, 1468, 1555, 1648, 1746, 1850, 1960, 2076, 2200, 2331, 2469, // Octave 3
    2616, 2772, 2937, 3111, 3296, 3492, 3700, 3920, 4153, 4400, 4662, 4939, // Octave 4
    5233, 5544, 5873, 6223, 6592, 6985, 7400, 7840, 8306, 8800, 9323, 9877, // Octave 5
    10465, 11087, 11746, 12445, 13185, 13969, 14799, 15679, 16612, 17600, 18647, 19755, // Octave 6
    20930, 22175, 23493, 24890, 26370, 27938, 29599, 31359, 33224, 35200, 37293, 39511, // Octave 7
    41860, 44349, 46986, 49780, 52740, 55876, 59199, 62719, 66449, 70400, 74586, 79021, // Octave 8
    83720, 88698, 93973, 99561, 105480, 111753, 118398, 125438, 132898, 140800, 149172, 158042, // Octave 9
    167440, 177397, 187946, 199121  // Octave 10, truncated at D#
};

const char* note_names[] = {
    "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "
};

// Function to get the nearest musical note and indicate if frequency is
// lower or higher
const char* get_nearest_note_x10(int frequency_x10, char *result) {
    int closest_note = -1;
    int min_diff = 1000000;
    int size = sizeof(note_frequencies) / sizeof(note_frequencies[0]);

    // Find the closest frequency
    for (int i = 0; i < size; i++) {
        int diff = abs(frequency_x10 - note_frequencies[i]);
        if (diff < min_diff) {
            min_diff = diff;
            closest_note = i;
        }
    }

    // Calculate a dynamic tolerance (e.g., 0.5% of the frequency)
    int tolerance = frequency_x10 * 1.2 / 1000;  // 0.12% tolerance

    // Determine if we are higher, lower, or close enough
    if (min_diff <= tolerance) {
        // Exact match within tolerance
        sprintf(result, " %s ", note_names[closest_note % 12]);
    } else if (frequency_x10 > note_frequencies[closest_note]) {
        // Frequency is higher than the closest note
        sprintf(result, " %s<", note_names[closest_note % 12]);
    } else {
        // Frequency is lower than the closest note
        sprintf(result, ">%s ", note_names[closest_note % 12]);
    }

    return result;
}


// Variables to store current frequency for both oscillators
float frequency_ch1 = 0;
float frequency_ch2 = 0;

// Variables to store total frequency and count for oscillator 1
float total_frequency_ch1 = 0;
int frequency_count_ch1 = 0;
const int AVERAGE_CYCLES1 = 10;  // Number of cycles to average over

// This function is called whenever a new frequency is measured
void add_to_freq1_averager(float new_frequency) {
    if (new_frequency < 70.0f) {
    	frequency_ch1 = new_frequency;

    	total_frequency_ch1 = 0;
    	frequency_count_ch1 = 0;
    	return;
    }

	// Accumulate the frequency
    total_frequency_ch1 += new_frequency;
    frequency_count_ch1++;

    // Check if we've reached the number of cycles to average
    if (frequency_count_ch1 >= AVERAGE_CYCLES1) {
        // Calculate the average frequency
        float average_frequency = total_frequency_ch1 / frequency_count_ch1;

        // Set current frequency
        frequency_ch1 = average_frequency;

        // Reset for the next batch of measurements
        total_frequency_ch1 = 0;
        frequency_count_ch1 = 0;
    }
}


// Variables to store total frequency and count for oscillator 2
float total_frequency_ch2 = 0;
int frequency_count_ch2 = 0;
const int AVERAGE_CYCLES2 = 10;  // Number of cycles to average over

// This function is called whenever a new frequency is measured
void add_to_freq2_averager(float new_frequency) {
	if (new_frequency < 70.0f) {
	    frequency_ch2 = new_frequency;

	    total_frequency_ch2 = 0;
	    frequency_count_ch2 = 0;
	    return;
	   }

	// Accumulate the frequency
    total_frequency_ch2 += new_frequency;
    frequency_count_ch2++;

    // Check if we've reached the number of cycles to average
    if (frequency_count_ch2 >= AVERAGE_CYCLES2) {
        // Calculate the average frequency
        float average_frequency = total_frequency_ch2 / frequency_count_ch2;

        // Set current frequency
        frequency_ch2 = average_frequency;

        // Reset for the next batch of measurements
        total_frequency_ch2 = 0;
        frequency_count_ch2 = 0;
    }
}


// Channel 1 (TIM2) variables
uint32_t IC_Val1_Ch1 = 0;
uint32_t IC_Val2_Ch1 = 0;
uint32_t Difference_Ch1 = 0;
int Is_First_Captured_Ch1 = 0;

// Channel 2 (TIM5) variables
uint32_t IC_Val1_Ch2 = 0;
uint32_t IC_Val2_Ch2 = 0;
uint32_t Difference_Ch2 = 0;
int Is_First_Captured_Ch2 = 0;

// Common clock and prescaler definitions
#define TIMCLOCK   100000000.0f
// Uncomment if using prescaler. Also check code in functions below
//#define PRESCALER  1

// Function to handle TIM2 (Channel 1)
void HAL_TIM_IC_CaptureCallback_TIM2(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // Channel 1 active
    {
        if (Is_First_Captured_Ch1 == 0) // First rising edge
        {
            IC_Val1_Ch1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            Is_First_Captured_Ch1 = 1; // First capture complete
        }
        else // Second rising edge
        {
            IC_Val2_Ch1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            if (IC_Val2_Ch1 >= IC_Val1_Ch1)
            {
                Difference_Ch1 = IC_Val2_Ch1 - IC_Val1_Ch1;
            }
            else
            {
                Difference_Ch1 = (0xffffffff - IC_Val1_Ch1) + IC_Val2_Ch1;
            }

            if (Difference_Ch1 > 0)
            {
            	// Old prescaler code
//                float refClock = TIMCLOCK / (PRESCALER);
//                frequency_ch1 = refClock / Difference_Ch1;
//            	frequency_ch1 = TIMCLOCK / Difference_Ch1; // Not using prescaler
            	add_to_freq1_averager(TIMCLOCK / Difference_Ch1);

                if (frequency_ch1 > 9999.9f)
                {
                	frequency_ch1 = 0;
                }
            }
            else
            {
                frequency_ch1 = 0;
            }

            __HAL_TIM_SET_COUNTER(htim, 0); // Reset counter
            Is_First_Captured_Ch1 = 0; // Ready for next capture
        }
    }
}

// Function to handle TIM5 (Channel 2)
void HAL_TIM_IC_CaptureCallback_TIM5(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) // Channel 2 active (TIM5)
    {
        if (Is_First_Captured_Ch2 == 0) // First rising edge
        {
            IC_Val1_Ch2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            Is_First_Captured_Ch2 = 1; // First capture complete
        }
        else // Second rising edge
        {
            IC_Val2_Ch2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

            if (IC_Val2_Ch2 >= IC_Val1_Ch2)
            {
                Difference_Ch2 = IC_Val2_Ch2 - IC_Val1_Ch2;
            }
            else
            {
                Difference_Ch2 = (0xffffffff - IC_Val1_Ch2) + IC_Val2_Ch2;
            }

            if (Difference_Ch2 > 0)
            {
            	// Old prescaler code
//                float refClock = TIMCLOCK / (PRESCALER);
//                frequency_ch2 = refClock / Difference_Ch2;
//                frequency_ch2 = TIMCLOCK / Difference_Ch2; // Not using prescaler
            	add_to_freq2_averager(TIMCLOCK / Difference_Ch2);

                if (frequency_ch2 > 9999.9f)
                {
                    frequency_ch2 = 0;
                }
            }
            else
            {
                frequency_ch2 = 0;
            }

            __HAL_TIM_SET_COUNTER(htim, 0); // Reset counter
            Is_First_Captured_Ch2 = 0; // Ready for next capture
        }
    }
}

// Main callback handler
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        HAL_TIM_IC_CaptureCallback_TIM2(htim); // Handle TIM2 (Channel 1)
    }
    else if (htim->Instance == TIM5)
    {
        HAL_TIM_IC_CaptureCallback_TIM5(htim); // Handle TIM5 (Channel 2)
    }
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  if(System_Init() != 0) {
            return -1;
        }

  OLED_0in96_rgb_Init();

  // This was 500ms but I was getting random pixels for 0.5s. Seems to work like this still
  Driver_Delay_ms(50);

  OLED_0in96_rgb_Clear();
  Driver_Delay_ms(500);

  //1.Create a new image size
  UBYTE *BlackImage;
  UWORD Imagesize = OLED_0in96_rgb_WIDTH * OLED_0in96_rgb_HEIGHT * 2;
  if((BlackImage = (UBYTE *)malloc(Imagesize/2)) == NULL) {
  		printf("Failed to apply for black memory...\r\n");
  		return -1;
  }

  Paint_NewImage(BlackImage, OLED_0in96_rgb_WIDTH, OLED_0in96_rgb_HEIGHT, 270,
		  	  	 BLACK); // 90 is orientation, 270 to flip upside down
  Paint_SetScale(65);
  printf("Drawing\r\n");
  //1.Select Image
  Paint_SelectImage(BlackImage);
  Paint_Clear(BLACK);
  Driver_Delay_ms(50);

  // IMAGES NOTES:
  // Photoshop down to 128x64 with greyscale black background no transparency.
  // Merge layers. Go image -> Adjustments -> Threshold. Adjust to taste
  // Save As... BMP file type
  // Open in Image2Lcd. Use settings C array, Vertical Scan, the top 16-bit
  // TrueColor option.
  // Uncheck Include head data, check Scan bottom to Top.
  // Click save. Create new entries in ImageData.h and ImageData.c with the
  // resulting array.
  // Use in function below.
  // FOR 270 DEGREES uncheck Scan bottom to Top and check Scan Right to Left
  // instead.
  // Logos not being used are commented out in ImageData.c and ImageData.h

  // Display Macbeth logo
  // Photoshop Threshold set to 106
  OLED_0in96_rgb_Display(gImage_macbeth_logo_270deg);
  Driver_Delay_ms(1250);
  Paint_Clear(BLACK);
  Driver_Delay_ms(50);

  // Display MCA logo
  // Photoshop Threshold also set to 106
  OLED_0in96_rgb_Display(gImage_mca_logo_270deg);
  Driver_Delay_ms(1250);
  Paint_Clear(BLACK);
  Driver_Delay_ms(50);

  // Draw O1 and O2 onto the screen out of the loop as they do not change
  Paint_DrawString_EN(10, 0, "O1", &Font16, WHITE, BLACK);
  Paint_DrawString_EN(10, 32, "O2", &Font16, WHITE, BLACK);

  char tunertext_ch1[4];
  char tunertext_ch2[4];
  float freq1_display;
  float freq2_display;

  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  OLED_0in96_rgb_test();

	  // O1 and O2 drawn outside of the loop

	  // WORKING CODE START
	  freq1_display = frequency_ch1;
	  freq2_display = frequency_ch2;

	  // Check if freq1_display is zero and display "--" if true, otherwise
	  // call the function
	  if (freq1_display == 0) {
	      Paint_DrawString_EN(52, 0, " -- ", &Font16, WHITE, BLACK);
	  } else {
	      Paint_DrawString_EN(52, 0, get_nearest_note_x10((int)(freq1_display * 10),
	    		  	  	  	  tunertext_ch1), &Font16, WHITE, BLACK);
	  }

	  // Display the frequency value
	  Paint_DrawNumStaticDecimalPoint(52, 16, freq1_display, &Font16, WHITE,
			  	  	  	  	  	  	  BLACK);

	  // Same logic for freq2_display
	  if (freq2_display == 0) {
	      Paint_DrawString_EN(52, 32, " -- ", &Font16, WHITE, BLACK);
	  } else {
	      Paint_DrawString_EN(52, 32, get_nearest_note_x10((int)(freq2_display * 10),
	    		  	  	  	  tunertext_ch2), &Font16, WHITE, BLACK);
	  }

	  // Display the second frequency value
	  Paint_DrawNumStaticDecimalPoint(52, 48, freq2_display, &Font16, WHITE,
			  	  	  	  	  	  	  BLACK);

	  OLED_0in96_rgb_Display(BlackImage);

	  Driver_Delay_ms(100); // Time between display updates
	  HAL_Delay(50);  // Time between display updates
	  // WORKING CODE END

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 3;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 3;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OLED_CS_Pin|OLED_DC_Pin|OLED_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OLED_CS_Pin OLED_DC_Pin OLED_RST_Pin */
  GPIO_InitStruct.Pin = OLED_CS_Pin|OLED_DC_Pin|OLED_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
