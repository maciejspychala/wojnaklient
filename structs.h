//
// Created by maciej on 11.02.16.
//

#ifndef WOJNA_STRUCTS_H
#define WOJNA_STRUCTS_H


struct Init{
    long mtype;
    int nextMsg;
};

struct Data {
    long mtype;
    int light;
    int heavy;
    int cavalry;
    int workers;
    int points;
    int resources;
    char info[120];
    char end;
};

struct Build {
    long mtype;
    int light;
    int heavy;
    int cavalry;
    int workers;
};

struct Attack {
    long mtype;
    int light;
    int heavy;
    int cavalry;
};

struct Alive {
    long mtype;
};


#endif //WOJNA_STRUCTS_H
