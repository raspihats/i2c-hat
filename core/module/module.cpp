/*
 * module.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "module.h"

namespace i2c_hat {
namespace module {

/**
  * @brief  Builds Module object
  * @param  delay - Module task start delay in milliseconds
  * @param  period - Module task period in milliseconds
  * @retval None
  */
Module::Module(const uint32_t delay, const uint32_t period) :
        Task(delay, period) {
}

} /* namespace module */
} /* namespace i2c_hat */
