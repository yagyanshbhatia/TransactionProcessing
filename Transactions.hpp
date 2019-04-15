#include <bits/stdc++.h>

using namespace std;


/*this class contains declarations of all the data structures that are used to perform the given tasks*/

class Transactions{


public:
	void readInput();   // to read the given input file
	void PossibleChains();  // to find all possible chains
	void isRecoverable();  // to check whether schedule is recoverable or not
	void isCascadeless();  // to check whether schedule is cascadeless or not
	void isConflictSerialisable(); 
	void isDeadlockFree();


private:
	vector<pair<int, pair<char,int> > > transaction; 
	// it stores all the trasaction details
    unordered_map<int,int> commit;
    // it stores the position of commit operation of a trasaction 
    vector<pair<int,vector<int> > > chain;
    // to store all the possible chains
    vector< pair<int,pair<int,int> > > firstReadArray; 
    // it represents a vector where each elements is a tuple containing tid of write operation, position of first read in a chain and its tid  
};