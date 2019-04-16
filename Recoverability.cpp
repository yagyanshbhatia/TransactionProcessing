#include "Transactions.cpp"




int main(){

	Transactions obj;

	obj.readInput();   // to read input file

	obj.PossibleChains(); //to find all possible chains

	/*Each chain represents collection of read operation on a dataitem that is written by a transaction before those read operations*/

	obj.isRecoverable();  // to check whether schedule is recoverable or not

	obj.isCascadeless(); // to check whether schedule is cascadeless or not 

	obj.isConflictSerialisable();

	obj.isDeadlockFree();

	return 0;

}