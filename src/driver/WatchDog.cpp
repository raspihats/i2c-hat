/*
 * WatchDog.cpp
 *
 *  Created on: Apr 6, 2016
 *      Author: fcos
 */

#include "WatchDog.h"
#include "stm32f0xx.h"

namespace i2c_hat {

WatchDog::WatchDog() {
    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    IWDG_Enable();

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(IWDG_Prescaler_8);
    while(IWDG_GetFlagStatus(IWDG_FLAG_PVU) == SET);

    IWDG_SetReload(0x0FFF);
    while(IWDG_GetFlagStatus(IWDG_FLAG_RVU) == SET);

    IWDG_ReloadCounter();
}

void WatchDog::feed() {
    IWDG_ReloadCounter();
}

} /* namespace i2c_hat */
