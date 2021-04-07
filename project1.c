#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct process
{
	int pID;
	int burst;
	int priority;
	int index;
};

struct process processes[100];

int main(int argc, char *argv[])
{
	int numProc;
	int numExec;
	int numInstruct;
	char *line = NULL;
	int lineLength;
	size_t size;
	int vcs = 1;
	int nvcs = 0;
	int burstTotal = 0;
	int avgWait = 0;
	int avgTurn = 0;
	int avgResp = 0;
	double cpu = 100.00;
	double tp = 0.0;
	double turnTime = 0.0;
	double waitTime = 0.0;
	double respTime = 0.0;
	FILE *file;
	bool voluntary = true;
	bool firstOccur = true;
	bool lastOccur = true;

	//check if .txt file name was passed as argument, otherwise assign file pointer to STDIN
	file = fopen(argv[1], "r");
	if(file == NULL)
	{
		file = stdin;
	}
	else {
		if(getline(&line, &size, file) < 0)
		{
			fprintf(stderr, "getline() error from stdin on P");
		}
		else {
			//store # of available processes locally
			numProc = atoi(line);
			if(getline(&line, &size, file) < 0)
			{
				fprintf(stderr, "getline() error from stdin on pN");
			}
			else {
				//store thread count and # of instructions locally by tokenizing
				numExec = atoi(strtok(line, " "));
				numInstruct = atoi(strtok(NULL, " "));
				//now that we know the # of instructions, loop "numInstruct"-many times and assign individual "process" attributes and store struct in array
				for(int i = 0; i < numInstruct; ++i)
				{
					if(getline(&line, &size, file) < 0)
					{
						fprintf(stderr, "getline() error from STDIN on pbp%d", i);
					}
					else {
						processes[i].pID = atoi(strtok(line, " "));
						processes[i].burst = atoi(strtok(NULL, " "));
						processes[i].priority = atoi(strtok(NULL, " "));
						processes[i].index = i;
					}
				}
			}
		}
	}
	//At this point, all struct values are assigned and ready for computation
	//begin by computing # of voluntary/non-voluntary context switches
	for(int j = 0; j < numInstruct-1; ++j)
	{
		//check if current index's pid is equal to the pid of the following process
		//if not, make sure the pid of the next process is the occurrence by iterating through all previous elements
		if(processes[j].pID != processes[j+1].pID)
		{
			for(int k = 0; k < j; ++k)
			{
				//pid of next process is same as current
				if(processes[j+1].pID == processes[k].pID)
				{
					++nvcs;
					voluntary = false;
					break;
				}
			}
		}
		else {
			voluntary = false;
		}
		if(voluntary)
		{
			++vcs;
		}
		voluntary = true;
	}
	//now that we have context switch values, move on to CPU utilization
	cpu = cpu/(double)numProc;

	//throughput/turnaround/waiting/response calculation

	for(int m = 0; m < numInstruct; ++m)
	{
		burstTotal += processes[m].burst;
		for(int n = m+1; n < numInstruct-1; ++n)
		{
			if(processes[m].pID == processes[n].pID)
			{
				lastOccur = false;
				break;
			}
		}
		for (int q = 0; q < m; ++q)
		{
			if(processes[q].pID == processes[m].pID)
			{
				firstOccur = false;
			}
		}
		if(firstOccur)
		{
			for(int r = 0; r < m; ++r)
			{
				avgResp += processes[r].burst;
			}
		}
		else {
			firstOccur = true;
		}
		if(lastOccur)
		{
			for(int p = 0; p < m+1; ++p)
			{
				if(p == m)
				{
					avgTurn += processes[p].burst;
				}
//check for previous occurrences
				if(processes[p].pID != processes[m].pID)
				{
					avgTurn += processes[p].burst;
					avgWait += processes[p].burst;
				}
			}
		}
		else {
			lastOccur = true;
		}
	}
	tp = (double)numExec/(double)burstTotal;
	turnTime = (double)avgTurn/(double)numExec;
	waitTime = (double)avgWait/(double)numExec;
	respTime = (double)avgResp/(double)numExec;

	//print values
	printf("%d\n%d\n%.2f\n%.2f\n%.2f\n%.2f\n%.2f\n", vcs, nvcs, cpu, tp, turnTime, waitTime, respTime);
	return 0;
}
