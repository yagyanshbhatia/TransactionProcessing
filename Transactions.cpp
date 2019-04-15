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


int dfs_utility_deadlock(map<int, set<int>> v, map<int,int> &visited, int i){
    visited[i] = 1;
    //do the actual dfs
    bool x = 1;
    for(auto f: v[i]){
        if(visited[f] == 1) return 0; // return 0 when found a cycle
        x = dfs_utility_deadlock(v, visited, f);
    }
    visited[i] = 2;
    return x;
}

int dfs_deadlock(map<int, set<int>> v){
    int n = v.size();

    map<int,int> visited;
    for(auto f: v) visited[f.first] = 0;

    bool no_cycle = 1;
    for (auto f : visited)
    {
        if(f.second == 0){
            // 0 = white, 1 = grey, 2 = black
            no_cycle = dfs_utility_deadlock(v, visited, f.first);
            if(!no_cycle) break;
        }
    }
    return no_cycle;
}

void freeResources(map<int,set<int>> &graph, map<int,int> &lock, int id){
    //all locks aquired by id, set free
    set<int> empty;
    for(auto f: lock){
        if(f.second == id) {
            lock[f.first] = -1;
            //free adjacency list and allocate next of that resource in graph
            int resource = f.first;
            graph[resource] = empty;
            cout<<"Freed resource "<<resource;
            //traverse graph and allocate to the first node that wants the freed resource. 
            for(auto g: graph){
                if(g.second.find(resource) != g.second.end()){
                    lock[resource] = g.first;
                    g.second.erase(resource);
                    graph[resource].insert(g.first);
                    cout<<" and given to "<<g.first;
                    break;  
                }
            }
            cout<<endl;
        }
    }
    //If the transaction is being committed, it must not have any request edges. So just delete it from the graph.
    graph[id] = empty;
}

void Transactions::isDeadlockFree(){
    //make a ds to store which resource is occupied by whom (or none)
    map<int,int> lock;
    for (int i = 0; i < transaction.size(); ++i)
        lock[transaction[i].second.second] = -1; //initialise all of them as free. 

    map<int,set<int>> graph;
    set<int> empty;
    for (int i = 0; i < transaction.size(); ++i){
        graph[transaction[i].first] = empty;
        graph[transaction[i].second.second] = empty;
    }

    //for all entries in transaction, add an edge in the resource allocation graph and check at this stage if a cycle is found
    //and realease all the resource occupied by a transaction when you encounter its commit.  
    for (int i = 0; i < transaction.size(); ++i)
    {
        char opt = transaction[i].second.first;
        int resource = transaction[i].second.second;
        int id = transaction[i].first;

        if(opt == 'C'){
            //free all the resources, change graph AND lock.
            freeResources(graph, lock, id);
            continue;
        }

        if(lock[resource] == id)continue;

        //add the required edge (allocation / request) in the graph
        if(lock[resource] == -1){
            //resource was available, so allocate if opt is W
            if(opt == 'W'){
                lock[resource] = id;
                graph[resource].insert(id);
            }
        }else{
            //add a request edge
            graph[id].insert(resource);
        }
        //check for cycle, if found, deadlock
        if(!dfs_deadlock(graph)){
            cout<<"deadlock found !"<<endl;
            return ;
        }
    }
    cout<<"No deadlock"<<endl;
    return;
}