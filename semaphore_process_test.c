#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEM_NAME "/semaphore_example"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

#define TMP_FILE "./tmp.txt"

void write_file(int _pid)
{
    FILE *fp;
    int i = 0;
    printf("Process %d is writing\n", _pid);
    fp = fopen(TMP_FILE, "a");
    if(NULL == fp)
    {
        perror("fp open file failed");
    }
    fprintf(fp, "Process %d\n", _pid);
    for(i = 0; i < 5000000; i++)
    {
        fprintf(fp, "%d ", i%10);
    }
    fprintf(fp, "\n");
    fclose(fp);
    return;
}

int main(void)
{
    int i;
    int n;
    sem_t *semaphore;

    semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphore) < 0)
    {
        perror("sem_close(3) failed");
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);
    }

    n = fork();
    if(n < 0)
    {
        perror("fork() failed");
    }
    else if(0 == n)/*child*/
    {
        sem_t *semaphore = sem_open(SEM_NAME, O_RDWR);
        sem_wait(semaphore);
        write_file((int)getpid());
        sem_post(semaphore);
        sem_close(semaphore);
        sleep(3);
        printf("child finish\n");
    }
    else if(n > 0)/*parrent*/
    {
        sem_t *semaphore = sem_open(SEM_NAME, O_RDWR);
        sem_wait(semaphore);
        write_file((int)getpid());
        sem_post(semaphore);
        sem_close(semaphore);
        if (waitpid(n, NULL, 0) < 0)
            perror("waitpid(2) failed");

        if (sem_unlink(SEM_NAME) < 0)
            perror("sem_unlink(3) failed");
            
        printf("parrent finish\n");
    }
    return 0;
}
