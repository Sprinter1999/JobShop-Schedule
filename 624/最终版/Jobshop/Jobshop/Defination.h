#pragma once
#include "stdafx.h"
#include"header.h"

ofstream fout;
ifstream fin;
int num_job;	//denotes the number of jobs
int num_machine;	//denotes the number of machines
int** machine_map;	//storing input data of machines
int** time_map;	//storing input data of the time for each job
int lenth;	//denotes the lenth of a chromosome(Ⱦɫ�壩
int evolution_time;	//the times a chromosome evoluates
int best_fitness;	//denotes the super_elite's chromosome

int Time[MAX_MATRIX][MAX_MATRIX];             //job i in operation j--->time needed
int MachineNum[MAX_MATRIX][MAX_MATRIX];       //job i in operation j--->machine number
time_t start_t, end_t, deadline_t, result_t;
double result_time; //as the name suggests
int Time_Used(int*);

int Time_Used(int *chromosome)
{
	int total_time;
	int object, state, machine, time;
	int i;
	int* machine_endtime = new int[num_machine];
	int* object_endtime = new int[num_job];
	int* object_state = new int[num_job];
	for (int i = 0; i<num_machine; i++)
		machine_endtime[i] = 0;
	for (int i = 0; i<num_job; i++)
	{
		object_state[i] = 0;
		object_endtime[i] = 0;
	}
	for (i = 0; i < num_job * num_machine; i++)
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
	}
	for (i = 0, total_time = machine_endtime[i]; i < num_machine; i++)
	{
		if (total_time < machine_endtime[i])
			total_time = machine_endtime[i];
	}
	delete[]machine_endtime;
	delete[]object_endtime;
	delete[] object_state;
	return total_time;
}