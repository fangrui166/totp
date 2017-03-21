#include <stdio.h> 
#include <stdlib.h> 
#include <stddef.h>
#include <time.h>

#define MAX_COIN    15
#define MAX_RANDOM  9999
#define MIN_RANDOM  1000
#define MAX_COUNTOFDAY 25
static unsigned short countOfDay = 0;
static unsigned int validity = 0;
typedef struct {
	int securityCode[MAX_COUNTOFDAY*MAX_COIN];
	int count;
}invalidSecurityCode;
invalidSecurityCode invalidCode = {0};
#if 0
unsigned long int getDateAndTime(void)
{
	char year,month,day,hour,min,sec;
	time_t timer;
	struct tm *p;
	timer = time(NULL);
	p=localtime(&timer);
	year = (1900+p->tm_year)%100;
	month = (1+p->tm_mon)%100;
	day = p->tm_mday%100;
	hour = p->tm_hour%100;
	min = p->tm_min%100;
	sec = p->tm_sec%100;
	printf("[Y:M:D-H:M:S] -> %d:%d:%d-%d:%d:%d\n",
		year,month,day,hour,min,sec);
	return year*10000000000+month*100000000+day*1000000+hour*10000+min*100+sec;
}
#else
unsigned long int getDateAndTime(void)
{
	time_t timer;
	time(&timer);
	return timer;
}
#endif

unsigned int myRandom(int start, int end)
{
	int max = end;
#ifdef _WIN32
	return start+rand()%(max+1);
#else
	return start+random()%(max+1);
#endif
}
void set_my_seed(unsigned long int time, unsigned short count, int coin)
{
	/*     time | count | coin 
	 *  0x FFFF |  FFF  |   F 
	 */
	unsigned long int  seed;
	time = time/(60*60*12);
	if(validity != time){
		validity = time;
		countOfDay = 0;
		invalidCode.count = 0;
	}
	seed = ((time & 0xFFFF) << 16) | ((count & 0x0FFF) << 4) | (coin & 0xF);
	srand(seed);
}
void set_serial_seed(int serial_number,int argv)
{
	/*     serial |  argv 
	 *  0x  FFFF  |  FFFF
	 */
	unsigned long int seed;
	seed = ((serial_number &0xFFFF) << 16) | (argv & 0xFFFF);
	srand(seed);
}
main()
{
	int i = 0,j=0,k=0;
	char verf_pass = 0;
	int flag = 0;
	unsigned long int time;
	int securityCode;
	unsigned long int  seed;
	unsigned int serial_number;
	printf("请设置本机序列号［0：9999］:\n");
	flag = scanf("%d",&serial_number);
	if(1 !=flag) {
		getchar();
	}
	if(serial_number > 9999){
		printf("输入数据有误！设为默认值：1234\n");
		serial_number = 1234;
	}
	while(1){
		printf("请输入验证码：\n");
		scanf("%d",&securityCode);
		time = getDateAndTime();
		printf("当前时间：%ld\n",time);

		for(i=1;i<MAX_COIN;i++){
			for(j=0;j<MAX_COUNTOFDAY;j++){
				unsigned int temp1,temp2;
				set_serial_seed(serial_number,j);
				temp1 = myRandom(MIN_RANDOM,MAX_RANDOM);
				set_my_seed(time, j, i);
				temp2 = myRandom(MIN_RANDOM,MAX_RANDOM);
				if( (temp1*10000+temp2) == securityCode){
					for(k=0;k<invalidCode.count;k++){
						if(securityCode == invalidCode.securityCode[k]){
							printf("验证码已失效，或已经使用。\n");
							break;
						}
					}
					if(k >= invalidCode.count){
						printf("投币个数：%d\n",i);
						invalidCode.securityCode[invalidCode.count] = securityCode;
						invalidCode.count ++;
						verf_pass = 1;
					}
					else{
						verf_pass = 0;
					}
					
					break;
				}
			}
			if(j < MAX_COUNTOFDAY) break;
		}
		if(i >= MAX_COIN){
			printf("验证码无效!\n");
		}
	}
}
