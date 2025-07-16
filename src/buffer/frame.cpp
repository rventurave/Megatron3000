#include "../include/buffer/frame.h"

// Objetivo: Constructor de frame con ID único
// Input: int id
// Output: Frame inicializado
//@autor Ronald Ventura
Frame::Frame(int id)
    : frameID(id), pageID("-"), operation('L'),dirty(0), time(0), pin(0), count(0), reference(0) {}