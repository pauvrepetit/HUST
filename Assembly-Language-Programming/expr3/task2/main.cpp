#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 10		//商品数量
#define numOfBag 1000

typedef struct goodStruct {
	char good_name[10];
	char good_len;
	char good_sale;
	short good_in_price;
	short good_out_price;
	short good_all_count;
	short good_sale_count;
	short good_recommand;
	short good_sort;
} goodStruct;

char bname[11] = "huao";
char bpass[11] = "huao";
char shopname[] = "huao";
char auth;
goodStruct good[N];
int goodLen = sizeof(goodStruct);
char welcomeMsg[] = "welcome to ";
char welcomeShopMag[] = " shop";
char loginErrorMsg[] = "Login Error\n";
char outenter[] = "\n";
char info[] = "input your name and password\n";
char in_name[13];
char in_pass[9];
char checkname[] = "input commodity you want to check:";
char goodname[12];
char notFound[] = "good not found\n";


char menu1[] = "1. Query product information\n";
char menu2[] = "2. Edit product information\n";
char menu3[] = "3. Calculate recommendation\n";
char menu4[] = "4. Calculate recommendation ranking\n";
char menu5[] = "5. Output all product information\n";
char menu6[] = "6. exit the program\n";

char errorInputMsg[] = "Input Error. Input again.\n";

char bufF2T10[12];
short DAT = 10;
char SIGN;

extern "C" void _cdecl WelcomeFunc(char *welcomeMsg, char * shopname, char *welcomeShopMsg, char *outenter, char * info, char *in_name, char *in_pass, char *bname, char *bpass, char *auth, char *loginErrorMsg);

extern "C" void _cdecl ShowMenu(char auth, char *menu1, char *menu2, char *menu3, char *menu4, char *menu5, char *menu6);;

extern "C" void _cdecl calIndex(void *goodAddr, int goodLen, short goodNum);;

extern "C" void _cdecl change_good(char *goodAddr, char *SIGN, short *DAT);

extern "C" void _cdecl outone(char *goodAddr);

extern "C" void _cdecl outall(short goodSum, void *goodAddr);

extern "C" void _cdecl sort(short goodSum, void *goodAddr);

extern "C" void _cdecl ExitProgram(void);

void init(void);
char *findGood(char *goodName);

int main(void)
{
	init();
	char temp;
	int funcNum;
	char *goodAddr;
	while (1)
	{
		WelcomeFunc(welcomeMsg, shopname, welcomeShopMag, outenter, info, in_name, in_pass, bname, bpass, &auth, loginErrorMsg);
		ShowMenu(auth, menu1, menu2, menu3, menu4, menu5, menu6);

		printf("Please choose a function:");
		scanf("%d", &funcNum);
		switch (funcNum)
		{
		case 1:
			while (1)
			{
				printf(checkname);
				scanf("%s", goodname);
				goodAddr = findGood(goodname);
				if (goodAddr == NULL)
				{
					printf(notFound);
				}
				else
				{
					break;
				}
			}
			outone(goodAddr);
			break;
		case 2:
			while (1)
			{
				printf(checkname);
				scanf("%s", goodname);
				goodAddr = findGood(goodname);
				if (goodAddr == NULL)
					printf(notFound);
				else
					break;
			}
			change_good(goodAddr, &SIGN, &DAT);
			break;
		case 3:
			calIndex(good, goodLen, N);
			break;
		case 4:
			sort(N, good);
			break;
		case 5:
			outall(N, good);
			break;
		case 6:
			ExitProgram();
			break;
		default:
			printf("input error, please input again\n");
			break;
		}
	}
	printf("\n");
	scanf("%c", &temp);
	getchar();
	strlen(bname);
	return 0;
}

char *findGood(char *goodName)
{
	void *goodAddr = good;
	for (int i = 0; i < N; i++)
	{
		if (strcmp(goodName, good[i].good_name) == 0)
		{
			return (char *)goodAddr + i * goodLen;
		}
	}
	return NULL;
}

void init(void) {
	bname[10] = 4;
	bpass[10] = 4;

	strcpy(good[0].good_name, "pen");
	good[0].good_len = 3;
	good[0].good_sale = 10;
	good[0].good_in_price = 35;
	good[0].good_out_price = 56;
	good[0].good_all_count = 70;
	good[0].good_sale_count = 25;
	good[0].good_sort = 1;

	strcpy(good[1].good_name, "book");
	good[1].good_len = 4;
	good[1].good_sale = 9;
	good[1].good_in_price = 12;
	good[1].good_out_price = 30;
	good[1].good_all_count = 25;
	good[1].good_sale_count = 5;
	good[1].good_sort = 1;

	for (int i = 2; i < N - 1; i++)
	{
		strcpy(good[i].good_name, "egg");
		good[i].good_len = 3;
		good[i].good_sale = 7;
		good[i].good_in_price = 35;
		good[i].good_out_price = 56;
		good[i].good_all_count = 70;
		good[i].good_sale_count = 25;
		good[i].good_sort = 1;
	}

	strcpy(good[N - 1].good_name, "bag");
	good[N - 1].good_len = 3;
	good[N - 1].good_sale = 10;
	good[N - 1].good_in_price = 20;
	good[N - 1].good_out_price = 50;
	good[N - 1].good_all_count = numOfBag;
	good[N - 1].good_sale_count = 0;
	good[N - 1].good_sort = 1;

}