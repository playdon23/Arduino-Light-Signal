#ifndef DEFS_H
#define DEFS_H

#define clrbit(data,bit_num) (data) &= ~(1 << bit_num)
#define setbit(data,bit_num) (data) |= (1 << bit_num)
#define testbit(data,bit_num) ((data) & (1 << bit_num))

#define TRESHOLD_HAZZARD_BUTTON 200

#endif