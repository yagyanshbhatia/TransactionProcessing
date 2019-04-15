#include "Transactions.hpp"


void Transactions::readInput(){
	/*
        -->this function read the given input file and store all the transactions details in d.s. 'transactions'    
	*/
	int Tid, DataItem=0;
    char Opr;
    while(cin>>Tid){
        cin>>Opr;
        if(Opr!='C')
            cin>>DataItem;
        else{
            commit[Tid]=transaction.size();
        }
        // Not covering if input format is wrong.
        transaction.push_back({Tid,{Opr,DataItem}});
    }
}


void Transactions::PossibleChains(){
	/*
	    --> this method find all the possible chains and store it in d.s. 'chain'
	    --> each chain represents collection of read operations on a dataitem that is written by a transaction before those read operations
        
	*/
	vector<int> temp;
	pair<int,int> firstRead;
	for(int i=0; i<transaction.size(); i++){
        if(transaction[i].second.first!='W')
            continue;

        int DataItem=transaction[i].second.second;

        for(int j=i+1; j<transaction.size(); j++){
            if(transaction[j].second.second!=DataItem || transaction[i].first == transaction[j].first)
                continue;
            if(transaction[j].second.first=='R'){
            	if(temp.size()==0)
            		firstRead={transaction[j].first,j};
            	temp.push_back(transaction[j].first);
            }
            if(transaction[j].second.first=='W')
                break;
        }
        if(temp.size()){
            chain.push_back({transaction[i].first,temp});
            firstReadArray.push_back({transaction[i].first,{firstRead}});
        }
        temp.clear();
    }
}


void Transactions::isRecoverable(){
	/*
         --> this method checks whether the given schedule is recoverable or not
	*/
	for(int i=0; i<chain.size(); i++){
        int x = commit[chain[i].first];
        for(int j=0; j<chain[i].second.size(); j++){
            if(x>=commit[chain[i].second[j]]){
            	cout<<"No - not recoverable"<<endl;
            	cout<<"Transaction "<<chain[i].second[j]<<" is reading DataItem written by Transaction "<<chain[i].first<<endl;
            	cout<<"And Transaction "<<chain[i].first<<" commits after Transaction "<<chain[i].second[j]<<endl;
            	return;
            }
        }
    }
    cout<<"Yes - recoverable"<<endl;
}


void Transactions::isCascadeless(){
	/*
         --> this method checks whether the given schedule is cascadeless or not
	*/
	for(int i=0; i<firstReadArray.size(); i++){
        int x = commit[firstReadArray[i].first];
        if(x>=firstReadArray[i].second.second){
        	cout<<"No - not cascadeless"<<endl;
        	cout<<"Transaction "<<firstReadArray[i].second.first<<" is reading DataItem written by Transaction "<<firstReadArray[i].first<<endl;
        	cout<<"And Transaction "<<firstReadArray[i].first<<" commits after reading by Transaction "<<firstReadArray[i].second.first<<endl;
        	return;
        }
    }
    cout<<"Yes - cascadeless"<<endl;
}

int dfs_utility(vector<set<int>> v, int * visited, int i){
    visited[i] = 1;
    //do the actual dfs
    bool x = 1;
    for(auto f: v[i]){
        if(visited[f] == 1) return 0; // return 0 when found a cycle
        x = dfs_utility(v, visited, f);
    }
    visited[i] = 2;
    return x;
}

int dfs(vector<set<int>> v){
    int n = v.size();
    int visited[n] = {0};
    bool no_cycle = 1;
    for (int i = 0; i < n; ++i)
    {
        if(visited[i] == 0){
            // 0 = white, 1 = grey, 2 = black
            no_cycle = dfs_utility(v, visited, i);
            if(!no_cycle) break;
        }
    }
    return no_cycle;
}

void Transactions::isConflictSerialisable(){
    set<int> nodes;
    for (int i = 0; i < transaction.size(); ++i)
        nodes.insert(transaction[i].first);

    int n = nodes.size();
    vector<set<int>> v(n+1);
    for (int i = 0; i < transaction.size(); ++i){
        if(transaction[i].second.first == 'C') continue;
            for (int j = i+1; j < transaction.size(); ++j){
                if(transaction[j].second.first == 'C') continue;

                if(transaction[j].first != transaction[i].first && transaction[j].second.second == transaction[i].second.second){
                    if(transaction[i].second.first == 'R' && transaction[j].second.first == 'R') continue;
                    int p = transaction[i].first;
                    int q = transaction[j].first;
                    v[p].insert(q);
                }
            }
    }
    // checck for cycle dfs
    int no_cycle = dfs(v);
    if(no_cycle) cout<<"conflict serializable"<<endl;
    else cout<<"Not conflict serializable"<<endl;
    for (int i = 0; i < v.size(); ++i)
    {   
        cout<<i<<" : ";
        for(auto f : v[i]){
            cout<<f<<" ";
        }
        cout<<endl;
    }
}