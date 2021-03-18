#pragma once
#include <inttypes.h>
#include "common.hh"

class cApplication{
    public:
        eApplicationID const id;
        cApplication(const eApplicationID id):id(id){}
};

