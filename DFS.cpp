#include <iostream>
    
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <queue>
#include <sstream>
#include <omp.h>
#include <fstream>
#include <vector>
#include <chrono>
using namespace std;
vector<int> output; 
vector<bool> visited;
vector < vector <int> >graph;
int global = 0;
int *marked = NULL;
int threads_num;
int length = 20;

void s_init () {
	int res, nodesNum, node1, node2;

	FILE *fin = fopen ("graph.in", "r");


	fscanf (fin, "%d", &nodesNum);
	
	graph.resize (nodesNum + 1);
	marked = new int [nodesNum + 1]();
	
	while (fscanf (fin, "%d%d", &node1, &node2) != EOF) {
		bool checkNode = node1 < 0 || node2 < 0 || node1 > nodesNum || node2 > nodesNum;
		
			
		graph[node1].push_back (node2);
		graph[node2].push_back (node1);
	}

	res = fclose (fin);
}


//Display the graph
void display () {
	for (unsigned int i = 0; i < graph.size(); ++i) {
		if (graph[i].size() != 0) {
			cout<< i<<"->";
			for (unsigned int j = 0; j < graph[i].size(); ++j) {
				cout<< graph[i][j]<<" ";
			}
			cout<<"\n";
		}
	}
}

void s_dfs(int v){
    visited[v] = true;
    for (int i = 0; i < graph[v].size(); ++i) {
        if (!visited[graph[v][i]]) {
            s_dfs(graph[v][i]);
        }
    }
}

void p_dfs(int v){
    #pragma omp critical
    visited[v] = true;

    #pragma omp critical
    {
        for (int i = 0; i < graph[v].size(); ++i) {
            if (!visited[graph[v][i]]) {
                #pragma omp task
                p_dfs(graph[v][i]);
            }
        }
    }
}
int main(int argc, char const *argv[])
{	
	
	threads_num = atoi (argv[1]); 
    	

	//initialize the graph for serial 
	s_init();
	// Display adjacencies 
	cout<<"Created the following graph:\n";
	display ();

  for (int i = 0; i < length; i++)
    {
        visited.push_back(false);
    }

	/* Perform Serial Breadth First Search */
	auto start = chrono::steady_clock::now();
	s_dfs(0);
	auto end = chrono::steady_clock::now();
	auto dur = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout<< "\nSerial DFS: "<<dur<<"ms\n\n";
  graph.clear();


  for (int i = 0; i < length; i++)
  {
    visited.push_back(false);
  }
  
	//initialize the graph for parallel 
	s_init();
	// Display adjacencies 
	cout<<"Created the following graph:\n";
	display ();


	/* Perform Parallel Breadth First Search */
	start = chrono::steady_clock::now();
#pragma omp parallel
  {
#pragma omp single
    {
      p_dfs(0);
    }
  }
  end = chrono::steady_clock::now();
  dur = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout<<"Parallel DFS: "<<dur << "ms"<<endl;


	return 0;
}
