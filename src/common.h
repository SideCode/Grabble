#ifndef COMMON_H
#define COMMON_H

/*
 Converts an 8bit binary number to its decimal equivalent.
 '##' is preproccesor code to join two things together.
 '\' extends the #define to multiple lines.
 The number is first morphed to hexadecimal so that bitwise operations are more intuitive.
*/
#define B8(x)(((0x##x)&0x00000001?1:0)  \
			       +((0x##x)&0x00000010?2:0)  \
			       +((0x##x)&0x00000100?4:0)  \
			       +((0x##x)&0x00001000?8:0)\
			       +((0x##x)&0x00010000?16:0) \
			       +((0x##x)&0x00100000?32:0) \
			       +((0x##x)&0x01000000?64:0) \
			       +((0x##x)&0x10000000?128:0))

#define ABS(x) (x < 0 ? -x : x)

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 180

#define TRUE 1
#define FALSE 0

#endif
