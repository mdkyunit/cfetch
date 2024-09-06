#ifndef CFETCH_H
#define CFETCH_H
enum info {
    OS = 1,
    Host = 2,
    Sess = 3,
    Opt1 = 4,
    Opt2 = 5,
};
enum args {
    noargs = 0,
    save = 1,
    changv = 2,
    help = 3,
    changh = 4,
    dpv = 5,
    Line6 = 6,
};
extern char **Settings;
extern int voffset;
extern int hoffset;
#endif