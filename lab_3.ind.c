#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>


int errorNum = 0;
int processAmount = 0;
int processMax = 0;
char *word;

#define MAX_LEN 255

int closeFile(FILE *f) {
    int error = 0;
    if (fclose(f) == EOF) {
        perror("File closing error: ");
        error = -1;
    }
    return error;
}

int closeDir(DIR *d) {
    int error = 0;
    if ((closedir(d)) == -1)
    {
        perror("Directory closing error:");
        error = -1;
    }
    return error;
}

void nextdir(char folder[255])
{
    DIR *dfd;

    /*
    struct dirent {
    ino_t          d_ino;        inode number 
    off_t          d_off;        offset to the next dirent 
    unsigned short d_reclen;     length of this record 
    unsigned char  d_type;       type of file; not supported
                                 by all file system types 
    char           d_name[256];  filename 
};
*/
    struct dirent *dp;
    
    if ((dfd = opendir(folder)) == NULL)
    {
        perror("Directory opening error:");
        errorNum = -1;
        return;
    }

    while ((dp = readdir(dfd)) != NULL && (errorNum >= 0))
    {
        if (dp->d_type == DT_REG) 
        {
            char path[MAXNAMLEN];

            strcpy(path, folder);
            strcat(path, "/");
            strcat(path, dp->d_name);

            if (processAmount >= processMax)
            {
                if (wait(NULL) == -1)
                {
                    perror("Waiting error: \n");
                }
                processAmount--;
            }
            
            processAmount++;
            
            pid_t child_pid = fork();
            if (child_pid == -1)
            {
                perror("Fork error: \n");
            }
            if (child_pid == 0)
            {
                if (execl("./count_words", "count_words", path, word, NULL) == -1)
                {
                    perror("Execl error: \n");
                }
                processAmount--;
            }
        }
        else if ((dp->d_type == DT_DIR) && ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) && errorNum >= 0) 
        {
            char next[255];
            strcpy(next, folder);
            strcat(next, "/");
            nextdir(strcat(next, dp->d_name));
        }
        
    }
    if ((closedir(dfd)) == -1)
    {
        perror("Directory closing error:");
        errorNum = -1;
        return;
    }
 
}

long strToInt(char* str)
{
    char* endptr;
    errno = 0;
    long groupSize = strtol(str, &endptr, 10);
    if (errno)
    {
        perror("Invalid processes amount:");
        return -1;
    }
    if (str == endptr)
    {
        fprintf(stderr, "Processes amount must have long type");
        return -1;
    }
    return groupSize;
}

int main(int argc, char *argv[]) 
{
    if (argc != 4)
    {
        fprintf(stderr, "Function %s has 3 parameters:\n", argv[0]);
        fprintf(stderr, "1: Path to directory\n");
        fprintf(stderr, "2: Processes MAX\n");
        fprintf(stderr, "3: Word to search\n");
        return -1;
    }
    char *path = realpath(argv[1], NULL);
    if (path == NULL)
    {
        perror("Invalid directory:");
    }
    processMax = strToInt(argv[2]);
    word = argv[3];
 
    if (processMax < 1)
    {
        fprintf(stderr, "Processes max amount must be > 0");
        return -1;
    }
    
    printf("Directory: %s \n", path);
    nextdir(path);
    int error;
    while (error > 0)
    {
        error = wait(NULL) ;
    }
    if (error < 0 && errno != ECHILD)
    {
        perror("Wait error:");
    }
    free(path);
    printf("\n");
}