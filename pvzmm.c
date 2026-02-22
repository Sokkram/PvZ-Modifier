#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/uio.h>
#include <dirent.h>
#include <stdint.h>


typedef struct{
    uintptr_t add;
}candidate;
candidate *candidates = NULL;
size_t cant_cont = 0;


pid_t findprocess(const char *name){
    DIR *dir = opendir("/proc");
    if(dir==NULL)
        return -1;
    else{
        struct dirent *ent;
        while(1){
            ent = readdir(dir);
            if(ent==NULL)
                break;
            if(!isdigit(ent->d_name[0]))
                continue;
            char path[255];
            sprintf(path,"/proc/%s/comm",ent->d_name);
            FILE *evelyn = fopen(path,"r");
            if(evelyn==NULL)
                continue;
            char pathgoodiguess[255];
            fgets(pathgoodiguess,sizeof(pathgoodiguess),evelyn);
            fclose(evelyn);
            pathgoodiguess[strlen(pathgoodiguess)-1] = '\0';
            if(strcmp(pathgoodiguess,name)==0){
                closedir(dir);
                return atoi(ent->d_name);
            }
        }
        closedir(dir);
        return -1;
    }
}


void scan(pid_t pid, int val){
    FILE *maps;
    char lin[512];
    char mapspath[128];
    sprintf(mapspath, "/proc/%d/maps", pid);
    maps = fopen(mapspath, "r");
    if(!maps) return;

    while(fgets(lin, sizeof(lin), maps)){
        uintptr_t start, end;
        char permiso[6];
        sscanf(lin, "%lx-%lx %4s",&start,&end, permiso);
        if(!(permiso[0]=='r' && permiso[1]=='w')) continue;

        size_t size = end - start;
        char *buffer = malloc(size);
        if(!buffer) continue;
        struct iovec local = {buffer, size};
        struct iovec remote = {(void *)start,size};

        ssize_t nread = process_vm_readv(pid, &local, 1, &remote, 1,0);
        if(nread<=0){
            free(buffer);
            continue;
        }
        for(size_t i = 0; i + sizeof(int) <= nread; i+=sizeof(int)){
            int value = *(int*)(buffer + i);
            if(value == val) {
                candidates = realloc(candidates, (cant_cont + 1) * sizeof(candidate));
                candidates[cant_cont].add = start + i;
                cant_cont++;
            }
        }
        free(buffer);
    }
    fclose(maps);
    printf("Found: %zu candidates\n",cant_cont);
}

void fliter(pid_t pid, int damnson){
    size_t newcont =0;
    for(size_t i = 0; i < cant_cont; i++){
        int valll = 0;
        struct iovec local = {&valll, sizeof(int)};
        struct iovec remote = {(void*)candidates[i].add, sizeof(int)};

        if(process_vm_readv(pid,&local,1,&remote,1,0) <= 0) continue;
        if (valll = damnson){
            candidates[newcont++] = candidates[i];
        }
    }
    cant_cont=newcont;
    candidates = realloc(candidates,cant_cont * sizeof(candidate));

    printf("Filtered, %zu candidates remain\n", cant_cont);
}

void newsuns(pid_t pid, int suuun){
    if(candidates == 0 ) return;
    struct iovec local = {&suuun, sizeof(int)};
    //struct iovec remote = {(void*)candidates[0].add, sizeof(int)};
    //process_vm_writev(pid, &local,1,&remote,1,0);
    for(int i = 0; i<=cant_cont; i++){
        struct iovec remote = {(void*)candidates[i].add, sizeof(int)};
        process_vm_writev(pid, &local,1,&remote,1,0);
    }
}


int main(){
    int opc;
    int pid = findprocess("popcapgame1.exe");

    if(pid == -1)
        printf("popcapgame1.exe couldn't be found. Please make sure 'popcapgame1.exe' is running before starting this.");
    else{
        printf("Process found! pid: %d\n",pid);
        do{
            system("clear");
            printf("|||| PvZ Modifier Pre-alpha By Okeraam ||||\n");
            printf("1) Suns\n");
            printf("2) Money\n");
            printf("3) Exit\n");
            scanf("%i",&opc);

            switch(opc){
                case 1:
                    int sun;
                    printf("Type the number of suns you currently have: ");
                    scanf("%d",&sun);
                    scan(pid,sun);
                    printf("\nType the new number of suns you currently have: ");
                    scanf("%d",&sun);
                    fliter(pid,sun);

                    printf("Type the amount of suns you want to add: ");
                    scanf("%d",&sun);
                    newsuns(pid,sun);
                    
                    break;
                case 2:
                    printf("This feature is not working yet :)");
                    getchar();
                    break;
            }
        }while(opc!=3);
    }
    return 0;
}
