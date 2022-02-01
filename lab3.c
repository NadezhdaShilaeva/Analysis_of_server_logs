#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int time_interval;
	scanf("%d", &time_interval);
	FILE* file = fopen("access_log_Jul95", "r");
	time_t t = time(NULL);
	struct tm* date_request = localtime(&t);
	char utc[6], month[4], months[12][4] = { "Jun", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Nov", "Dec" };
	time_t local_time = 0, pre_local_time = 0, start_local_time_max, end_local_time_max, delta_time;
	char request[500];
	int i, status = 0, count_error_request = 0, count_request = 0, count_request_max = 0;
	int* arr_count_request = (int*)calloc(time_interval, sizeof(int));
	char symbol, pre_symbol;
	if (file == NULL)
		return -1;
	pre_symbol = fgetc(file);
	symbol = fgetc(file);
	while (!feof(file))
	{
		while ((symbol != '[') || (pre_symbol != ' '))
		{
			pre_symbol = symbol;
			symbol = fgetc(file);
		}
		pre_local_time = local_time;
		fscanf(file, "%d/%3s/%d:%d:%d:%d",
			&date_request->tm_mday, &month, &date_request->tm_year,&date_request->tm_hour, &date_request->tm_min, &date_request->tm_sec);
		for (i = 0; i < 12; ++i)
		{
			if (strcmp(month, months[i]))
			{
				date_request->tm_mon = i;
				break;
			}
		}
		date_request->tm_year -= 1900;
		local_time = mktime(date_request);
		if (pre_local_time == 0)
		{
			start_local_time_max = local_time;
			end_local_time_max = local_time;
			pre_local_time = local_time;
			fgetc(file);
			fscanf(file, "%5s", &utc);
			utc[5] = '\0';
		}
		if (pre_local_time == local_time)
		{
			++arr_count_request[time_interval - 1];
			++count_request;
		}
		else
		{
			if (count_request > count_request_max)
			{
				count_request_max = count_request;
				end_local_time_max = pre_local_time;
				start_local_time_max = pre_local_time - time_interval + 1;
			}
			delta_time = local_time - pre_local_time;
			for (i = 0; i < time_interval; ++i)
			{
				if (i < delta_time)
				{
					count_request -= arr_count_request[i];
				}
				if (i + delta_time < time_interval)
				{
					arr_count_request[i] = arr_count_request[i + delta_time];
				}
				else
				{
					arr_count_request[i] = 0;
				}
			}
			arr_count_request[time_interval - 1] = 1;
			++count_request;
		}
		while (fgetc(file) != '"');
		pre_symbol = fgetc(file);
		symbol = fgetc(file);
		for (i = 0; (pre_symbol != '"') || (symbol != ' '); ++i)
		{
			request[i] = pre_symbol;
			pre_symbol = symbol;
			symbol = fgetc(file);
		}
		request[i] = '\0';
		fscanf(file, "%d", &status);
		while ((!feof(file)) && (symbol != '\n'))
		{
			symbol = fgetc(file);
		}
		if (status / 100 == 5)
		{
			++count_error_request;
			printf("%s\n", request);
		}
	}
	fclose(file);
	printf("%d\n", count_error_request);
	if (count_request > count_request_max)
	{
		count_request_max = count_request;
		end_local_time_max = pre_local_time;
		start_local_time_max = pre_local_time - time_interval + 1;
	}
	date_request = localtime(&start_local_time_max);
	printf("%s %s\n", asctime(date_request), utc);
	date_request = localtime(&end_local_time_max);
	printf("%s %s\n", asctime(date_request), utc);
	free(arr_count_request);
	printf("%d\n", count_request_max);
	return 0;
}