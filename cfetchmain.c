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
int voffset = 2;

void outpfetch(bool savest);
void saveart(int argc, char **argv);
void drawoutp(char **agrv, int voffset);
void popconf(int stg);
void args(int argc, char **argv);

int main(int argc, char **argv) {


    char *configf = (char*)malloc(sizeof(char)*20);
    //ocupconf();
    FILE * config = fopen("cfetchconf", "r");
    if (config == NULL) {
        popconf(usave);
        printf("cfetch: No config detected, populating...\n");
        usleep(1000000);
        config = fopen("cfetchconf", "r");
    }
    fscanf(config, "%s", configf);
    fclose(config);
    //printf("%s %lu\n",configf, strlen(configf));
    for (int i = 0; i < strlen(configf) && configf[i] != '\''; ++i) {
        //printf("%c\n", configf[i]);
        switch (set = (configf[i])) {
            case '0':
            //printf("Save turned off.");
            usave=FALSE;
            break;
            case '1':
            //printf("Save turned on.");
            usave=TRUE;
            break;
        }
    }
    free(configf);

    //printf("Usave=%d\n",usave);
    args(argc, argv);
    return 0;
    saveart(argc, argv);
    drawoutp(argv, voffset);

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

void drawoutp(char **argv, int voffset) {
    int i = 0; // used for loops
    struct utsname Linux;
    uname(&Linux);

    char *syst = (char*)malloc(sizeof(char) * 100);
    char *host = (char*)malloc(sizeof(char) * 100);
    char *sess = (char*)malloc(sizeof(char) * 100);
    //char *opt = (char*)malloc(sizeof(char) * 100);
    //char *opt2  = (char*)malloc(sizeof(char) * 100);

    sprintf(syst, "        OS: %s %s %s", Linux.sysname, Linux.release, Linux.machine);
    sprintf(host, "        Hostname: %s@%s", getenv("USER"), Linux.nodename );
    sprintf(sess, "        DE: %s", getenv("GDMSESSION"));
    FILE *art;
    if (usave == TRUE) {
        art = fopen("cfetchart", "r");
    } else if (usave == FALSE) {
        art = fopen(argv[1], "r");
            if (argv[1] == NULL) {
            char s;
            printf("cfetch: no input file detected while use save mode is 'off'.\nwould you like to turn that on and try again? y/(n)");
            scanf("%1c", &s);
                if (s == 'y') {
                    usleep(2000000);
                    //system("clear")
                    usave = TRUE;
                    popconf(usave);
                    art = fopen("cfetchart", "r");
                } else {
                    exit(0);
                }
            }
        }

    int newlcnt = 0; // Newline Count
    int chrnt = 0; // Character count
    int chrprln = 0; // Chr Count/Newline Count

    while ((chr = fgetc(art)) != EOF) {
        if (chr == '\n'){
            newlcnt++;
            llength = ftell(art);
            switch (newlcnt-voffset) {
                case 1:
                while ( syst[i] != '\0') {
                putchar(syst[i]);
                ++i;
                }
                break;

                case 2:
                while ( host[i] != '\0') {
                putchar(host[i]);
                ++i;
                }
                break;

                case 3:
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
    chrprln = chrnt/newlcnt;
    puts("\n");
    fclose(art);
    free(syst);
    free(host);
    free(sess);
}
void popconf(int stg) {
    char Settings[7][40];
    FILE* config = fopen("cfetchconf", "w");
    fseek(config, 0, SEEK_SET);
    sprintf(Settings[0], "USE_SAVED_ART=%d\'\n", stg);
    fputs(Settings[0], config);
    fseek(config,0,SEEK_CUR);
    fputs("ADD_CONF'", config);
    fclose(config);
}
void args(int argc, char **argv){
    int a = 0;
    int b = 1;
    char *acceptedArgs[4] = {"-s", "-v", "-h", "IN"};
    if (argc > 3) {
        printf("cfetch: too many arguments. -h for help.");
        exit (0);
    }
    if (argc >= 2) {
        while (4 > a){
            //printf("%s\n",acceptedArgs[a]);
            b = strcmp(acceptedArgs[a], argv[1]);
                if (b == 0) {
                ++a;
                break;
                }
            a++;
    }
    }
   //printf("%d\n", a);
    //exit (0);
    switch (a) 
    {
        case 0:
        drawoutp(argv, voffset);
        break;
    
        case 1:
        saveart(argc, argv);
        drawoutp(argv, voffset);
        break;
    
        case 2:
        break;

        case 3:
        printf("cfetch: cfetch -s [save ascii art.] -v [change vertical offset.]\n");
        exit(0);
        break;
    
        default:
        printf("cfetch: Invalid arguments, -h for help.\n");
        exit(1);
    }
}