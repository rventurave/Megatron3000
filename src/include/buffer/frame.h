#ifndef FRAME_H
#define FRAME_H
#include <string>
#include <queue>
struct Frame {
    int frameID;
    std::string pageID;
    char operation; 
    int dirty;      
    int time;       
    int pin;        
    int count;
    int reference;    
    std::queue<char> opsQueue;
    Frame(int id);
};

#endif
