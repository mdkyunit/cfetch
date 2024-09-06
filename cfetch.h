#ifndef CFETCH_H
#define CFETCH_H
enum info {
    OS = 1,
    Host = 2,
    Sess = 3,
    Opt1 = 4,
    Opt2 = 5,
};
enum format {
    Line1 = 1,
    Line2 = 2,
    Line3 = 3,
    Line4 = 4,
    Line5 = 5,
    Line6 = 6,
};
extern char **Settings;
extern int voffset;
#endif