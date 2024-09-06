#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <cjson/cJSON.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "cfetch.h"
char chr;
int set;
bool usave;
bool tusave = FALSE;
int llength;
int voffset = 0;
int hoffset = 8;

void outpfetch(bool savest);
void saveart(int argc, char **argv);
void drawoutp(char **agrv, int voffset, int hoffset);
void popconf(int stg);
void args(int argc, char **argv);
//int vhoffset();

int main(int argc, char **argv) {


    char configf[4][20]; 
    //(char*)malloc(sizeof(char)*20);
    //ocupconf();
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
        popconf(usave);
        printf("cfetch: No config detected, populating...\n");
        usleep(1000000);
        config = fopen("cfetchconf", "r");
    }
    fscanf(config, "%s", configf[0]);
    fseek(config, 0, SEEK_CUR);
    fscanf(config, "%s", configf[1]);
    fseek(config, 0, SEEK_CUR);
    fscanf(config, "%s", configf[2]);
    fclose(config);
    //printf("%s %lu\n",configf, strlen(configf));
    for (int i = 0; i < strlen(configf[0]);++i) /* && configf[0][i] != '\''; ++i)*/ 
    {
        //printf("%c\n", configf[i]);
        switch (set = (configf[0][i])) {
            case '0':
            //printf("Save turned off.\n");
            usave=FALSE;
            break;
            case '1':
            //printf("Save turned on.\n");
            usave=TRUE;
            break;
        }
    }
    voffset = atoi(&configf[1][9]); //could have done this the whole time for config. hours spent...
    hoffset = atoi(&configf[2][9]); 
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
//if (argc == 3 && 0 == (strcmp(argv[1], "-s"))) {
    FILE* sart = fopen(argv[2], "r");
    FILE* svart = fopen("cfetchart", "w");
    while ((chr = fgetc(sart)) != EOF) {
        putc(chr, svart);
    }
    printf("cfetch: art saved successfully. Run 'cfetch' to display\n");
    fclose(svart);
    fclose(sart);
    usave = TRUE;
    popconf(usave);
    exit(0);
}

void drawoutp(char **argv, int voffset, int hoffset) {
    int i = 0; // used for loops
    struct utsname Linux;
    uname(&Linux);

    char *syst = (char*)malloc(sizeof(char) * 100);
    char *host = (char*)malloc(sizeof(char) * 100);
    char *sess = (char*)malloc(sizeof(char) * 100);
    //char *opt = (char*)malloc(sizeof(char) * 100);
    //char *opt2  = (char*)malloc(sizeof(char) * 100);

    sprintf(syst, "OS: %s %s %s", Linux.sysname, Linux.release, Linux.machine);
    sprintf(host, "Hostname: %s@%s", getenv("USER"), Linux.nodename );
    sprintf(sess, "DE: %s", getenv("GDMSESSION"));
    FILE *art;
    if (usave == TRUE) {
        art = fopen("cfetchart", "r");
    } else if (usave == FALSE) {
        art = fopen(argv[1], "r");
            if (art == NULL) {
            char s;
            printf("cfetch: config file is missing or damaged.\nwould you like to recover it? (y)/n\n");
            scanf("%1c", &s);
                if (s != 'y'){
                    exit(0);
                }
                else if (s == 'y') {
                    sleep(1);
                    printf("cfetch: If config has been tampered with inappropriately, possibility of stack smashing.\nIgnore it, i got this.\n");
                    usleep(2000000);
                    //system("clear")
                    usave = TRUE;
                    popconf(usave);
                    art = fopen("cfetchart", "r");
                }
            }
        }

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
                while ( syst[i] != '\0') {
                putchar(syst[i]);
                ++i;
                }
                break;

                case 2:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }
                while ( host[i] != '\0') {
                putchar(host[i]);
                ++i;
                }
                break;

                case 3:
                while (h < hoffset) {
                putchar(' ');
                ++h;
                }
                while ( sess[i] != '\0') {
                putchar(sess[i]);
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
    free(syst);
    free(host);
    free(sess);
}
void popconf(int stg) {
    char Settings[3][20];
    FILE* config = fopen("cfetchconf", "w");
    fseek(config, 0, SEEK_SET);
    sprintf(Settings[0], "USE_SAVED_ART=%d\'\n", stg);
    fputs(Settings[0], config);
    fseek(config,0,SEEK_CUR);
    sprintf(Settings[1], "V_OFFSET=%d\'\n", voffset);
    fputs(Settings[1], config);
    sprintf(Settings[2], "H_OFFSET=%d\'\n", hoffset);
    fputs(Settings[2], config);
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
        popconf(usave);
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
        popconf(usave);
        drawoutp(argv, voffset, hoffset);
        } else {
        printf("cfetch: missing required arguments.\n");
        }
        break;

        case 5:
        popconf(usave);
        printf("cfetch: restoring configuration file.\n");
        sleep(2);
        exit(0);
        break;
        default:
        printf("cfetch: Invalid arguments, -h for help.\n");
        exit(1);
    }
}