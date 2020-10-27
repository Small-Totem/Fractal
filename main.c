#include <stdint.h>
#include <stdio.h>
#include "svpng.inc"
#pragma warning(disable:4996)

#define SHIFT_X        -2.1     //-2.0  ���ֱ�С=ͼƬ����
#define SHIFT_Y        -1.2    //-1.0  ���ֱ�С=ͼƬ����

#define PIXEL_X        1300 //1536
#define PIXEL_Y        1200   //1024

#define PIXEL_times    6  //��Ⱦ���ʣ�����Խ��Խ���������ǹ�֪��Ҫ���
								//1920x1080 4�� ������˰����(i7-9750h)��ͼƬԼ100mb
								//1080x800 10�� 1��40��  253mb
								//1600*1400 10�� 1��40�� 640mb��׼ȷ���ݣ�
								//1300x1200 12�� 1��55�� 642mb��׼ȷ���ݣ�
#define SCALE          512.0  //512.0 ����Խ������Խ��
#define SCALE_times    1

/*Ŀǰ�Թ����Ҳ��Ǻܳ�ĺ���:
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
	printf("������Ⱦ\n");
	uint8_t* p = data;
	for (int i = 0; i < PIXEL_Y * PIXEL_times; ++i) {
		for (int j = 0; j < PIXEL_X * PIXEL_times; ++j) {
			uint8_t n = mandelbrot(j / (SCALE* SCALE_times* PIXEL_times) +(SHIFT_X),
				                   i / (SCALE* SCALE_times* PIXEL_times) +(SHIFT_Y)) * 255;
			if(n>235){*p++ = n; *p++ = n; *p++ =n;}   //�ڰ�  ����Խ����Խ͸��
			else if(n>210){*p++ = 0; *p++ = 0; *p++ = n;}  //��
			else if (n > 160) { *p++ = 0; *p++ = n; *p++ = 0; }  //��
			else { *p++ = n; *p++ = 0; *p++ = 0; }  // ��
		}
	}
	FILE* file = fopen("test.png", "wb");
	svpng(file, PIXEL_X * PIXEL_times, PIXEL_Y * PIXEL_times, data, 0);
	fclose(file);
	printf("���\n");
}