#include "stdafx.h"
#include"header.h"
extern int fixperiod[30][3];                      //30 rooms are prepared for fix times and 3 rooms for properties
extern time_t start_t;
extern time_t deadline_t;
extern time_t end_t;
extern int best_fitness;
extern int evolution_time;
extern ifstream fin;
extern int num_job;	//denotes the number of jobs
extern int num_machine;	//denotes the number of machines
extern int** machine_map;	//storing input data of machines
extern int** time_map;	//storing input data of the time for each job
extern int lenth;	//denotes the lenth of a chromosome(Ⱦɫ�壩

void Init_Factors();
void Init_Input();

void Init_Factors()
{
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 3; j++)
			fixperiod[i][j] = 0;
	start_t = clock();
	deadline_t = start_t + 280 * CLOCKS_PER_SEC;		//time limit
	end_t = clock();
	best_fitness = 10000;
	srand((int)time(0));
	evolution_time = 1500000;
}

void Init_Input()
{
	fin.open("testdata.txt");	//change the path here
	fin >> num_job >> num_machine;
	lenth = num_machine * num_job;	//get the lenth of a chromosome
	time_map = new int*[num_job];
	for (int i = 0; i<num_job; i++)
	{
		time_map[i] = new int[num_machine];
	}
	machine_map = new int*[num_job];
	for (int i = 0; i<num_job; i++)
	{
		machine_map[i] = new int[num_machine];
	}

	for (int i = 0; i<num_job; i++)
		for (int j = 0; j<num_machine; j++)
		{
			fin >> machine_map[i][j];
			fin >> time_map[i][j];
		}
}