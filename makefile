CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0

# Updated object files list to include all missing stage and pipeline register files.
OBJ = yess.o Memory.o Tools.o RegisterFile.o ConditionCodes.o Loader.o \
      FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o \
      Simulate.o F.o D.o E.o M.o W.o PipeReg.o PipeRegField.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)
	$(CC) -g -Wall -std=c++11 -O0 -o yess $(OBJ)

# Updated dependencies to include the new stage header files.
yess.o: Memory.h RegisterFile.h ConditionCodes.h Loader.h \
         FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h Stage.h

Memory.o: Memory.h Tools.h
RegisterFile.o: RegisterFile.h Tools.h
ConditionCodes.o: ConditionCodes.h Tools.h
Loader.o: Loader.h Memory.h
Tools.o: Tools.h
FetchStage.o: FetchStage.h Stage.h
DecodeStage.o: DecodeStage.h Stage.h
ExecuteStage.o: ExecuteStage.h Stage.h
MemoryStage.o: MemoryStage.h Stage.h
WritebackStage.o: WritebackStage.h Stage.h
PipeReg.o: PipeReg.h

clean:
	rm -f $(OBJ) yess

run:
	make clean
	make yess
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *
