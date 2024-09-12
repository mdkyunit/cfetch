#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
//#include <cjson/cJSON.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include "cfetch.h"
char chr;
int llength;
int voffset = 0;
int hoffset = 8;

void outpfetch(bool savest);
void saveart(int argc, char **argv);
void drawoutp(char **agrv, int voffset, int hoffset);
void popconf();
void args(int argc, char **argv);
void prcsstring(const char *src, char *dst, char delimiter);
void getdigits(const char *src, char *dst);

int main(int argc, char **argv) {


    char configf[4][20]; 

    FILE * config = fopen("cfetchconf", "r");
    FILE * art = fopen("cfetchart", "r");
    if (art == NULL) {
        art = fopen("cfetchart", "w");
        printf("cfetch: No art file detected, populating...\n");
        usleep(1000000);
        fclose(art);
    } else {
    fclose(art);
    }
    if (config == NULL) {
        popconf();
        printf("cfetch: No config detected, populating...\n");
        usleep(1000000);
        config = fopen("cfetchconf", "r");
    }
    fscanf(config, "%s", configf[0]);
    fseek(config, 0, SEEK_CUR);
    fscanf(config, "%s", configf[1]);

    fclose(config);
    

    voffset = atoi(&configf[0][9]); //could have done this the whole time for config. hours spent...
    hoffset = atoi(&configf[1][9]); 

    args(argc, argv);
    return 0;

    //printf("Newlines Detected %d\n", newlcnt);
    //printf("Characters Detected %d\n", chrnt);
    //printf("Characters per one line %d\n", chrprln);
    //printf("OS: %s %s %s\n", Linux.sysname, Linux.release, Linux.machine);
    //printf("%d\n", llength);

}


void saveart(int argc, char **argv) {

    FILE* sart = fopen(argv[2], "r");
    FILE* svart = fopen("cfetchart", "w");
    while ((chr = fgetc(sart)) != EOF) {
        putc(chr, svart);
    }
    printf("cfetch: art saved successfully. Run 'cfetch' to display\n");
    fclose(svart);
    fclose(sart);
    popconf();
    exit(0);
}

void drawoutp(char **argv, int voffset, int hoffset) {
    int i = 0; // used for loops
    struct utsname Linux;

    uname(&Linux);
    int memory;
    int memoryfree;
    char grepoutp[7][75]; // = malloc(100);
    FILE *mem = popen("grep \"MemTotal\" /proc/meminfo", "r");
    FILE *memu = popen("grep \"MemAvailable\" /proc/meminfo", "r");
    FILE *cpu = popen("grep -m 1 \"model name\" /proc/cpuinfo", "r");
    FILE *uptime = popen("uptime -p", "r");
    FILE *device = popen("hostnamectl | grep \"Hardware Model:\"","r");
    FILE *OS = popen("hostnamectl | grep \"Operating System:\"","r");
    FILE *Kernel = popen("hostnamectl | grep \"Kernel:\"","r");
    //FILE *ex = popen("grep \"MemFree\" /proc/meminfo", "r");
    fgets(grepoutp[0], 65, mem); 
    fgets(grepoutp[1], 65, memu);
    fgets(grepoutp[2], 65, cpu);
    fgets(grepoutp[3], 65, uptime); 
    fgets(grepoutp[4], 65 , device);
    fgets(grepoutp[5], 65 , OS);
    fgets(grepoutp[6], 65 , Kernel);

    pclose(mem);
    pclose(memu);
    pclose(cpu);
    pclose(uptime);
    pclose(device);
    pclose(OS);
    pclose(Kernel);

    //printf("Grep outp size %lu\n", sizeof(grepoutp)+2);
    //char (*sysinfo)[300];
    //printf("Mem alloc=%lu\n",sizeof(*sysinfo)*7);
    //sysinfo = calloc(300, sizeof(*sysinfo)); 
    char (*sysinfo)[170] = calloc(7, sizeof(*sysinfo)); //Dont mess this up next time lol.

    getdigits(grepoutp[0], sysinfo[0]);
    getdigits(grepoutp[1], sysinfo[1]);
    prcsstring(grepoutp[2], sysinfo[2], ':');
    prcsstring(grepoutp[3], sysinfo[3], ' ');
    prcsstring(grepoutp[4], sysinfo[4], ':');
    prcsstring(grepoutp[5], sysinfo[5], ':');
    prcsstring(grepoutp[6], sysinfo[6], ':');

    //printf("CPU info %s\n", sysinfo[2]);
    
    memory = atoi(sysinfo[0]);
    memoryfree = atoi(sysinfo[1]);
    //printf("memfree=%d",memoryfree);
    memory = memory/1024;
    memoryfree = memory - (memoryfree/1024);
    char info[9][100];
    //char *red = {"\e[31m"};
    sprintf(info[0], "\e[1m%s\e[0m@\e[1m%s\e[0m\'", getenv("USER"), Linux.nodename );
    sprintf(info[1], "---------------------------\'");
    sprintf(info[2], "\e[1mHost:\e[0m%s\'", sysinfo[4]);
    sprintf(info[3], "\e[1mOS:\e[0m%s %s\'", sysinfo[5], Linux.machine);
    sprintf(info[4], "\e[1mKernel:\e[0m%s\'", sysinfo[6]);
    sprintf(info[5], "\e[1mDE:\e[0m %s %s\'", getenv("XDG_CURRENT_DESKTOP"), getenv("GDMSESSION"));
    sprintf(info[6], "\e[1mMemory:\e[0m %dmB / %dmB\'",memoryfree, memory);
    sprintf(info[7], "\e[1mCPU:\e[0m%s\'",sysinfo[2]);
    sprintf(info[8], "\e[1mUptime:\e[0m %s\'", sysinfo[3]);
    
    free(sysinfo);

    FILE *art = fopen("cfetchart", "r");

    int newlcnt = 0; // Newline Count
    int chrnt = 0; // Character count
    int chrprln = 0; // Chr Count/Newline Count
    int h = 0;
    while ((chr = fgetc(art)) != EOF) {
        if (chr == '\n'){
            newlcnt++;
            llength = ftell(art);
            h = 0;
            while (h < hoffset) {
                putchar(' ');
                ++h;
            }
            int index = newlcnt - voffset - 1; // index = -4 if 1 - 4 - 1, if it is below zero, it wont print.
             if (index >= 0 && index < 9) { // Remember this to not have a GIGANTIC switch statement.
                while (info[index][i] != '\'') {
                    putchar(info[index][i]);
                    ++i;
                }
            }
        }
        i = 0;
        ++chrnt;
        putchar(chr);
    }
    //chrprln = chrnt/newlcnt;
    puts("\n");
    //printf("Length: %d\n",llength);
    fclose(art);
}
void popconf() {
    char Settings[3][20];
    FILE* config = fopen("cfetchconf", "w");
    fseek(config, 0, SEEK_SET);
    //sprintf(Settings[0], "USE_SAVED_ART=%d\'\n", stg);
    //fputs(Settings[0], config);
    fseek(config,0,SEEK_CUR);
    sprintf(Settings[0], "V_OFFSET=%d\'\n", voffset);
    fputs(Settings[0], config);
    sprintf(Settings[1], "H_OFFSET=%d\'\n", hoffset);
    fputs(Settings[1], config);
    fclose(config);
}
void args(int argc, char **argv){
    int a = 0;
    int b = 1;
    char *acceptedArgs[6] = {"-s", "-vo", "-h", "-ho", "-r", "IN"};
    if (argc > 3) {
        printf("cfetch: too many arguments. -h for help.");
        exit (0);
    }
    if (argc >= 2) {
        while (6 > a){
            //printf("%s\n",acceptedArgs[a]);
            b = strcmp(acceptedArgs[a], argv[1]);
                if (b == 0) {
                ++a;
                break;
                }
            a++;
        }
    }

    switch (a)
    {
        case noargs:
        drawoutp(argv, voffset, hoffset);
        break;
    
        case save:
        if (argc !=2) {
        saveart(argc, argv);
        drawoutp(argv, voffset, hoffset);
        } else {
        printf("cfetch: missing required arguments.\n");
        }
        break;
    
        case changv:
        if (argc != 2) {
        voffset = atoi(argv[2]);
        popconf();
        drawoutp(argv, voffset, hoffset);
        } else {
        printf("cfetch: missing required arguments.\n");
        }
        break;

        case help:
        printf("cfetch: cfetch -s (text file (chafa works best.)) [save ascii art.] -vo (num) [change vertical offset.]\n-ho (num) [change horizontal offset.] -r [restore config file.]\n");
        exit(0);
        break;
        
        case 4:
        if (argc != 2) {
        hoffset = atoi(argv[2]);
        popconf();
        drawoutp(argv, voffset, hoffset);
        } else {
        printf("cfetch: missing required arguments.\n");
        }
        break;

        case 5:
        popconf();
        printf("cfetch: restoring configuration file.\n");
        sleep(2);
        popconf();
        exit(0);
        break;

        default:
        printf("cfetch: Invalid arguments, -h for help.\n");
        exit(1);
    }
}
void prcsstring(const char *src, char *dst, char delimiter) {
    int i = 0, k = 0;
    while (src[i] != '\0') {
        if (src[i] == delimiter) {
            i++;
            while (src[i] != '\0' && src[i] != '\n') { // REMEMBER how to set this up, saves SO much time.
                dst[k++] = src[i++];
            }
            break;
        }
        i++;
    }
    dst[k] = '\0';
}
void getdigits(const char *src, char *dst) {
    int i = 0, k = 0;
    while (src[i] != '\n' && src[i] != '\0') {
        if (isdigit(src[i])) {
            dst[k++] = src[i];
        }
        i++;
    }
    dst[k] = '\0';  // ALWAYS use functions. I need to stop making redundant code.
}