#include"stdafx.h"
#include"header.h"
extern time_t start_t;
extern time_t end_t;
extern time_t deadline_t;
extern time_t result_t;
extern int evolution_time;
extern int best_fitness;
extern int num_machine;
extern int num_job;
extern int lenth;
extern double result_time;

int chromosome[Max_Lenth];	//storing a chromosome
int super_elite[Max_Lenth];	//storing the best one's(suoer_elite) chromosome
int fitness;	//denotes the time used of a chromosome
int generation;		//denotes the generations a chromosome evolutes


int* Local_Optimal();
void Recreate_Chromosome();
void Evolution();
void Update_Elite();
extern int Time_Used(int*);

int* Local_Optimal()
{
	while (end_t<deadline_t)
	{
		fitness = 10000;
		Recreate_Chromosome();
		for (generation = 0; generation < evolution_time; generation++)
			Evolution();
		Update_Elite();
		end_t = clock();
	}
	result_time = (double)(result_t - start_t) / CLOCKS_PER_SEC;
	cout << "Result_time:" << result_time << endl;
	cout << "Super_elite:" << best_fitness << endl;
	return super_elite;
}

void Recreate_Chromosome()
{
	int counter = 0;	//record the lenth of a chromesome which has been finnished
	int*job_recorder = new int[num_job];	//record how many times a job has been arranged
	for (int i = 0; i<num_job; i++)
		job_recorder[i] = 0;
	while (counter != lenth)
	{
		int a = rand() % num_job;	//select a job to arrange
		while (job_recorder[a] == num_machine)
			a = rand() % num_job;
		chromosome[counter] = a;
		counter++;
		job_recorder[a]++;
	}
	delete[] job_recorder;
}

void Evolution()
{
	int pre_totaltime = fitness;
	int index_a;	//select the first location to exchange
	index_a = rand() % lenth;
	int index_b;	//select the second location to exchange
	index_b = rand() % lenth;
	while (index_a == index_b || chromosome[index_a] == chromosome[index_b])
		index_b = rand() % lenth;
	int temp;
	temp = chromosome[index_a];		//exchange the two genes chosen
	chromosome[index_a] = chromosome[index_b];
	chromosome[index_b] = temp;
	int new_fitness = Time_Used(chromosome);
	if (new_fitness > pre_totaltime)		//if the result is worse, go back
	{
		temp = chromosome[index_a];
		chromosome[index_a] = chromosome[index_b];
		chromosome[index_b] = temp;
	}
	else	//successfully evolved
	{
		fitness = new_fitness;
		if (fitness < best_fitness)
		{
			cout << fitness <<"("<<(double)(clock()-start_t)/CLOCKS_PER_SEC<<")"<< endl;
			best_fitness = fitness;
		}
		/*
		if (new_fitness != pre_totaltime)
			cout << "generation(" << generation << "):" << fitness << endl;
			*/
	}
}

void Update_Elite()
{
	if (Time_Used(chromosome) == best_fitness)
	{
		result_t = clock();
		for (int i = 0; i < Max_Lenth; i++)
			super_elite[i] = chromosome[i];
		best_fitness = Time_Used(chromosome);
	}
}