/*
 * k_type.h
 *
 *  Created on: Mar 1, 2018
 *      Author: fcos
 */

#ifndef K_TYPE_H_
#define K_TYPE_H_

#include "thermocouple.h"

class K_Type: public Thermocouple {
public:
    K_Type();
    bool ConvertTempToVoltage(float temp, float &voltage);
    bool ConvertVoltageToTemp(float voltage, float &temp);
private:
    static Range coeff[2];
    static Range coeff_inverse[3];
};

#endif /* K_TYPE_H_ */
