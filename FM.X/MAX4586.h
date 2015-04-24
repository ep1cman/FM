/* 
 * File:   MAX4586.h
 * Author: Phil
 *
 * Created on 24 April 2015, 21:30
 */

#ifndef MAX4586_H
#define MAX4586_H

#define MAX4586_L_ADDR 0x6A
#define MAX4586_R_ADDR 0x6E

#define OUTPUT_1 0x01
#define OUTPUT_2 0x02
#define OUTPUT_3 0x04
#define OUTPUT_4 0x08

int selectOutput(int outputNumber);


#endif	/* MAX4586_H */

