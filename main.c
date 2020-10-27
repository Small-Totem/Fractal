#include <stdint.h>
#include <stdio.h>
#include "svpng.inc"
#pragma warning(disable:4996)

#define SHIFT_X        -2.1     //-2.0  数字变小=图片右移
#define SHIFT_Y        -1.2    //-1.0  数字变小=图片下移

#define PIXEL_X        1300 //1536
#define PIXEL_Y        1200   //1024

#define PIXEL_times    6  //渲染倍率，数字越大越清晰，但是鬼知道要多久
								//1920x1080 4倍 大概用了半分钟(i7-9750h)，图片约100mb
								//1080x800 10倍 1分40秒  253mb
								//1600*1400 10倍 1分40秒 640mb（准确数据）
								//1300x1200 12倍 1分55秒 642mb（准确数据）
#define SCALE          512.0  //512.0 数字越大则倍率越大
#define SCALE_times    1

/*目前试过而且不是很丑的函数:
double c =  a*a - b*b/a + x


*/


double mandelbrot(double x, double y) {
	double a = x, b = y;
	for (int i = 0; i < 128; ++i) {
		double c =  a*a - b*b + x, d = 2 * a * b + y;
		a = c, b = d;
		if (a * a + b * b > 4) return 1 - i / 128.0;
	}
	return 0;
}

uint8_t data[PIXEL_Y* PIXEL_times * PIXEL_X * PIXEL_times * 3];

int main(void) {
	printf("正在渲染\n");
	uint8_t* p = data;
	for (int i = 0; i < PIXEL_Y * PIXEL_times; ++i) {
		for (int j = 0; j < PIXEL_X * PIXEL_times; ++j) {
			uint8_t n = mandelbrot(j / (SCALE* SCALE_times* PIXEL_times) +(SHIFT_X),
				                   i / (SCALE* SCALE_times* PIXEL_times) +(SHIFT_Y)) * 255;
			if(n>235){*p++ = n; *p++ = n; *p++ =n;}   //黑白  数字越大则越透明
			else if(n>210){*p++ = 0; *p++ = 0; *p++ = n;}  //蓝
			else if (n > 160) { *p++ = 0; *p++ = n; *p++ = 0; }  //绿
			else { *p++ = n; *p++ = 0; *p++ = 0; }  // 红
		}
	}
	FILE* file = fopen("test.png", "wb");
	svpng(file, PIXEL_X * PIXEL_times, PIXEL_Y * PIXEL_times, data, 0);
	fclose(file);
	printf("完成\n");
}