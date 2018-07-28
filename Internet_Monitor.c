#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>

#define CHECK_TIME 3 //Delay between internet connection checks in minutes


int main()
{

	FILE *ping_status, *ping_fail_log, *ping_success_log;
	char word[100], text[500];
	char ping_success[100] = "1packetstransmitted,1received,0%packetloss";
	time_t     now, start;
	struct tm  ts, tn;
	char buf[80];
	char input = 'x';
	char check[10] = "X";
	int fail_count=0;
	int check_count = 0;
	int Init=1;

	//Setup curses.h
	initscr();
	cbreak();
	noecho();
	scrollok(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	while(input != 'q')
	{

		input = getch();
		erase();

		printw(
		"Internet Monitor UP\n"
		"We will check the internet is operational every 3 minutes\n\n\n"
		"Last check reported a %s\n"
		"The internet has been checked %i times\n"
		"A failure has been logged %i times\n"
		"Please check the ping_fail_log.txt file for dates and times\n"
		, check
		, check_count
		, fail_count
		);


		//Perform ping to google and store in ping.txt
		system("ping -c1 google.com > ping_status.txt");

		//Read ping.txt and copy each word into text string
		ping_status = fopen("ping_status.txt", "r");
 		word[0] = '\0'; 
		text[0] = '\0';
		do{
			fscanf(ping_status, "%s", word);
			strcat(text, word);
		}
		while(!feof(ping_status));
		fclose(ping_status);

		//if text contains ping success string then log success, else log fail
		if(strstr(text,ping_success )!=NULL)
		{
//			printf("ping success\n");
			ping_success_log = fopen("ping_success_log.txt", "a");
			// Get current time
			time(&now);
			// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
			ts = *localtime(&now);
			strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
//			printf("%s\n", buf);

			fprintf(ping_success_log, "%s\n", buf);
			fclose(ping_success_log);
			strcpy(check, "success");
	  	}
		else
		{
//			printf("ping failed\n");
			ping_fail_log = fopen("ping_failed_log.txt", "a");
			// Get current time
			time(&now);

			// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
			ts = *localtime(&now);
			strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
//			printf("%s\n", buf);
			fprintf(ping_fail_log, "%s\n", buf);
			fclose(ping_fail_log);
			fail_count++;
			strcpy(check,"fail");
		}
		check_count++;

		//Create a delay between internet connection checks
		if(!Init)
		{
			//Get Start Time
			time(&start);
			// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
			ts = *localtime(&start);
			do{
				input = getch();
				time(&now);
				tn = *localtime(&now);
				if(tn.tm_min < ts.tm_min)
				{
					tn.tm_min+= 60;
				}
			}
			while(tn.tm_min - ts.tm_min < CHECK_TIME && input != 'q');
		//	while((tn.tm_min - ts.tm_min < CHECK_TIME) && (tn.tm_min - (60 - ts.tm_min) < CHECK_TIME || ((60 - CHECK_TIME) > ts.tm_min)) && input != 'q');
		}else
		{
			Init = 0;
		}
	};


	endwin();
	return 0;

}
