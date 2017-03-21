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
	int coin = 0;
	int flag = 0;
	unsigned long int time;
	unsigned int securityCode,securityCode0;
	unsigned long int  seed;
	unsigned int serial_number;
	printf("请输入使用机器的序列号［0：9999］:\n");
	flag = scanf("%d",&serial_number);
	if(1 !=flag) {
		getchar();
	}
	if(serial_number > 9999){
		printf("输入数据有误！设为默认值：1234\n");
		serial_number = 1234;
	}
	while(1){
		printf("请输入投币数[1:15]：\n");
		flag = scanf("%d",&coin);
		if(1 !=flag) {
			getchar();
			coin = MAX_COIN +1;
		}
		if(coin > MAX_COIN){
			printf("输入数据有误！\n");
			continue;
		}
		time = getDateAndTime();
		countOfDay ++;
		if(countOfDay > MAX_COUNTOFDAY){
			printf("今天的币卖完了\n");
			continue;
		}
		set_serial_seed(serial_number, countOfDay);
		securityCode0 = myRandom(MIN_RANDOM,MAX_RANDOM);
		set_my_seed(time, countOfDay, coin);
		//printf("当前时间：%ld\n",time);
		//printf("种子：%ld\n",seed);
		securityCode = myRandom(MIN_RANDOM,MAX_RANDOM);
		printf("验证码：%d%d\n\n",securityCode0,securityCode);
	}
}
