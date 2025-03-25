#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "F.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "DecodeStage.h"
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
bool DecodeStage::doClockLow(PipeReg **pregs, Stage **stages)
{
    // Grab values from the D register and set defaults for others.
    D * dreg = (D *) pregs[DREG];
    E * ereg = (E *) pregs[EREG];

    uint64_t stat  = dreg->getstat()->getOutput();
    uint64_t icode = dreg->geticode()->getOutput();
    uint64_t ifun  = dreg->getifun()->getOutput();
    uint64_t valC  = dreg->getvalC()->getOutput();
    uint64_t valA  = 0;         // default for fields not in D
    uint64_t valB  = 0;
    uint64_t dstE  = RNONE;      // use RNONE for register destinations
    uint64_t dstM  = RNONE;
    uint64_t srcA  = RNONE;
    uint64_t srcB  = RNONE;
    setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
    return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void DecodeStage::doClockHigh(PipeReg **pregs)
{
    E * ereg = (E *) pregs[EREG];
    ereg->getstat()->normal();
    ereg->geticode()->normal();
    ereg->getifun()->normal();
    ereg->getvalC()->normal();
    ereg->getvalA()->normal();
    ereg->getvalB()->normal();
    ereg->getdstE()->normal();
    ereg->getdstM()->normal();
    ereg->getsrcA()->normal();
    ereg->getsrcB()->normal();
}

void DecodeStage::setEInput(E *ereg, uint64_t stat, uint64_t icode,
                            uint64_t ifun, uint64_t valC, uint64_t valA,
                            uint64_t valB, uint64_t dstE, uint64_t dstM,
                            uint64_t srcA, uint64_t srcB)
{
    ereg->getstat()->setInput(stat);
    ereg->geticode()->setInput(icode);
    ereg->getifun()->setInput(ifun);
    ereg->getvalC()->setInput(valC);
    ereg->getvalA()->setInput(valA);
    ereg->getvalB()->setInput(valB);
    ereg->getdstE()->setInput(dstE);
    ereg->getdstM()->setInput(dstM);
    ereg->getsrcA()->setInput(srcA);
    ereg->getsrcB()->setInput(srcB);
}

/*
DecodeStage doClockLow:  Add a setEInput method (described below) to the DecodeStage class.
This method will be called at the end of the doClockLow method to set the input values for the E pipeline register.
(See figures in the textbook and on the asulearn site to find out what these fields are.)
Some of the values to be stored in the E register are obtained directly from the D register.
(See how FetchStage::doClockLow grabs the predPC value from the F register.
You'll need similar statements in DecodeStage::doClockLow to grab fields from the D register.)
Some of the values to be stored in the E register can't be obtained from the D register (for example, valA and dstE).
Those values should be set to either 0 or RNONE (RNONE if the value is a register, such as dstE).
Thus, your doClockLow method will consist of statements to grab values from the D register that should be stored in the E register and a call to the setEinput method.

DecodeStage setEinput: Model this method after the setDInput method in the FetchStage.
Note that it has a parameter for each pipe register field within the E pipeline register and will have a call to the setInput method for each of these.

DecodeStage doClockHigh: This method will perform the behavior of the rising clock edge by calling the normal method on each of the pipe register fields within the E pipeline register.
Note that the normal method will set the state of the register to the input.
(See the PipeRegField class for more details.)

You'll implement the same methods in ExecuteStage, MemoryStage, and WritebackStage.
*/