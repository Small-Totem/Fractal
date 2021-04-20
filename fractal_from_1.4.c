#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "svpng.inc"
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <Windows.h>

#pragma warning(disable : 4996)

#define generate_info_output(a, b) \
	printf(b);                     \
	printf("\n")

static int static_id = 0;
static int static_color_reversal = 0;
static int static_generate_mode = 0;
static int static_iteration_times = 0;
static int static_PIXEL_X = 0;
static int static_PIXEL_Y = 0;
static int static_use_thread = 0;
static int static_auto_iteration_max = 0;
static int static_flag_should_use_symmetrical_generate = 0;
static int static_flag_monitor_generate_info = 0;
static double static_scale = 0;
static double static_CENTER_X = 0;
static double static_CENTER_Y = 0;
static char *static_file_path;

static double static_generate_progress_thread[10];

struct param_for_thread
{
	int thread_id;
	uint8_t *p;
};
struct param_for_auto_iteration
{
	double a, b, n;
};

char *get_string_from_text_num_text(char *text1, int num, char *text2)
{
	//��char* int char*ƴ������
	//���鷳��ʵ��
	//ֻ֧��3λ��
	char temp_num[3];
	sprintf(temp_num, "%d", num);

	char *temp = (char *)malloc(40 * sizeof(char));
	temp[0] = '\0';
	strcat(temp, text1);
	strcat(temp, temp_num);
	strcat(temp, text2);
	return temp;
}

double get_absolute_value(double x)
{
	return (x >= 0 ? x : -x);
}

struct param_for_auto_iteration mandelbrot_continue(double x, double y, double a, double b,
													int id, int color_reversal, int iteration_start_num, int iteration_end_num)
{
	struct param_for_auto_iteration param;
	param.n = 0;
	switch (id)
	{
		// x^2
	case 0:
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a - b * b + x, d = 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 1:
		//x^3
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a * a - 3 * a * b * b + x, d = 3 * a * a * b - b * b * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;

	case 2:
		// x^4
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a * a * a - 6 * a * a * b * b + b * b * b * b + x, d = 4 * a * a * a * b - 4 * b * b * b * a + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 3:
		// x^4+x^3+x^2
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a * a * a - 6 * a * a * b * b + b * b * b * b + a * a * a - 3 * a * b * b + a * a - b * b + x,
				   d = 4 * a * a * a * b - 4 * b * b * b * a + 3 * a * a * b - b * b * b + 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;

	case 4:
		//x^2-x^3
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a - b * b - (a * a * a - 3 * a * b * b) + x, d = 2 * a * b - (3 * a * a * b - b * b * b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;

	case 5:
		//The Burning Ship fractal
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a - b * b + x, d = 2 * get_absolute_value(a) * get_absolute_value(b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;

	case 6:
		//������Ļ���
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a - b * b / a + x, d = 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 7:
		// һ������
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a - b * b + x, d = 2 * a * b / (a + b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 8:
		//����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * (a + b) - b * (a - b) + x, d = 2 * a * b / (a + b) * (a - b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 9:
		//�����Ǽ���
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a * a - b * b - 0.70176, d = 2 * a * b - 0.3842;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 10:
		//ţ�ٷ��� f(z)=z^3-1 x_n+1=x_n-f(x_n)/f'(x_n)
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a - (2 * a * b * (3 * pow(a, 2) * b - pow(b, 3)) + (pow(a, 2) - pow(b, 2)) * ((-3 * a * pow(b, 2)) + pow(a, 3) - 1)) / (3 * pow((pow(b, 2) + pow(a, 2)), 2)),
				   d = b - ((pow(a, 2) - pow(b, 2)) * (3 * pow(a, 2) * b - pow(b, 3)) - 2 * a * b * ((-3 * a * pow(b, 2)) + pow(a, 3) - 1)) / (3 * pow((pow(b, 2) + pow(a, 2)), 2));
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 11:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = a * a - b * get_absolute_value(b) + x, d = 2 * a * get_absolute_value(b) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 12:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = a * a - b * get_absolute_value(b) + x, d = 2 * a * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 13:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = a * a + x, d = a * get_absolute_value(b) - b * b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 14:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)   z=a+b*i //-a-b*i
			double c = a * a - b * b + x, d = 2 * a * b + y;
			a = c, b = d;
			c = a - 0.2 * get_absolute_value(a) + 0.2 * get_absolute_value(b), d = b - 0.2 * get_absolute_value(b) - 0.2 * get_absolute_value(a);
			a = c, b = d;

			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 15:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = a * a - b * b + x, d = 2 * a * b + y;
			a = c, b = d;
			c = a + 0.0009 * (a / (a * a + b * b)), d = b - 0.0009 * (b / (a * a + b * b));
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 16:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = 0.0009 * (a / (a * a + b * b)) + x, d = -0.0009 * (b / (a * a + b * b)) + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 17:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = +x, d = +y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 18:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = +x, d = +y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	case 19:
		//����������youtube��Ť�����²�����
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			//a==real(z) b==imag(z)
			double c = +x, d = +y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;

	case -1: //(һ��Բ������������Ⱦģʽ)
		for (int i = iteration_start_num; i <= iteration_end_num; i++)
		{
			double c = a + x, d = b + y;
			a = c, b = d;
			if (a * a + b * b > 4)
			{
				param.n = i / (double)iteration_end_num;
				break;
			}
		}
		break;
	default:
		break;
	}

	if (!color_reversal && param.n != 0)
	{
		param.n = 1 - param.n;
	}
	param.a = a;
	param.b = b;
	return param;
}

struct param_for_auto_iteration mandelbrot(double x, double y, int id, int color_reversal, int iteration_times)
{
	return mandelbrot_continue(x, y, x, y, id, color_reversal, 1, iteration_times);
}

void write_data(uint8_t *p, int n, int generate_mode)
{
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
		} //�ڰ�  ����Խ����Խ͸��
		else if (n > 200)
		{
			*p++ = 0;
			*p++ = n;
			*p++ = 0;
		} //��
		else if (n > 160)
		{
			*p++ = n + 20;
			*p++ = n;
			*p++ = 0;
		} //��
		else if (n > 120)
		{
			*p++ = 0;
			*p++ = 0;
			*p++ = n + 90.0;
		} //ǳ��
		else if (n > 80)
		{
			*p++ = 0;
			*p++ = 0;
			*p++ = n + 40.0;
		} //����
		else if (n > 40)
		{
			*p++ = n;
			*p++ = 0;
			*p++ = n + 50.0;
		} //��
		else if (n > 10)
		{
			*p++ = n + 80.0;
			*p++ = 0;
			*p++ = 0;
		} // ��
		else
		{
			*p++ = n;
			*p++ = 0;
			*p++ = 0;
		} //�죬���������޶�
		break;
	default:
		*p++ = n;
		*p++ = n;
		*p++ = n;
		break;
	}
}

//y==0ʱ��ֻ����Ⱦһ�루ֻ�ԶԳ�ͼ����Ч��
void symmetrical_generate(uint8_t *p)
{
	//�������ݶԳƸ���
	uint8_t *temp1 = p;
	uint8_t *temp2 = p;

	temp2 += 3 * static_PIXEL_X * static_PIXEL_Y;
	temp2 -= 3 * static_PIXEL_X;
	for (int i1 = 0; i1 < ((static_PIXEL_Y + 1) / 2); i1++)
	{
		for (int i2 = 0; i2 < 3 * static_PIXEL_X; i2++)
		{
			*temp2 = *temp1;
			temp1++;
			temp2++;
		}
		temp2 -= 2 * 3 * static_PIXEL_X;
	}
}

void *multithread_generate(void *arg)
{
	struct param_for_thread tmp = *(struct param_for_thread *)arg;
	uint8_t *p = tmp.p;

	int one_part_of_temp_i;
	int temp_i_start;
	int temp_i_end;
	int temp_i;
	int temp_j;
	double temp_scale;

	if (static_flag_should_use_symmetrical_generate == 1)
	{
		one_part_of_temp_i = (static_PIXEL_Y / 2) / static_use_thread;
	}
	else
	{
		one_part_of_temp_i = (static_PIXEL_Y) / static_use_thread;
	}

	temp_i_start = tmp.thread_id * one_part_of_temp_i;

	p += temp_i_start * static_PIXEL_X * 3;
	temp_i_end = temp_i_start + one_part_of_temp_i;
	if (tmp.thread_id == static_use_thread - 1)
	{
		//�����ܱ����������(�������ȫ�������һ���߳���Ⱦ)
		if (static_flag_should_use_symmetrical_generate == 1)
		{
			temp_i_end += (static_PIXEL_Y / 2) % static_use_thread;
		}
		else
		{
			temp_i_end += static_PIXEL_Y % static_use_thread;
		}
	}

	temp_i = static_PIXEL_Y;
	temp_j = static_PIXEL_X;
	temp_scale = 512.0 * (static_scale);

	int divider = (temp_i_end - temp_i_start) / 100; //������Ⱦ���ȵ�Ƶ����
													 //dividerԽ�������ԽƵ��
	if (divider < 1)
		divider = 1;

	for (int i = temp_i_start; i < temp_i_end; ++i)
	{
		for (int j = 0; j < temp_j; ++j)
		{
			struct param_for_auto_iteration param_continue = mandelbrot((static_CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
																		(static_CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
																		static_id, static_color_reversal, static_iteration_times);

			uint8_t n = param_continue.n * 255;

			//����Ӧ����
			if (static_auto_iteration_max > 0 && n == 0 && static_iteration_times < static_auto_iteration_max)
			{
				int it_start = static_iteration_times;
				int it_end = it_start * 5;
				while (it_end < static_auto_iteration_max)
				{
					//�Ժ�ɫ�ĵط���ߵ�������
					if (n == 0)
					{
						param_continue = mandelbrot_continue((static_CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
															 (static_CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
															 param_continue.a, param_continue.b, static_id, static_color_reversal,
															 it_start, it_end);
						n = param_continue.n * 255;
						it_start = it_end;
						it_end *= 5;
					}
					else
					{
						break;
					}
				}
				if (n == 0)
				{
					//�Ի��Ǻ�ɫ�ĵط��������һ�ε���
					n = mandelbrot_continue((static_CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
											(static_CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
											param_continue.a, param_continue.b, static_id, static_color_reversal,
											it_start, static_auto_iteration_max)
							.n *
						255;
				}
			}

			write_data(p, n, static_generate_mode);
			p += 3;
		}
		if (i % divider == 0 && static_flag_monitor_generate_info)
			static_generate_progress_thread[tmp.thread_id] = (double)(i - temp_i_start) / (temp_i_end - temp_i_start);
	}
	if (static_flag_monitor_generate_info)
		static_generate_progress_thread[tmp.thread_id] = 1;

	generate_info_output(4, get_string_from_text_num_text("�߳�", tmp.thread_id, "����"));

	pthread_exit(0);
}

void generate()
{
	uint8_t *data = (uint8_t *)malloc((static_PIXEL_Y) * (static_PIXEL_X)*3);

	//���߳�
	if (static_use_thread > 1 && static_PIXEL_Y >= 20)
	{
		pthread_t my_thread[10];
		struct param_for_thread param1[10];

		for (int i = 0; i < static_use_thread; i++)
		{
			param1[i].thread_id = i;
			param1[i].p = data;
			pthread_create(&my_thread[i], NULL, multithread_generate, &param1[i]);
		}

		for (int i = 0; i < static_use_thread; i++)
			pthread_join(my_thread[i], NULL);

		if (static_flag_should_use_symmetrical_generate == 1)
			symmetrical_generate(data);

		generate_info_output(0, "��Ⱦ���,����д��");

		FILE *file = fopen(static_file_path, "wb");
		svpng(file, (static_PIXEL_X), (static_PIXEL_Y), data, 0);
		fclose(file);
		free(data);
		generate_info_output(5, "���");
		return;
	}

	//��ͨ
	else
	{
		uint8_t *p = data;
		double temp_i = (static_PIXEL_Y);
		double temp_j = (static_PIXEL_X);
		double temp_scale = 512.0 * (static_scale);

		for (int i = 0; i < temp_i; ++i)
		{
			for (int j = 0; j < temp_j; ++j)
			{
				struct param_for_auto_iteration param_continue = mandelbrot((static_CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
																			(static_CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
																			static_id, static_color_reversal, static_iteration_times);

				uint8_t n = param_continue.n * 255;
				//Խ�ӽ�mandelbrot�����ڣ���nֵԽ��   ���ڼ�����,n=0   n=255ʱ Ϊ��

				if (static_auto_iteration_max > 0 && n == 0 && static_iteration_times < static_auto_iteration_max)
				{
					int it_start = static_iteration_times;
					int it_end = it_start * 5;
					while (it_end < static_auto_iteration_max)
					{
						//�Ժ�ɫ�ĵط���ߵ�������
						if (n == 0)
						{
							param_continue = mandelbrot_continue((static_CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
																 (static_CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
																 param_continue.a, param_continue.b, static_id, static_color_reversal,
																 it_start, it_end);
							n = param_continue.n * 255;
							it_start = it_end;
							it_end *= 5;
						}
						else
						{
							break;
						}
					}
					if (n == 0)
					{
						//�Ի��Ǻ�ɫ�ĵط��������һ�ε���
						n = mandelbrot_continue((static_CENTER_X)-temp_j / (2 * (temp_scale)) + j / (temp_scale),
												(static_CENTER_Y)-temp_i / (2 * temp_scale) + i / temp_scale,
												param_continue.a, param_continue.b, static_id, static_color_reversal,
												it_start, static_auto_iteration_max)
								.n *
							255;
					}
				}

				write_data(p, n, static_generate_mode);
				p += 3;
			}
			if (i % 10 == 0 && static_flag_monitor_generate_info && static_flag_should_use_symmetrical_generate)
			{
				static_generate_progress_thread[0] = (double)i / (double)temp_i * 2;
			}
			else if (i % 10 == 0 && static_flag_monitor_generate_info)
			{
				static_generate_progress_thread[0] = (double)i / (double)temp_i;
			}
			//�Գ���Ⱦ
			//((temp_i+1)/2) : �����������1�����temp_i���������ͻ���һ��û��Ⱦ
			//��һ֮����������Ⱦ�ˣ�ż���в�Ӱ��
			if (static_flag_should_use_symmetrical_generate == 1 && i == (((int)temp_i + 1) / 2))
			{
				symmetrical_generate(data);
				break;
			}
		}
		if (static_flag_monitor_generate_info)
			static_generate_progress_thread[0] = 1;

		generate_info_output(0, "��Ⱦ���,����д��");

		FILE *file = fopen(static_file_path, "wb");
		svpng(file, (static_PIXEL_X), (static_PIXEL_Y), data, 0);
		fclose(file);
		free(data);
		generate_info_output(5, "���");
		return;
	}
}

void call_generate(char *path, int screen_height, int screen_width,
				   double center_x, double center_y,
				   double scale_times, int fractal_id,
				   int color_reversal, int generate_mode,
				   int iteration_times, int use_thread,
				   int auto_iteration_max, int monitor_generate_info)
{

	static_file_path = path;
	static_id = fractal_id;
	static_color_reversal = color_reversal;
	static_generate_mode = generate_mode;
	static_iteration_times = iteration_times;
	static_PIXEL_X = screen_width;
	static_PIXEL_Y = screen_height;
	static_use_thread = use_thread;
	static_auto_iteration_max = auto_iteration_max;
	static_scale = scale_times;
	static_CENTER_X = center_x;
	static_CENTER_Y = center_y;
	static_flag_monitor_generate_info = monitor_generate_info;

	for (int i = 0; i < 10; i++)
	{
		static_generate_progress_thread[i] = 0;
	}
	if (static_CENTER_Y == 0 && ((static_id <= 4 && static_id >= -1) || static_id == 15))
	{
		static_flag_should_use_symmetrical_generate = 1;
		generate_info_output(0, "���öԳ���Ⱦ����");
	}
	else
	{
		static_flag_should_use_symmetrical_generate = 0;
	}

	//��� ����x�߳���Ⱦ
	generate_info_output(0, get_string_from_text_num_text("����", static_use_thread, "�߳���Ⱦ"));

	generate();
}

/*
call_generate(char* path,int screen_height, int screen_width,
double center_x, double center_y,
double scale_times, int fractal_id,
int color_reversal, int generate_mode,
int iteration_times, int use_thread,
int auto_iteration_max, int monitor_generate_info)
*/

int main()
{
	double scale = 256;
	double pixel_times = 1;

	call_generate("D:\\zjh_lab\\f\\n\\fractal.png",
				  (int)(1080 * pixel_times), (int)(1920 * pixel_times),
				  0, 0, (double)scale * pixel_times, 0,
				  1, 0, 128, 10, 2000, 1);
	system("start D:\\zjh_lab\\f\\n\\fractal.png");

	exit(0);
}