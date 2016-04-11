/*
 * Module.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "Module.h"

/**
  * @brief  Builds Module object
  * @param  delay - Module task start delay in milliseconds
  * @param  period - Module task period in milliseconds
  * @retval None
  */
Module::Module(const uint32_t delay, const uint32_t period) : CooperativeTask(delay, period) {
}

/**
  * @brief  Processes I2C request frame
  * @param  request: pointer to request frame
  * @param  response: pointer to response frame
  * @retval 1 if response
  *         0 if no response
  */
uint32_t Module::processRequest(I2CFrame *request, I2CFrame *response) {
    (void)request;
    (void)response;
    return 0;
}
