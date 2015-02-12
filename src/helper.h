/*
 * helper.h
 *
 *  Edited on: 2013-9-15
 *      Author: Dangping
 *      Edited by Fanjing on 2013-9-15
 */

#ifndef _HELPER_H_
#define _HELPER_H_
// the Helper class

class Helper {
public:
    int U_Randint(int a, int b); //uniform distribution, the average number is (a+b)/2.
    double P_Rand(double Lamda); //poisson distribution, the average number is Lamda.
    double E_Rand(double Lamda); //exponential distribution, the average number is 1/Lamda.
};

#endif /* HELPER_H_ */
