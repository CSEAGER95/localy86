#ifndef WRITEBACKSTAGE_H
#define WRITEBACKSTAGE_H

#include "Stage.h"
#include "PipeReg.h"

class WritebackStage : public Stage {
public:
    bool doClockLow(PipeReg ** pregs, Stage ** stages);
    void doClockHigh(PipeReg ** pregs);
};

#endif