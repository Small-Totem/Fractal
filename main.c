#include <stdint.h>
#include <stdio.h>
#include "svpng.inc"
#pragma warning(disable:4996)
#define STRING(x) #x   //处理字符串，STRING(a_string)  展开后为：  "a_string"
#define TO_STRING(x) STRING(x)//为了宏的嵌套调用，否则STRING(FILE_NAME)会输出"FILE_NAME"

//------------------------------------------------------
//Small_Totem 2020.10.28

//图像中心的x,y坐标
#define CENTER_X       -0.8//-0.8//-1.75
#define CENTER_Y	0//-0.9//-0.185

//渲染图像的分辨率
#define PIXEL_X        1200   //1536
#define PIXEL_Y        750 //1024

//渲染图像的清晰度倍率（其实就是增加分辨率）//数字越大越清晰，但是鬼知道要多久
#define PIXEL_times    1 

//图像缩放倍率，不影响渲染速度
#define SCALE          512.0  //这个是标准缩放大小，不要改
#define SCALE_times    1

//生成的文件名
#define FILE_NAME      my_test.png  

/*目前试过而且不是很丑的函数:
		double c =  a*a - b*b/a + x
		double c =  a*(a+b) - b*(a-b) + x, d = 2*a * b + y;

*/

double mandelbrot(double x, double y) {
	double a = x, b = y;
	for (int i = 0; i < 128; ++i) {
		double c =  a*a - b*b + x, d = 2*a * b + y;
		a = c, b = d;
		if (a*a+b*b > 4) return 1 - i / 128.0;
	}
	return 0;
}

uint8_t data[(PIXEL_Y) * (PIXEL_times) * (PIXEL_X) * (PIXEL_times) * 3];

int main(void) {
	printf("正在渲染\n");
	uint8_t* p = data;
	double temp_i = (PIXEL_Y) * (PIXEL_times);
	double temp_j = (PIXEL_X) * (PIXEL_times);
	double temp_scale=(SCALE)* (SCALE_times)*(PIXEL_times);

	for (int i = 0; i < temp_i; ++i) {
		for (int j = 0; j < temp_j; ++j) {
			uint8_t n = mandelbrot((CENTER_X)-temp_j / (2 * temp_scale) + j / temp_scale,
								   (CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale) * 255;
			//越接近mandelbrot集合内，则n值越高

			if (n > 230) { *p++ = n; *p++ = n; *p++ = n; }   //黑白  数字越大则越透明
			else if (n > 200) { *p++ = 0; *p++ = n; *p++ = 0; }  //绿
			else if (n > 160) { *p++ = n+20; *p++ = n; *p++ = 0; }  //黄
			else if (n > 120) { *p++ = 0; *p++ = 0; *p++ = n + 90.0; }  //浅蓝
			else if (n > 80) { *p++ = 0; *p++ = 0; *p++ = n+40.0; }  //深蓝
			else if (n > 40) { *p++ = n; *p++ = 0; *p++ = n+50.0; }  //紫
			else if (n > 10) { *p++ = n + 80.0; *p++ = 0; *p++ = 0; }  // 红
			else { *p++ = n ; *p++ = 0; *p++ = 0; }  //红，但不加鲜艳度
		}
	}
	FILE* file = fopen(TO_STRING(FILE_NAME), "wb");
	svpng(file, (PIXEL_X) * (PIXEL_times), (PIXEL_Y) * (PIXEL_times), data, 0);
	fclose(file);
	printf("完成\n");
	//exit();
	system(TO_STRING(start FILE_NAME));//打开图片
}
