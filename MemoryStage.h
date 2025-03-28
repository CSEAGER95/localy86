#ifndef MEMORYSTAGE_H
#define MEMORYSTAGE_H

#include "Stage.h"
#include "PipeReg.h"

class MemoryStage : public Stage {
public:
    bool doClockLow(PipeReg ** pregs, Stage ** stages);
    void doClockHigh(PipeReg ** pregs);

      void setWInput(W *wreg, uint64_t stat, uint64_t icode,
                              uint64_t valE, uint64_t valM, uint64_t dstE, uint64_t dstM);
};

#endif