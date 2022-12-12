

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

typedef struct
{
    char jobNo[10];
    char description[20];
    char machineNo[10];
    int duration;
    char clientName[30];
} job;

typedef struct
{
    char jobNo[10];
    char machineNo[10];
    int start;
    int end;
} schedule;

typedef struct
{
    char jobNo[10];
    char description[20];
    char machineNo[10];
    int duration;
    char clientName[30];
    int start;
    int end;
} jobSchedule;

// define structure to hold jobSchedules for each machine
typedef struct
{
    int machineNo;
    jobSchedule jobScheduleList[MAX];
    int jobScheduleCount;
} machineSchedule;

void readFile(job *jobList, int *jobCount, char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "r");
    char line[100];
    int i = 0;
    while (fgets(line, 100, fp) != NULL)
    {
        char *token = strtok(line, ":");
        strcpy(jobList[i].jobNo, token);
        token = strtok(NULL, ":");
        strcpy(jobList[i].description, token);
        token = strtok(NULL, ":");
        strcpy(jobList[i].machineNo, token);
        token = strtok(NULL, ":");
        jobList[i].duration = atoi(token);
        token = strtok(NULL, ":");
        strcpy(jobList[i].clientName, token);
        i++;
    }
    *jobCount = i;
    fclose(fp);
}

void findDisplayAndRemoveInvalidJobs(
    job *jobList,
    int *jobCount,
    char *invalidJobsFileName)
{
    FILE *fp;
    fp = fopen(invalidJobsFileName, "w");
    int i;
    for (i = 0; i < *jobCount; i++)
    {
        if (strlen(jobList[i].jobNo) != 4 ||
            strlen(jobList[i].description) < 1 ||
            strlen(jobList[i].machineNo) != 2 ||
            jobList[i].duration < 1 ||
            strlen(jobList[i].clientName) < 1)
        {
            printf("Invalid job entry: %s", jobList[i].jobNo);
            fprintf(fp, "Invalid job entry: %s", jobList[i].jobNo);
            int j;
            for (j = i; j < *jobCount - 1; j++)
            {
                jobList[j] = jobList[j + 1];
            }
            *jobCount = *jobCount - 1;
            i--;
        }
    }
    fclose(fp);
}

void scheduleJob(
    job *jobList,
    int jobCount,
    schedule *scheduleList,
    int *scheduleCount)
{
    int i;
    for (i = 0; i < jobCount; i++)
    {
        scheduleList[i].start = 1;
        scheduleList[i].end = jobList[i].duration;
        strcpy(scheduleList[i].jobNo, jobList[i].jobNo);
        strcpy(scheduleList[i].machineNo, jobList[i].machineNo);
    }
    *scheduleCount = i;
}

// group scheduleList based on machines
void groupScheduleList(
    schedule *scheduleList,
    int scheduleCount,
    machineSchedule *machineScheduleList,
    int *machineScheduleCount)
{
    int i;
    for (i = 0; i < scheduleCount; i++)
    {

        int machineNo = atoi(scheduleList[i].machineNo + 1);

        int j;
        for (j = 0; j < *machineScheduleCount; j++)

        {
            if (machineScheduleList[j].machineNo == machineNo)
            {
                break;
            }
        }
        if (j == *machineScheduleCount)
        {
            machineScheduleList[j].machineNo = machineNo;
            *machineScheduleCount = *machineScheduleCount + 1;
        }
        machineScheduleList[j].jobScheduleList[machineScheduleList[j].jobScheduleCount].start = scheduleList[i].start;
        machineScheduleList[j].jobScheduleList[machineScheduleList[j].jobScheduleCount].end = scheduleList[i].end;
        strcpy(machineScheduleList[j].jobScheduleList[machineScheduleList[j].jobScheduleCount].jobNo, scheduleList[i].jobNo);
        machineScheduleList[j].jobScheduleCount = machineScheduleList[j].jobScheduleCount + 1;
    }
}

// write schedule from groupScheduleList
void writeSchedule(
    machineSchedule *machineScheduleList,
    int machineScheduleCount)
{
    FILE *fp;
    int i;
    for (i = 0; i < machineScheduleCount; i++)
    {
        char fileName[20];
        sprintf(fileName, "schedule%d.txt", machineScheduleList[i].machineNo);
        fp = fopen(fileName, "w");
        fprintf(fp, "- Schedule for machine %d\n", machineScheduleList[i].machineNo);
        int j;
        for (j = 0; j < machineScheduleList[i].jobScheduleCount; j++)
        {
            fprintf(fp, "%s:%d:%d\n", machineScheduleList[i].jobScheduleList[j].jobNo, machineScheduleList[i].jobScheduleList[j].start, machineScheduleList[i].jobScheduleList[j].end);
        }
        fclose(fp);
    }
}

int main(int argc, char *argv[])
{
    job jobList[MAX];
    int jobCount;
    int i;
    for (i = 1; i < argc; i++)
    {
        readFile(jobList, &jobCount, argv[i]);
    }
    findDisplayAndRemoveInvalidJobs(jobList, &jobCount, "invalidJobs.txt");
    schedule scheduleList[MAX];
    int scheduleCount;
    scheduleJob(jobList, jobCount, scheduleList, &scheduleCount);
    machineSchedule machineScheduleList[MAX];
    int machineScheduleCount = 0;
    groupScheduleList(scheduleList, scheduleCount, machineScheduleList, &machineScheduleCount);
    writeSchedule(machineScheduleList, machineScheduleCount);
    return 0;
}
