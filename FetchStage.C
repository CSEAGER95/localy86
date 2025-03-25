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
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"   // Added to provide INOP, IJXX, IRET, etc.
#include "Memory.h"   // Add this include if not already there


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   // Select current PC from F, M, W registers.
   uint64_t f_pc = selectPC(freg, mreg, wreg);

   // Fetch instruction byte from memory instead of hardcoding.
   Memory *mem = Memory::getInstance();
   bool memError;
   uint8_t byte = mem->getByte(f_pc, memError);
   uint64_t instruction = byte;  

   // Extract icode and ifun.
   uint64_t f_icode = instruction >> 4;
   uint64_t f_ifun  = instruction & 0xF;

   bool regids = need_regids(f_icode);
   bool valC_needed = need_valC(f_icode);

   // Compute next sequential address.
   uint64_t f_valP = PCincrement(f_pc, regids, valC_needed);

   // For now, no constant is fetched.
   uint64_t f_valC = 0;

   // Predict next PC.
   uint64_t f_predPC = predictPC(f_icode, f_valC, f_valP);

   // Set F register's predPC.
   freg->getpredPC()->setInput(f_predPC);

   // Set D register inputs (using defaults for rA, rB).
   setDInput(dreg, SAOK, f_icode, f_ifun, RNONE, RNONE, f_valC, f_valP);

   return false;
}

uint64_t FetchStage::selectPC(F *freg, M *mreg, W *wreg)
{
   // If M stage is executing a jump that failed, use its valA.
   if (mreg->geticode()->getOutput() == IJXX && mreg->getCnd()->getOutput() == 0)
      return mreg->getvalA()->getOutput();
   // Otherwise, if W stage is executing a return, use its valM.
   else if (wreg->geticode()->getOutput() == IRET)
      return wreg->getvalM()->getOutput();
   // Otherwise, use the predicted PC from F.
   else
      return freg->getpredPC()->getOutput();
}

bool FetchStage::need_regids(uint64_t f_icode)
{
   return (f_icode == IRRMOVQ || f_icode == IOPQ ||
           f_icode == IPUSHQ   || f_icode == IPOPQ ||
           f_icode == IIRMOVQ  || f_icode == IRMMOVQ ||
           f_icode == IMRMOVQ);
}

bool FetchStage::need_valC(uint64_t f_icode)
{
   return (f_icode == IIRMOVQ || f_icode == IRMMOVQ ||
           f_icode == IMRMOVQ || f_icode == IJXX ||
           f_icode == ICALL);
}

uint64_t FetchStage::PCincrement(uint64_t f_pc, bool need_regids_flag, bool need_valC_flag)
{
   return f_pc + 1 + (need_regids_flag ? 1 : 0) + (need_valC_flag ? 8 : 0);
}

uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{
   return (f_icode == IJXX || f_icode == ICALL) ? f_valC : f_valP;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */


void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   freg->getpredPC()->normal();
   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}

