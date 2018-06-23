#include <iostream>
using namespace std;

// macro
#define PRINT_IN_CONSOLE	0
#define FILE_HEAD_LEN		12

// data
unsigned char file_head_data[FILE_HEAD_LEN];
#pragma pack(push)		//保存对齐状态 
#pragma pack(1)			//设定为1字节对齐
struct GD_Data_struct 
{
	unsigned char weapon_no[5];
	unsigned char unknow_1[100];
	unsigned char weapon_name[100];
	unsigned char weapon_type[3];
	unsigned int weapon_damage;
	unsigned short weapon_speed;
	unsigned int weapon_range;
	unsigned char unknow_2[6];
	unsigned short weapon_bullet;
	unsigned int weapon_diameter;
	unsigned short weapon_interval;
	unsigned char unknow_3[13];
	unsigned short weapon_splash[37];
	unsigned char patch[1659];
}GD_Data;
#pragma pack(pop)		//恢复对齐状态 

// function
void hex2asc(unsigned char *out_buf_ptr, unsigned char *in_buf_ptr, unsigned int in_data_len)
{
	unsigned int i = 0;
	unsigned char temp = 0;

	for(i = 0; i < in_data_len; i++)
	{
		temp = (*in_buf_ptr)>>4;
		if(temp >= 0 && temp <= 9)
			*out_buf_ptr++ = temp + '0';
		else if(temp > 9)
			*out_buf_ptr++ = temp - 10 + 'A';

		temp = (*in_buf_ptr)&0x0F;
		if(temp >= 0 && temp <= 9)
			*out_buf_ptr++ = temp + '0';
		else if(temp > 9)
			*out_buf_ptr++ = temp - 10 + 'A';

		*out_buf_ptr++ = ' ';

		in_buf_ptr++;
	}
}

void printf_gd_weapon_data(GD_Data_struct GD_Data)
{
	unsigned char str[2000];

	memset(str, 0, sizeof(str));
	hex2asc(str, GD_Data.unknow_1, sizeof(GD_Data.unknow_1));
	cout<<"未知1："<<str<<endl;

	memset(str, 0, sizeof(str));
	sprintf((char*)str, "%S", GD_Data.weapon_name);
	cout<<"名称："<<str<<endl;

	// 01 普通 02 必杀 03修理 | 01 格斗 02 射击 03 倒地 04 浮游 | 00 特殊 01 格斗 02 实弹 03 光束
#if 1
	cout<<"类型：";

	if(GD_Data.weapon_type[0] == 1)
		cout<<"01普通 ";
	else if(GD_Data.weapon_type[0] == 2)
		cout<<"02必杀 ";
	else if(GD_Data.weapon_type[0] == 3)
		cout<<"03修理 ";
	else
		cout<<GD_Data.weapon_type[0];

	if(GD_Data.weapon_type[1] == 1)
		cout<<"01格斗 ";
	else if(GD_Data.weapon_type[1] == 2)
		cout<<"02射击 ";
	else if(GD_Data.weapon_type[1] == 3)
		cout<<"03倒地 ";
	else if(GD_Data.weapon_type[1] == 4)
		cout<<"04浮游 ";
	else
		cout<<GD_Data.weapon_type[1];

	if(GD_Data.weapon_type[2] == 0)
		cout<<"00特殊 ";
	else if(GD_Data.weapon_type[2] == 1)
		cout<<"01格斗 ";
	else if(GD_Data.weapon_type[2] == 2)
		cout<<"02实弹 ";
	else if(GD_Data.weapon_type[2] == 3)
		cout<<"03光束 ";
	else
		cout<<GD_Data.weapon_type[2];

	cout<<endl;
#else
	memset(str, 0, sizeof(str));
	hex2asc(str, GD_Data.weapon_type, sizeof(GD_Data.weapon_type));
	cout<<"类型："<<str<<endl;
#endif

	cout<<"威力："<<GD_Data.weapon_damage<<endl;

	cout<<"弹速："<<GD_Data.weapon_speed<<endl;

	cout<<"射程："<<GD_Data.weapon_range<<endl;

	memset(str, 0, sizeof(str));
	hex2asc(str, GD_Data.unknow_2, sizeof(GD_Data.unknow_2));
	cout<<"未知2："<<str<<endl;

	cout<<"弹量："<<GD_Data.weapon_bullet<<endl;

	cout<<"直径："<<GD_Data.weapon_diameter<<endl;

	cout<<"间隔："<<GD_Data.weapon_interval<<endl;

	memset(str, 0, sizeof(str));
	hex2asc(str, GD_Data.unknow_3, sizeof(GD_Data.unknow_3));
	cout<<"未知3："<<str<<endl;

	cout<<"溅射：";
	for(int i = 0; i <sizeof(GD_Data.weapon_splash)/sizeof(GD_Data.weapon_splash[0]) ; i++)
		cout<<GD_Data.weapon_splash[i]<<" ";
	cout<<endl;

	//memset(str, 0, sizeof(str));
	//sprintf((char*)str, "%S", GD_Data.patch);
	//cout<<"weapon_name:"<<str<<endl;

	cout<<endl;
}

static bool is_need_read_file = true;
unsigned char printf_gd_data(FILE *file_ptr)
{
	unsigned char unknow_1[100];
	bool is_first_read = true;
	memset(unknow_1, 0, sizeof(unknow_1));

	while (1)
	{
		if(is_need_read_file) {
			if(0 == fread(&GD_Data, sizeof(GD_Data), 1, file_ptr))
				return 0;
		} else {
			is_need_read_file = true;
		}

		if(is_first_read) {
			memcpy(unknow_1, GD_Data.unknow_1, sizeof(GD_Data.unknow_1));
			is_first_read = false;
		}

		// 判断前八个字节是否一致，一致即为同一机体
		if(0 != memcmp(unknow_1, GD_Data.unknow_1, 8)) {
			is_need_read_file = false;
			break;
		} else {
			memcpy(unknow_1, GD_Data.unknow_1, sizeof(GD_Data.unknow_1));
			printf_gd_weapon_data(GD_Data);
		}
	}

	cout<<endl;

	return 1;
}

unsigned char printf_head_data(FILE *file_ptr)
{
	unsigned char temp[FILE_HEAD_LEN];

	if( 0 == fread(temp, sizeof(file_head_data), 1, file_ptr)) {
		return 0;
	} else {
		memset(file_head_data, 0, sizeof(file_head_data));
		hex2asc(file_head_data, temp, sizeof(file_head_data));
		cout<<"包头："<<file_head_data<<endl;
		cout<<endl;
		return 1;
	}
}

int main()
{
	FILE *file_ptr;

	setlocale(LC_CTYPE, "chs");

	file_ptr = fopen("0012_gai.gdt", "rb");

	if(file_ptr == NULL) {
		cout<<"无法打开文件"<<endl;
		system("pause");
		return 0;
	}

	if(0 == printf_head_data(file_ptr)) {
		fclose(file_ptr);
		return 0;
	} else {
#if PRINT_IN_CONSOLE
		system("pause");
		system("cls");
#endif
	}

	while(1) {
		if(0 == printf_gd_data(file_ptr))
			break;
#if PRINT_IN_CONSOLE
		system("pause");
		system("cls");
#endif
	}

	fclose(file_ptr);
	return 0;
}