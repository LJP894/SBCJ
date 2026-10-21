/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : STM32TouchController.cpp
  ******************************************************************************
  */
/* USER CODE END Header */

/* USER CODE BEGIN STM32TouchController */

#include <STM32TouchController.hpp>
#include "stm32f7xx_hal.h"

extern "C"
{
extern I2C_HandleTypeDef hi2c3;
}

void STM32TouchController::init()
{
    uint8_t mode = 0;
    HAL_I2C_Mem_Write(&hi2c3, 0x70, 0xA4, I2C_MEMADD_SIZE_8BIT, &mode, 1, 20);
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    uint8_t buf[4];

    if (HAL_I2C_Mem_Read(&hi2c3, 0x70, 0x02, I2C_MEMADD_SIZE_8BIT, buf, 1, 20) != HAL_OK)
    {
        static uint32_t lastRecover = 0;
        uint32_t now = HAL_GetTick();
        if ((now - lastRecover) > 500u)
        {
            lastRecover = now;
            HAL_I2C_DeInit(&hi2c3);
            HAL_I2C_Init(&hi2c3);
        }
        return false;
    }
    if (buf[0] == 0 || buf[0] >= 6)
    {
        return false;
    }

    if (HAL_I2C_Mem_Read(&hi2c3, 0x70, 0x03, I2C_MEMADD_SIZE_8BIT, buf, 4, 20) != HAL_OK)
    {
        return false;
    }
    int32_t first = (int32_t)(((buf[0] << 8) | buf[1]) & 0x0FFF);
    int32_t second = (int32_t)(((buf[2] << 8) | buf[3]) & 0x0FFF);

    x = second;
    y = first;
    if (x < 0 || x >= 480 || y < 0 || y >= 272)
    {
        x = first;
        y = second;
    }
    if (x < 0 || x >= 480 || y < 0 || y >= 272)
    {
        return false;
    }
    return true;
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
