// filepath: /u/css/carsonma/cs3481-yess-group-x/DecodeStage.h
#ifndef DECODESTAGE_H
#define DECODESTAGE_H

#include "Stage.h"
#include "PipeReg.h"

class DecodeStage : public Stage {
public:
    bool doClockLow(PipeReg ** pregs, Stage ** stages);
    void doClockHigh(PipeReg ** pregs);

    void setEInput(E *ereg, uint64_t stat, uint64_t icode,
                            uint64_t ifun, uint64_t valC, uint64_t valA,
                            uint64_t valB, uint64_t dstE, uint64_t dstM,
                            uint64_t srcA, uint64_t srcB);
};

#endif