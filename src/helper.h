/*
 * helper.h
 *
 *  Edited on: 2013-9-15
 *      Author: Dangping
 *      Edited by Fanjing on 2013-9-15
 */

#ifndef _HELPER_H_
#define _HELPER_H_
//生成请求的辅助函数

class Helper
{
public:
	int U_Randint(int a, int b);//均匀分布，均值为(a+b)/2
    double P_Rand(double Lamda);//泊松分布，Lamda为泊松分布的均值
    double E_Rand(double Lamda);//指数分布，均值为1 / Lamda
};



#endif /* HELPER_H_ */
