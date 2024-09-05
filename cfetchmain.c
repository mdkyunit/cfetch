//#include <ncurses.h>
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
void outpfetch(bool savest);
void saveart(int argc, char **argv);
void drawoutp(char **agrv);

int main(int argc, char **argv) {


    char *configf = (char*)malloc(sizeof(char)*20);
    int option;
    FILE* config = fopen("cfetchconf", "r");
    fscanf(config, "%s", configf);
    fclose(config);
    //printf("%s %lu\n",configf, strlen(configf));
    for (int i = 0; i < strlen(configf); ++i) {
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

    saveart(argc, argv);
    drawoutp(argv);

    //printf("Newlines Detected %d\n", newlcnt);
    //printf("Characters Detected %d\n", chrnt);
    //printf("Characters per one line %d\n", chrprln);
    //printf("OS: %s %s %s\n", Linux.sysname, Linux.release, Linux.machine);
    //printf("%d\n", llength);

    return 0;
}


void saveart(int argc, char **argv) {
if (argc == 3 && 0 == (strcmp(argv[1], "-s"))) {
    FILE* sart = fopen(argv[2], "r");
    FILE* svart = fopen("cfetchart", "w");
    while ((chr = fgetc(sart)) != EOF) {
        putc(chr, svart);
    }
    printf("Art saved successfully. Run 'cfetch' to display\n");
    fclose(svart);
    fclose(sart);
    FILE* config = fopen("cfetchconf", "w");
    fputs("USE_SAVED_ART=1", config);
    fclose(config);
    usave = TRUE;
}   else if (argc == 3 && 0 != (strcmp(argv[1], "-s"))) {
    printf("unrecognized argument '%s'\n", argv[1]);
    exit (1);
    }
}

void drawoutp(char **argv) {
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
            printf("no input file detected while use save mode is 'off'.\nwould you like to turn that on and try again? y/(n)");
            scanf("%1c", &s);
            if (s == 'y') {
                usleep(2000000);
                //system("clear")
                usave = TRUE;
                FILE* config = fopen("cfetchconf", "w");
                fputs("USE_SAVED_ART=1", config);
                fclose(config);
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
            switch (newlcnt) {
                case OS:
                while ( syst[i] != '\0') {
                putchar(syst[i]);
                ++i;
                }
                break;

                case Host:
                while ( host[i] != '\0') {
                putchar(host[i]);
                ++i;
                }
                break;

                case Sess:
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