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
int set;
int llength;
int voffset = 0;
int hoffset = 8;

void outpfetch(bool savest);
void saveart(int argc, char **argv);
void drawoutp(char **agrv, int voffset, int hoffset);
void popconf();
void args(int argc, char **argv);

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
    set = 0;

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
    char grepoutp[4][50]; // = malloc(100);
    char fmemcmd[40];
    FILE *mem = popen("grep \"MemTotal\" /proc/meminfo", "r");
    FILE *memu = popen("grep \"MemAvailable\" /proc/meminfo", "r");
    FILE *cpu = popen("grep -m 1 \"model name\" /proc/cpuinfo", "r");
    //FILE *ex = popen("grep \"MemFree\" /proc/meminfo", "r");
    fgets(grepoutp[0], 40, mem); 
    fgets(grepoutp[1], 40, memu);
    fgets(grepoutp[2], 40, cpu); 
    printf("Gpout0= %s",grepoutp[0]);
    printf("Gpout1= %s",grepoutp[1]);
    printf("Gpout1= %s\n",grepoutp[2]);
    pclose(mem);
    pclose(memu);
    pclose(cpu);
    printf("Grep outp size %lu\n", sizeof(grepoutp)+2);
    char (*memoryr)[60];
    printf("Mem alloc=%lu\n",sizeof(*memoryr)*4);
    memoryr = malloc(sizeof(*memoryr)*4); //Remember this.
    //*memoryr = (char*)malloc(sizeof(grepoutp)+1);

    int k = 0;
    while (grepoutp[0][i] != '\0') {
        if (isdigit(grepoutp[0][i]) != 0) {
            memoryr[0][k++] = grepoutp[0][i];
        }
        i++;
    }
    i = 0;
    k = 0;
    while (grepoutp[1][i] != '\0') {
        if (isdigit(grepoutp[1][i]) != 0) {
            memoryr[1][k++] = grepoutp[1][i];
        }
        i++;
    }
    i = 0;
    k = 0;
    while (1) {
        if (grepoutp[2][i] == ':') {
            i++;
            while (grepoutp[2][i] != '\0') {
            memoryr[2][k++] = grepoutp[2][i++];
            }
            break;
        }
        i++;
    }
    printf("CPU info %s\n", memoryr[2]);
    i = 0;
    //memoryr[0][++k] = '\0';
    //memoryr[1][++k] = '\0';
    memory = atoi(memoryr[0]);
    memoryfree = atoi(memoryr[1]);
    printf("memfree=%d",memoryfree);
    memory = memory/1024;
    memoryfree = memory - (memoryfree/1024);
    free(memoryr);
    char info[5][303];
    //char *syst = (char*)malloc(sizeof(char) * 100);
    //char *host = (char*)malloc(sizeof(char) * 100);
    //char *sess = (char*)malloc(sizeof(char) * 100);
    //char *opt = (char*)malloc(sizeof(char) * 100);
    //char *opt2  = (char*)malloc(sizeof(char) * 100);

    sprintf(info[0], "OS: %s %s %s\'", Linux.sysname, Linux.release, Linux.machine);
    sprintf(info[1], "Hostname: %s@%s\'", getenv("USER"), Linux.nodename );
    sprintf(info[2], "DE: %s\'", getenv("GDMSESSION"));
    sprintf(info[3], "Memory: %dmB / %dmB\'",memoryfree, memory);
    sprintf(info[4], "CPU:%s\'",memoryr[2]);
    
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
            switch (newlcnt-voffset) {
                case 1:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }   
                while ( info[0][i] != '\'') {
                putchar(info[0][i]);
                ++i;
                }
                break;

                case 2:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }
                while ( info[1][i] != '\'') {
                putchar(info[1][i]);
                ++i;
                }
                break;

                case 3:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }
                while ( info[2][i] != '\'') {
                putchar(info[2][i]);
                ++i;
                }
                break;

                case 4:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }
                while ( info[3][i] != '\'') {
                putchar(info[3][i]);
                ++i;
                }
                break;

                case 5:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }
                while ( info[4][i] != '\'') {
                putchar(info[4][i]);
                ++i;
                }
                break;
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
    //free(syst);
    //free(host);
    //free(sess);
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
        exit(0);
        break;

        default:
        printf("cfetch: Invalid arguments, -h for help.\n");
        exit(1);
    }
}