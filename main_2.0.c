#include <stdint.h>
#include <stdio.h>
#include "svpng.inc"
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <Windows.h>
#pragma warning(disable : 4996)
#define SCALE 512.0 //这个是标准缩放大小，不要改

//图像中心的x,y坐标
#define CENTER_X -1.810175967039
#define CENTER_Y -0.0044548213382266
//分辨率
#define PIXEL_X 1920
#define PIXEL_Y 1080
//渲染倍率
#define PIXEL_times 1

//显示渲染进度2==显示100段 1==显示5段 0==false   仅用于单线程模式
#define SHOW_PROCESS 2
//0==不使用多线程 10==使用10个线程
#define USE_Multithread 10
/*多线程原理：指定10个线程，把画面分为十个横条，每个线程渲染一条
每个横条高度为PIXEL_X/10 最后一条为PIXEL_X/10+PIXEL_X%10*/

uint8_t data[(int)((PIXEL_Y) * (PIXEL_X)*3 * PIXEL_times * PIXEL_times)];

//为了线程的参数传递
struct Param
{
	int id;
	int color_reversal;
	int generate_mode;
	int iteration_times;
	double scale;
	int num;
	int thread_id;
};

void generate(int id, int color_reversal, int generate_mode, int iteration_times,
			  double scale, int num);

double get_absolute_value(double x)
{
	if (x > 0)
		return x;
	else
		return -x;
}

double mandelbrot(double x, double y, int id, int color_reversal, int iteration_times)
{
	double a = x, b = y;
	switch (id)
	{
		// x^2
	case 0:
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a - b * b + x, d = 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case 1:
		//x^3
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a * a - 3 * a * b * b + x, d = 3 * a * a * b - b * b * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;

	case 2:
		// x^4
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a * a * a - 6 * a * a * b * b + b * b * b * b + x, d = 4 * a * a * a * b - 4 * b * b * b * a + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case 3:
		// x^4+x^3+x^2
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a * a * a - 6 * a * a * b * b + b * b * b * b + a * a * a - 3 * a * b * b + a * a - b * b + x,
				   d = 4 * a * a * a * b - 4 * b * b * b * a + 3 * a * a * b - b * b * b + 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;

	case 4:
		//x^2-x^3
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a - b * b - (a * a * a - 3 * a * b * b) + x, d = 2 * a * b - (3 * a * a * b - b * b * b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;

	case 5:
		//The Burning Ship fractal
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a - b * b + x, d = 2 * get_absolute_value(a) * get_absolute_value(b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;

	case 6:
		//无意义的混沌
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a - b * b / a + x, d = 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case 7:
		// 一个混沌
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a - b * b + x, d = 2 * a * b / (a + b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case 8:
		//混沌
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * (a + b) - b * (a - b) + x, d = 2 * a * b / (a + b) * (a - b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case 9:
		//茱莉亚集合
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a * a - b * b - 0.70176, d = 2 * a * b - 0.3842;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case 10:
		//牛顿分形 f(z)=z^3-1 x_n+1=x_n-f(x_n)/f'(x_n)
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a - (2 * a * b * (3 * pow(a, 2) * b - pow(b, 3)) + (pow(a, 2) - pow(b, 2)) * ((-3 * a * pow(b, 2)) + pow(a, 3) - 1)) / (3 * pow((pow(b, 2) + pow(a, 2)), 2)),
				   d = b - ((pow(a, 2) - pow(b, 2)) * (3 * pow(a, 2) * b - pow(b, 3)) - 2 * a * b * ((-3 * a * pow(b, 2)) + pow(a, 3) - 1)) / (3 * pow((pow(b, 2) + pow(a, 2)), 2));
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	case -1: //(一个圆，用来分析渲染模式)
		for (int i = 0; i < iteration_times; ++i)
		{
			double c = a + x, d = b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				if (color_reversal)
				{
					return i / (double)iteration_times;
				}
				else
					return 1 - i / (double)iteration_times;
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

void *multithread_generate(void *arg)
{
	struct Param tmp = *(struct Param *)arg;

	int thread_num = 10;
	uint8_t *p = data;

	int one_part_of_temp_i = (PIXEL_Y * PIXEL_times) / thread_num;
	int temp_i_start = tmp.thread_id * one_part_of_temp_i;

	p += temp_i_start * PIXEL_X * 3 * PIXEL_times;
	int temp_i_end = temp_i_start + one_part_of_temp_i;
	if (tmp.thread_id == thread_num - 1)
	{
		//处理不能被整除的情况(多出来的全部由最后一个线程渲染)
		temp_i_end += PIXEL_Y % thread_num;
	}

	//printf("thread:%d,%d,%d\n",tmp.thread_id,temp_i_start,temp_i_end);
	int temp_i = (int)(PIXEL_Y * PIXEL_times);
	int temp_j = (int)(PIXEL_X * PIXEL_times);
	double temp_scale = 512.0 * (tmp.scale) * (PIXEL_times);

	for (int i = temp_i_start; i < temp_i_end; ++i)
	{
		for (int j = 0; j < temp_j; ++j)
		{

			uint8_t n = mandelbrot((CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
								   (CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
								   tmp.id, tmp.color_reversal, tmp.iteration_times) *
						255;
			//越接近mandelbrot集合内，则n值越高

			switch (tmp.generate_mode)
			{
			case 0:
				*p++ = 4 * n;
				*p++ = 2 * n;
				*p++ = n;
				break;
			case 1:
				*p++ = 4 * n;
				*p++ = n;
				*p++ = 2 * n;
				break;
			case 2:
				*p++ = 2 * n;
				*p++ = 4 * n;
				*p++ = n;
				break;
			case 3:
				*p++ = n;
				*p++ = 4 * n;
				*p++ = 2 * n;
				break;
			case 4:
				*p++ = n;
				*p++ = 2 * n;
				*p++ = 4 * n;
				break;
			case 5:
				*p++ = 2 * n;
				*p++ = n;
				*p++ = 4 * n;
				break;
			case 6:
				*p++ = 16 * n;
				*p++ = 4 * n;
				*p++ = n;
				break;
			case 7:
				*p++ = 2 * n;
				*p++ = 4 * n;
				*p++ = 16 * n;
				break;
			case 8:
				*p++ = 8 * n;
				*p++ = 16 * n;
				*p++ = 2 * n;
				break;
			case 9:
				*p++ = 4 * n;
				*p++ = 16 * n;
				*p++ = n;
				break;
			case 10:
				*p++ = 32 * n;
				*p++ = 2 * n;
				*p++ = 8 * n;
				break;
			case 11:
				*p++ = 8 * n;
				*p++ = 64 * n;
				*p++ = n;
				break;
			case 12:
				*p++ = 32 * n;
				*p++ = 64 * n;
				*p++ = 4 * n;
				break;
			case 13:
				*p++ = 64 * n;
				*p++ = 16 * n;
				*p++ = 32 * n;
				break;
			case 14:
				*p++ = 8 * n;
				*p++ = 16 * n;
				*p++ = 64 * n;
				break;
			case 15:
				*p++ = 16 * n;
				*p++ = 2 * n;
				*p++ = 64 * n;
				break;
			case 16:
				*p++ = 64 * n;
				*p++ = 32 * n;
				*p++ = 8 * n;
				break;
			case 17:
				*p++ = 64 * n;
				*p++ = 4 * n;
				*p++ = 8 * n;
				break;
			case 18:
				*p++ = 32 * n;
				*p++ = 64 * n;
				*p++ = 16 * n;
				break;
			case 19:
				*p++ = 32 * n;
				*p++ = 64 * n;
				*p++ = 128 * n;
				break;
			case 20:
				*p++ = 128 * n;
				*p++ = 4 * n;
				*p++ = 30 * n;
				break;
			case -1:
				if (n > 235)
				{
					*p++ = n;
					*p++ = n;
					*p++ = n;
				} //黑白  数字越大则越透明
				else if (n > 200)
				{
					*p++ = 0;
					*p++ = n;
					*p++ = 0;
				} //绿
				else if (n > 160)
				{
					*p++ = n + 20;
					*p++ = n;
					*p++ = 0;
				} //黄
				else if (n > 120)
				{
					*p++ = 0;
					*p++ = 0;
					*p++ = n + 90.0;
				} //浅蓝
				else if (n > 80)
				{
					*p++ = 0;
					*p++ = 0;
					*p++ = n + 40.0;
				} //深蓝
				else if (n > 40)
				{
					*p++ = n;
					*p++ = 0;
					*p++ = n + 50.0;
				} //紫
				else if (n > 10)
				{
					*p++ = n + 80.0;
					*p++ = 0;
					*p++ = 0;
				} // 红
				else
				{
					*p++ = n;
					*p++ = 0;
					*p++ = 0;
				} //红，但不加鲜艳度
				break;
			default:
				*p++ = n;
				*p++ = n;
				*p++ = n;
				break;
			}
		}
	}
	//printf("%d thread_finish\n",tmp.thread_id);
}

void save_png(int num)
{
	uint8_t *p = data;
	char path[30] = "D:\\fractal\\";
	char *str;
	int dec, sign;
	int ndig = 0;
	char png[] = ".png";

	if (num < 0)
	{
		num = -num;
		char negative[] = "-";
		strcat(path, negative);
	}

	str = fcvt(num, ndig, &dec, &sign); //把int转成char[]
	if (num == 0)
	{
		str = "0";
	}
	strcat(path, str);
	strcat(path, png);

	FILE *file = fopen(path, "wb");

	printf("渲染完成,正在写入 ");
	svpng(file, (int)((PIXEL_X) * (PIXEL_times)), (int)((PIXEL_Y) * (PIXEL_times)), data, 0);
	fclose(file);
	printf("完成\n");
}

void generate(int id, int color_reversal, int generate_mode, int iteration_times,
			  double scale, int num)
{

	printf("正在渲染%d\n", num);

	if (USE_Multithread == 10 && PIXEL_Y >= 10)
	{
		printf("启用10线程渲染\n");
		pthread_t my_thread[10];

		struct Param param1[10];
		//这里暂时没想到更好的办法，先将就下(这里除了thread_id以外都可以只传1次)
		for (int i = 0; i < 10; i++)
		{
			param1[i].id = id;
			param1[i].color_reversal = color_reversal;
			param1[i].generate_mode = generate_mode;
			param1[i].iteration_times = iteration_times;
			param1[i].scale = scale;
			param1[i].num = num;
			param1[i].thread_id = i;
			pthread_create(&my_thread[i], NULL, multithread_generate, &param1[i]);
		}

		for (int i = 0; i < 10; i++)
		{
			pthread_join(my_thread[i], NULL);
		}
		save_png(num);
		return;
	}

	uint8_t *p = data;
	int temp_i = (PIXEL_Y * PIXEL_times);
	int temp_j = (PIXEL_X * PIXEL_times);
	double temp_scale = 512.0 * (scale) * (PIXEL_times);

	for (int i = 0; i < temp_i; ++i)
	{
		for (int j = 0; j < temp_j; ++j)
		{

			uint8_t n = mandelbrot((CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
								   (CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
								   id, color_reversal, iteration_times) *
						255;
			//越接近mandelbrot集合内，则n值越高

			switch (generate_mode)
			{
			case 0:
				*p++ = 4 * n;
				*p++ = 2 * n;
				*p++ = n;
				break;
			case 1:
				*p++ = 4 * n;
				*p++ = n;
				*p++ = 2 * n;
				break;
			case 2:
				*p++ = 2 * n;
				*p++ = 4 * n;
				*p++ = n;
				break;
			case 3:
				*p++ = n;
				*p++ = 4 * n;
				*p++ = 2 * n;
				break;
			case 4:
				*p++ = n;
				*p++ = 2 * n;
				*p++ = 4 * n;
				break;
			case 5:
				*p++ = 2 * n;
				*p++ = n;
				*p++ = 4 * n;
				break;
			case 6:
				*p++ = 16 * n;
				*p++ = 4 * n;
				*p++ = n;
				break;
			case 7:
				*p++ = 2 * n;
				*p++ = 4 * n;
				*p++ = 16 * n;
				break;
			case 8:
				*p++ = 8 * n;
				*p++ = 16 * n;
				*p++ = 2 * n;
				break;
			case 9:
				*p++ = 4 * n;
				*p++ = 16 * n;
				*p++ = n;
				break;
			case 10:
				*p++ = 32 * n;
				*p++ = 2 * n;
				*p++ = 8 * n;
				break;
			case 11:
				*p++ = 8 * n;
				*p++ = 64 * n;
				*p++ = n;
				break;
			case 12:
				*p++ = 32 * n;
				*p++ = 64 * n;
				*p++ = 4 * n;
				break;
			case 13:
				*p++ = 64 * n;
				*p++ = 16 * n;
				*p++ = 32 * n;
				break;
			case 14:
				*p++ = 8 * n;
				*p++ = 16 * n;
				*p++ = 64 * n;
				break;
			case 15:
				*p++ = 16 * n;
				*p++ = 2 * n;
				*p++ = 64 * n;
				break;
			case 16:
				*p++ = 64 * n;
				*p++ = 32 * n;
				*p++ = 8 * n;
				break;
			case 17:
				*p++ = 64 * n;
				*p++ = 4 * n;
				*p++ = 8 * n;
				break;
			case 18:
				*p++ = 32 * n;
				*p++ = 64 * n;
				*p++ = 16 * n;
				break;
			case 19:
				*p++ = 32 * n;
				*p++ = 64 * n;
				*p++ = 128 * n;
				break;
			case 20:
				*p++ = 128 * n;
				*p++ = 4 * n;
				*p++ = 30 * n;
				break;
			case -1:
				if (n > 235)
				{
					*p++ = n;
					*p++ = n;
					*p++ = n;
				} //黑白  数字越大则越透明
				else if (n > 200)
				{
					*p++ = 0;
					*p++ = n;
					*p++ = 0;
				} //绿
				else if (n > 160)
				{
					*p++ = n + 20;
					*p++ = n;
					*p++ = 0;
				} //黄
				else if (n > 120)
				{
					*p++ = 0;
					*p++ = 0;
					*p++ = n + 90.0;
				} //浅蓝
				else if (n > 80)
				{
					*p++ = 0;
					*p++ = 0;
					*p++ = n + 40.0;
				} //深蓝
				else if (n > 40)
				{
					*p++ = n;
					*p++ = 0;
					*p++ = n + 50.0;
				} //紫
				else if (n > 10)
				{
					*p++ = n + 80.0;
					*p++ = 0;
					*p++ = 0;
				} // 红
				else
				{
					*p++ = n;
					*p++ = 0;
					*p++ = 0;
				} //红，但不加鲜艳度
				break;
			default:
				*p++ = n;
				*p++ = n;
				*p++ = n;
				break;
			}
		}

		if (SHOW_PROCESS == 1)
		{
			if (i == (int)(temp_i / 5))
			{
				printf("20%%完成\n");
			}
			else if (i == (int)(temp_i / 5 * 2))
			{
				printf("40%%完成\n");
			}
			else if (i == (int)(temp_i / 5 * 3))
			{
				printf("60%%完成\n");
			}
			else if (i == (int)(temp_i / 5 * 4))
			{
				printf("80%%完成\n");
			}
		}
		else if (SHOW_PROCESS == 2)
		{
			for (int n = 1; n < 100; n++)
			{
				if (i != (int)(temp_i / 100) * n)
					continue;
				else
				{
					printf("%d%%完成\n", n);
					break;
				}
			}
		}
	}

	save_png(num);
}

int main()
{
	//				           0==false
	//void generate(int id, int color_reversal, int generate_mode, int iteration_times,
	//double scale, int num);

	//生成的图片在"D:\\fractal\\"中，可以在save_png(int num)函数中更改
	generate(5, 1, 0, 128, 12, 0);

	exit(0);
}