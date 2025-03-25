// class to perform the combinational logic of
// the Fetch stage
class FetchStage : public Stage
{
   private:
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP);
      // New helper prototypes:
      uint64_t selectPC(F *freg, M *mreg, W *wreg);
      bool need_regids(uint64_t f_icode);
      bool need_valC(uint64_t f_icode);
      uint64_t PCincrement(uint64_t f_pc, bool need_regids, bool need_valC);
      uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
   public:
      bool doClockLow(PipeReg **pregs, Stage **stages);
      void doClockHigh(PipeReg **pregs);
};
