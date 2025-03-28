#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    M * mreg = (M *) pregs[MREG];
    W * wreg = (W *) pregs[WREG];
    
    uint64_t stat  = mreg->getstat()->getOutput();
    uint64_t icode = mreg->geticode()->getOutput();
    uint64_t valE  = mreg->getvalE()->getOutput();
    // Assume M->getvalA() holds the memory-related value (valM)
    uint64_t valM  = mreg->getvalA()->getOutput();
    uint64_t dstE  = mreg->getdstE()->getOutput();
    uint64_t dstM  = mreg->getdstM()->getOutput();
    setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
    return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void MemoryStage::doClockHigh(PipeReg ** pregs)
{
    W * wreg = (W *) pregs[WREG];
    wreg->getstat()->normal();
    wreg->geticode()->normal();
    wreg->getvalE()->normal();
    wreg->getvalM()->normal();
    wreg->getdstE()->normal();
    wreg->getdstM()->normal();
}

void MemoryStage::setWInput(W *wreg, uint64_t stat, uint64_t icode,
                           uint64_t valE, uint64_t valM, uint64_t dstE, uint64_t dstM)
{
   wreg->geticode()->setInput(icode);
   wreg->getstat()->setInput(stat);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}