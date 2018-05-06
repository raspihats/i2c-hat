/*
 * k_type.cpp
 *
 *  Created on: Mar 1, 2018
 *      Author: fcos
 *
 * Nist Polinomials: https://srdata.nist.gov/its90/download/allcoeff.tab
 */

#include "k_type.h"

/*
************************************
* This section contains coefficients for type K thermocouples for
* the two subranges of temperature listed below.  The coefficients
* are in units of °C and mV and are listed in the order of constant
* term up to the highest order.  The equation below 0 °C is of the form
* E = sum(i=0 to n) c_i t^i.
*
* The equation above 0 °C is of the form
* E = sum(i=0 to n) c_i t^i + a0 exp(a1 (t - a2)^2).
*
*     Temperature Range (°C)
*        -270.000 to 0.000
*         0.000 to 1372.000
************************************
name: reference function on ITS-90
type: K
temperature units: °C
emf units: mV
range: -270.000, 0.000, 10
  0.000000000000E+00
  0.394501280250E-01
  0.236223735980E-04
 -0.328589067840E-06
 -0.499048287770E-08
 -0.675090591730E-10
 -0.574103274280E-12
 -0.310888728940E-14
 -0.104516093650E-16
 -0.198892668780E-19
 -0.163226974860E-22
range: 0.000, 1372.000, 9
 -0.176004136860E-01
  0.389212049750E-01
  0.185587700320E-04
 -0.994575928740E-07
  0.318409457190E-09
 -0.560728448890E-12
  0.560750590590E-15
 -0.320207200030E-18
  0.971511471520E-22
 -0.121047212750E-25
exponential:
 a0 =  0.118597600000E+00
 a1 = -0.118343200000E-03
 a2 =  0.126968600000E+03
 */
Range K_Type::coeff[2] = {
    {
        -270.000,               // min °C
        0.000,                  // max °C
        11,                     // coeff count
        {
            0.000000000000E+00, // c0
            0.394501280250E-01, // c1
            0.236223735980E-04, // c2
           -0.328589067840E-06, // c3
           -0.499048287770E-08, // c4
           -0.675090591730E-10, // c5
           -0.574103274280E-12, // c6
           -0.310888728940E-14, // c7
           -0.104516093650E-16, // c8
           -0.198892668780E-19, // c9
           -0.163226974860E-22, // c10
        }
    },
    {
        0.000,                  // min °C
        1372.000,               // max °C
        10,                     // coeff count
        {
           -0.176004136860E-01, // c0
            0.389212049750E-01, // c1
            0.185587700320E-04, // c2
           -0.994575928740E-07, // c3
            0.318409457190E-09, // c4
           -0.560728448890E-12, // c5
            0.560750590590E-15, // c6
           -0.320207200030E-18, // c7
            0.971511471520E-22, // c8
           -0.121047212750E-25, // c9
        }
    }
};

/*
************************************
* This section contains coefficients of approximate inverse
* functions for type K thermocouples for the subranges of
* temperature and voltage listed below. The range of errors of
* the approximate inverse function for each subrange is also given.
* The coefficients are in units of °C and mV and are listed in
* the order of constant term up to the highest order.
* The equation is of the form t_90 = d_0 + d_1*E + d_2*E^2 + ...
*     + d_n*E^n,
* where E is in mV and t_90 is in °C.
*
*    Temperature        Voltage            Error
*      range              range            range
*      (°C)               (mV)             (° C)
*    -200. to 0.      -5.891 to 0.000    -0.02 to 0.04
*     0. to 500.      0.000 to 20.644    -0.05 to 0.04
*     500. to 1372.   20.644 to 54.886   -0.05 to 0.06
********************************************************
Inverse coefficients for type K:

Temperature  -200.             0.           500.
  Range:        0.           500.          1372.

  Voltage   -5.891          0.000         20.644
  Range:     0.000         20.644         54.886

         0.0000000E+00  0.000000E+00 -1.318058E+02
         2.5173462E+01  2.508355E+01  4.830222E+01
        -1.1662878E+00  7.860106E-02 -1.646031E+00
        -1.0833638E+00 -2.503131E-01  5.464731E-02
        -8.9773540E-01  8.315270E-02 -9.650715E-04
        -3.7342377E-01 -1.228034E-02  8.802193E-06
        -8.6632643E-02  9.804036E-04 -3.110810E-08
        -1.0450598E-02 -4.413030E-05  0.000000E+00
        -5.1920577E-04  1.057734E-06  0.000000E+00
         0.0000000E+00 -1.052755E-08  0.000000E+00

  Error      -0.02          -0.05          -0.05
  Range:      0.04           0.04           0.06
 */
Range K_Type::coeff_inverse[3] = {
    {
        -5.891,                 // min mV
        0.000,                  // max mV
        10,                     // coeff count
        {
            0.0000000E+00,      // d0
            2.5173462E+01,      // d1
           -1.1662878E+00,      // d2
           -1.0833638E+00,      // d3
           -8.9773540E-01,      // d4
           -3.7342377E-01,      // d5
           -8.6632643E-02,      // d6
           -1.0450598E-02,      // d7
           -5.1920577E-04,      // d8
            0.0000000E+00,      // d9
        }
    },
    {
        0.000,                  // min mV
        20.644,                 // max mV
        10,                     // coeff count
        {
            0.000000E+00,       // d0
            2.508355E+01,       // d1
            7.860106E-02,       // d2
           -2.503131E-01,       // d3
            8.315270E-02,       // d4
           -1.228034E-02,       // d5
            9.804036E-04,       // d6
           -4.413030E-05,       // d7
            1.057734E-06,       // d8
           -1.052755E-08        // d9
        }
    },
    {
        20.644,                 // min mV
        54.886,                 // max mV
        10,                     // coeff count
        {
           -1.318058E+02,       // d0
            4.830222E+01,       // d1
           -1.646031E+00,       // d2
            5.464731E-02,       // d3
           -9.650715E-04,       // d4
            8.802193E-06,       // d5
           -3.110810E-08,       // d6
            0.000000E+00,       // d7
            0.000000E+00,       // d8
            0.000000E+00,       // d9
        }
    }
};

K_Type::K_Type() {
    // TODO Auto-generated constructor stub

}

bool K_Type::ConvertTempToVoltage(float temp, float &voltage) {
    int i, j;
    int range_index;
    double t_pow_i;
    double mili_volts;

    i = 0;
    while (not ((coeff[i].min <= temp) and (temp <= coeff[i].max)) ) {
        i++;
        if(i == 2) {
            return false;
        }
    }
    range_index = i;

    mili_volts = 0;
    for(i = 0; i < coeff[range_index].coeff_count; i++) {
        t_pow_i = 1;
        for(j = 0; j < i; j++) {
            t_pow_i *= temp;
        }
        mili_volts += coeff[range_index].coeff[i] * t_pow_i;
    }

    voltage = mili_volts / 1000;

    return true;
}

bool K_Type::ConvertVoltageToTemp(float voltage, float &temp) {
    int i, j;
    int range_index;
    double e_pow_i;
    double mili_volts;

    mili_volts = voltage * 1e+3;

    i = 0;
    while (not ((coeff_inverse[i].min <= mili_volts) and (mili_volts <= coeff_inverse[i].max)) ) {
        i++;
        if(i == 3) {
            return false;
        }
    }
    range_index = i;

    temp = 0;
    for(i = 0; i < coeff_inverse[range_index].coeff_count; i++) {
        e_pow_i = 1;
        for(j = 0; j < i; j++) {
            e_pow_i *= mili_volts;
        }
        temp += coeff_inverse[range_index].coeff[i] * e_pow_i;
    }

    return true;
}

