/*
 * thermocouple.h
 *
 *  Created on: Mar 1, 2018
 *      Author: fcos
 */

#ifndef THERMOCOUPLE_H_
#define THERMOCOUPLE_H_

class Thermocouple {
public:
Thermocouple();
    virtual bool ConvertTempToVoltage(float temp, float &voltage)=0;
    virtual bool ConvertVoltageToTemp(float voltage, float &temp)=0;
    virtual ~Thermocouple();
};

struct Range {
    float min;
    float max;
    int coeff_count;
    float coeff[14];
};

#endif /* THERMOCOUPLE_H_ */
