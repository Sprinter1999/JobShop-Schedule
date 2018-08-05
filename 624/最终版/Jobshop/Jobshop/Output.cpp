#include"stdafx.h"
#include"header.h"
extern ofstream fout;
extern ifstream fin;
extern int num_job;	//denotes the number of jobs
extern int num_machine;	//denotes the number of machines
extern int** machine_map;	//storing input data of machines
extern int** time_map;	//storing input data of the time for each job
extern time_t start_t;
extern time_t end_t;
extern time_t deadline_t;
extern time_t result_t;
extern double result_time;
extern int best_fitness;

//some variable about the fix instructions
int fix_machine;
int fix_time[20];
int fix_begin[20];
int fix_end[20];
int fix[20];
int if_fix;
int time1;
HANDLE Fix_Mutex;
int Fixtimes = 0;                             //the times that we input fix instructions
int fixperiod[30][3]; //30 rooms are prepared for fix times and 3 rooms for properties
FILE * fp;

typedef struct {
	int begin;
	int end;
	int job;
	int operation;
} RR;

RR result[50][50];
void Output_Total(int *);
int Output_Txt(int *);
void Output_Graph_1(RR[][50]);
void input_fix();
void Output_Graph_2(RR[][50],int *);
void output_testdata();
void output_changedGantte();
void Print();
void Fixing(int*);
void output_new_solution();

void Output_Total(int * super_elite)
{
	Output_Txt(super_elite);
	system("cls");
	Output_Graph_1(result);
	DWORD Fix_Thread_ID = 2;
	HANDLE Fix_Thread;
	Fix_Mutex = CreateMutex(NULL, false, NULL);
	Fix_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)input_fix, NULL, 0, &Fix_Thread_ID);
	Output_Graph_2(result,super_elite);
	CloseHandle(Fix_Thread);
	Sleep(500);
	system("pause");
}


//Input the data from the txt
int Output_Txt(int *chromosome)
{
	fout.open("output.txt");
	int **start_time, **end_time, **state_map, **object_map;
	int *machine_jobs, *machine_endtime, *object_endtime, *object_state;
	int object, state, machine, time, total_time, i, j;
	start_time = (int**)malloc(sizeof(int *) * num_machine);	//输出结果中每项第1个参数：本次加工开始时间 
	end_time = (int**)malloc(sizeof(int *) * num_machine);		//输出结果中每项第4个参数：本次加工结束时间 
	state_map = (int**)malloc(sizeof(int *) * num_machine);		//输出结果中每项第3个参数：本次加工是该工件几号步骤 
	object_map = (int**)malloc(sizeof(int *) * num_machine);	//输出结果中每项第2个参数：本次加工是几号工件 
	machine_jobs = (int*)malloc(sizeof(int) * num_machine);		//该工作为该机器第几号工作，输出结果的横坐标 
	machine_endtime = (int*)malloc(sizeof(int) * num_machine);		//该机器当前工作结束时间 
	object_endtime = (int*)malloc(sizeof(int) * num_job);		//该工件本次工作结束时间 
	object_state = (int*)malloc(sizeof(int) * num_job);			//该工件即将进行的工作编号 
	for (i = 0; i < num_machine; i++)
	{
		start_time[i] = (int*)malloc(sizeof(int) * num_job);
		end_time[i] = (int*)malloc(sizeof(int) * num_job);
		state_map[i] = (int*)malloc(sizeof(int) * num_job);
		object_map[i] = (int*)malloc(sizeof(int) * num_job);
	}

	for (int i = 0; i<num_machine; i++)
	{
		machine_endtime[i] = 0;
		machine_jobs[i] = 0;
	}
	for (int i = 0; i<num_job; i++)
	{
		object_state[i] = 0;
		object_endtime[i] = 0;
	}

	for (i = 0; i < num_job * num_machine; i++)	//填写输出结果表 
	{
		object = chromosome[i];
		state = object_state[object];
		object_state[object] ++;
		machine = machine_map[object][state];
		time = time_map[object][state];

		if (machine_endtime[machine] > object_endtime[object])
		{
			machine_endtime[machine] = machine_endtime[machine] + time;
			object_endtime[object] = machine_endtime[machine];
		}
		else
		{
			machine_endtime[machine] = object_endtime[object] + time;
			object_endtime[object] = machine_endtime[machine];
		}

		end_time[machine][machine_jobs[machine]] = machine_endtime[machine];
		start_time[machine][machine_jobs[machine]] = end_time[machine][machine_jobs[machine]] - time;
		object_map[machine][machine_jobs[machine]] = object;
		state_map[machine][machine_jobs[machine]] = state;
		machine_jobs[machine] ++;
	}
	for (i = 0, total_time = 0, total_time = machine_endtime[i]; i < num_machine; i++)
	{
		if (total_time < machine_endtime[i])
			total_time = machine_endtime[i];
	}

	for (i = 0; i < num_machine; i++)
	{
		fout << "M" << i;
		for (j = 0; j < num_job; j++)
		{
			fout << " " << "(" << start_time[i][j] << "," << object_map[i][j] << "-" << state_map[i][j] << "," << end_time[i][j] << ")";
			result[i][j].begin = start_time[i][j];
			result[i][j].job = object_map[i][j];
			result[i][j].operation = state_map[i][j];
			result[i][j].end = end_time[i][j];
		}
		fout << endl;
	}
	fout << "Time Used:" << result_time << "s" << endl;
	fout << "End Time:" << best_fitness << endl;

	for (i = 0; i < num_machine; i++)		//释放内存 
	{
		free(start_time[i]);
		free(end_time[i]);
		free(state_map[i]);
		free(object_map[i]);
	}
	free(end_time);
	free(state_map);
	free(object_map);
	free(machine_jobs);
	free(machine_endtime);
	free(object_endtime);
	free(object_state);

	return total_time;
}

//produce:Gantte Graph&best solution
void Output_Graph_1(RR result[][50])
{
	initgraph(1300, 600);
	setbkcolor(WHITE);
	setcolor(GREEN);
	line(20, 445, 20, 10);
	line(10, 20, 20, 10);
	line(20, 10, 30, 20);
	line(20, 445, 1300, 445);
	line(1290, 435, 1300, 445);
	line(1290, 455, 1300, 445);
	int i, j;

	output_testdata();
	printf("\n");
	for (i = 0; i <num_machine; i++)
	{
		printf("M%d", i);
		for (j = 0; j <num_job; j++)
		{
			if (result[i][j].begin == EOF) break;                //end when there is no data-in
			printf(" (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job, result[i][j].operation, result[i][j].end);
			setfillcolor(hsl2rgb(float((result[i][j].job) * 36), 1.0f, 0.5f));
			setfont(14, 0, "黑体");
			setcolor(GREEN);
			xyprintf(0, 10 * 4 * i, "M%d", i);
			bar(result[i][j].begin + 20, 10 * 4 * i, result[i][j].end + 20, 10 * 4 * i + 20);
			setfont(10, 0, "黑体");
			setcolor(WHITE);
			xyprintf(result[i][j].begin + 20, 445, "%d", result[i][j].begin);
			xyprintf(result[i][j].end + 20, 445, "%d", result[i][j].end);
			setfont(9, 0, "黑体");
			setcolor(BLACK);
			setbkmode(TRANSPARENT);
			xyprintf(result[i][j].begin + 20, 10 * 4 * i, "%d-%d", result[i][j].job, result[i][j].operation);
		}
		printf("\n");
	}
	setcolor(BLACK);
	setfont(20, 0, "黑体");
	xyprintf(20, 500, "总时间：%d", best_fitness);
	printf("Time Used: %.3lfs\n", result_time);
	printf("End Time: %d\n", best_fitness);
	getch();
	closegraph();
}

//affix the fix instructions
void input_fix()
{
	WaitForSingleObject(Fix_Mutex, INFINITE);
	while (1)
	{
		getchar();
		int fixtime;
		scanf("%d %d", &fix_machine, &fixtime);
		if (fixperiod[fix_machine][0] == 0)
		{
			fixperiod[fix_machine][0] = 1;                     //"0" place stores the machine in need of fixing
			fixperiod[fix_machine][1] = time1;               //"1" place stores the time that begins
			fixperiod[fix_machine][2] = time1 + fixtime;    //"2" place stores the time that ends
		}
		if_fix = 1;                                        //which denotes that fix instruction exists
	}
	ReleaseMutex(Fix_Mutex);
}
//produce the changed gantte graph and changed solution
void Output_Graph_2(RR result[][50],int* super_elite)
{
	fp = fopen("output2.txt", "w");                     //firstly produce the target file output2
	time1 = 0;
	while (time1 <= best_fitness)
	{
		Print();
		if (if_fix)                     //if fix instruction is truly needed....
			Fixing(super_elite);
	}

	output_changedGantte();
	output_new_solution();
	double alltime;
	alltime = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	printf("Time Used: %.3lfs\n", alltime);
	fprintf(fp, "Time Used: %.3lfs\n", alltime);
	printf("End Time: %d\n", best_fitness);
	fprintf(fp, "End Time: %d\n", best_fitness);
	fclose(fp);
	getch();
	closegraph();
}
//produce the testdata.txt
void output_testdata()
{
	ifstream out("testdata.txt", ios::in);
	string str;
	while (!out.eof())
	{
		getline(out,str);
		cout << str << endl;
	}
	out.close();
}

void output_changedGantte()
{
	initgraph(1300, 600);
	setbkcolor(WHITE);
	setcolor(GREEN);
	line(20, 445, 20, 10);
	line(10, 20, 20, 10);
	line(20, 10, 30, 20);
	line(20, 445, 1300, 445);
	line(1290, 435, 1300, 445);
	line(1290, 455, 1300, 445);
	for (int i = 0; i < num_machine; i++)
	{
		if (result[i][0].begin == -1)
			continue;
		for (int j = 0; j < num_job; j++) {
			if (result[i][j].begin == -1)           //end where there is no data-in
				break;
			setfont(12, 0, "黑体");
			setcolor(GREEN);
			xyprintf(0, 10 * 4 * i, "M%d", i);
			if (fixperiod[i][0] && fixperiod[i][1] <= result[i][j].end&&fixperiod[i][2] >= result[i][j].begin)
			{
				setcolor(RED);
				setfillcolor(BLACK);
				bar(fixperiod[i][1] + 20, 10 * 4 * i, fixperiod[i][2] + 20, 10 * 4 * i + 20);
				setfont(14, 0, "黑体");
				xyprintf(fixperiod[i][1] / 2 + fixperiod[i][2] / 2 + 10, 10 * 4 * i, "检修");
			}
			setfillcolor(hsl2rgb(float((result[i][j].job) * 36), 1.0f, 0.5f));
			bar(result[i][j].begin + 20, 10 * 4 * i, result[i][j].end + 20, 10 * 4 * i + 20);
			setfont(8, 0, "黑体");
			setcolor(WHITE);
			xyprintf(result[i][j].begin * 10 + 20, 445, "%d", result[i][j].begin);
			xyprintf(result[i][j].end * 10 + 20, 445, "%d", result[i][j].end);
			setfont(8, 0, "黑体");
			setcolor(BLACK);
			setbkmode(TRANSPARENT);
			xyprintf(result[i][j].begin + 20, 10 * 4 * i, "%d-%d", result[i][j].job, result[i][j].operation);
		}
	}
	setcolor(BLACK);
	setfont(20, 0, "黑体");
	xyprintf(20, 500, "总时间：%d", best_fitness);
}

void Print()
{
	int i, j;
	int flag;
	time1 = time1 + 9;                     //100 is the time gap that we output the fix procedure
	printf("\n");                            //for we want to make this fix procedure faster
	printf("当前时间：%d\n", time1);
	int k, p;
	for (i = 0; i < num_machine; i++)
	{
		j = 0;
		flag = 0;
		while (j<num_job)
		{
			if (result[i][j].begin <= time1 && result[i][j].end >= time1)
			{
				printf("M%d 加工 当前工件-工序：%d-%d 当前状态已持续时长：%d 当前状态结束时间: %d\n", i, result[i][j].job, result[i][j].operation, time1 - result[i][j].begin, result[i][j].end);
				flag = 1;
				break;
			}
			j++;
		}
		bool fixing = true;
		if (!flag)fixing = false;
		if (flag == 0)
		{
			if (fixperiod[i][0] && time1 >= fixperiod[i][1] && time1 <= fixperiod[i][2])
			{
				printf("M%d 检修\n", i);
				fixing = true;
			}
		}
		if (!fixing)
			printf("M%d 空闲\n", i);
	}
	printf("\n");
	api_sleep(3000);                 //during this 3000ms,we need to input our fix instruction
}

void Fixing(int* super_elite)
{
	int **start_time, **end_time, **state_map, **object_map;
	int *machine_jobs, *machine_endtime, *object_endtime, *object_state;
	int object, state, machine, time, total_time, i, j;
	start_time = (int**)malloc(sizeof(int *) * num_machine);
	end_time = (int**)malloc(sizeof(int *) * num_machine);
	state_map = (int**)malloc(sizeof(int *) * num_machine);
	object_map = (int**)malloc(sizeof(int *) * num_machine);
	machine_jobs = (int*)malloc(sizeof(int) * num_machine);
	machine_endtime = (int*)malloc(sizeof(int) * num_machine);
	object_endtime = (int*)malloc(sizeof(int) * num_job);
	object_state = (int*)malloc(sizeof(int) * num_job);
	for (i = 0; i < num_machine; i++)
	{
		start_time[i] = (int*)malloc(sizeof(int) * num_job);
		end_time[i] = (int*)malloc(sizeof(int) * num_job);
		state_map[i] = (int*)malloc(sizeof(int) * num_job);
		object_map[i] = (int*)malloc(sizeof(int) * num_job);
	}
	for (int i = 0; i < num_machine; i++)
	{
		machine_endtime[i] = 0;
		machine_jobs[i] = 0;
	}
	for (int i = 0; i < num_job; i++)
	{
		object_state[i] = 0;
		object_endtime[i] = 0;
	}

	for (i = 0; i < num_job * num_machine; i++)
	{
		object = super_elite[i];
		state = object_state[object];
		object_state[object] ++;
		machine = machine_map[object][state];
		time = time_map[object][state];
		int change_start = 0;
		int change_end = 0;
		bool change = false;
		if (machine_endtime[machine] > object_endtime[object])
		{
			int temp_start = machine_endtime[machine];
			int temp_end = machine_endtime[machine] + time;
			if (fixperiod[machine][0] && temp_start <= fixperiod[machine][2] && temp_end >= fixperiod[machine][1])
			{
				temp_start = fixperiod[machine][2];
				temp_end = temp_start + time;
				change_start = temp_start;
				change_end = temp_end;
				change = true;
			}
			machine_endtime[machine] = temp_end;
			object_endtime[object] = temp_end;
		}
		else
		{
			int temp_start = object_endtime[object];
			int temp_end = object_endtime[object] + time;
			if (fixperiod[machine][0] && temp_start <= fixperiod[machine][2] && temp_end >= fixperiod[machine][1])
			{
				temp_start = fixperiod[machine][2];
				temp_end = temp_start + time;
				change_start = temp_start;
				change_end = temp_end;
				change = true;
			}
			machine_endtime[machine] = temp_end;
			object_endtime[object] = temp_end;
		}

		end_time[machine][machine_jobs[machine]] = machine_endtime[machine];
		start_time[machine][machine_jobs[machine]] = end_time[machine][machine_jobs[machine]] - time;
		object_map[machine][machine_jobs[machine]] = object;
		state_map[machine][machine_jobs[machine]] = state;
		machine_jobs[machine] ++;
		if (change)
		{
			change = false;
		}
	}
	for (i = 0, total_time = 0, total_time = machine_endtime[i]; i < num_machine; i++)
	{
		if (total_time < machine_endtime[i])
			total_time = machine_endtime[i];
	}
	best_fitness = total_time;
	for (i = 0; i < num_machine; i++)                        //store the RR result[][50] table
	{
		for (j = 0; j < num_job; j++)
		{
			result[i][j].begin = start_time[i][j];
			result[i][j].job = object_map[i][j];
			result[i][j].operation = state_map[i][j];
			result[i][j].end = end_time[i][j];
		}
	}

	for (i = 0; i < num_machine; i++)                      //free~
	{
		free(start_time[i]);
		free(end_time[i]);
		free(state_map[i]);
		free(object_map[i]);
	}
	free(end_time);
	free(state_map);
	free(object_map);
	free(machine_jobs);
	free(machine_endtime);
	free(object_endtime);
	free(object_state);
	if_fix = 0;
}

void output_new_solution()
{
	for (int i = 0; i < num_machine; i++)        //to screen and file:output2
	{
		printf("M%d", i);                  //print on the screen
		fprintf(fp, "M%d", i);            //print on the file output2.txt
		for (int j = 0; j < num_job; j++)
		{
			if (result[i][j].begin == -1)
				break;
			if (result[i][j].begin <= fixperiod[i][2] &&fixperiod[i][0]&& result[i][j].end >= fixperiod[i][1])
			{
				printf(" (%d,检修,%d)", fixperiod[i][1], fixperiod[i][2]);
				fprintf(fp, " (%d,检修,%d)", fixperiod[i][1], fixperiod[i][2]);
			}
			printf(" (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job, result[i][j].operation, result[i][j].end);
			fprintf(fp, " (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job, result[i][j].operation, result[i][j].end);
		}
		printf("\n");
		fprintf(fp, "\n");
	}
}
