/**
g++ cc.cpp -o cc -fcilkplus -lcilkrts -std=c++11
./cc < test.txt
*/
#include <cilk/cilk.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <chrono>

using namespace std;

enum class Group {
  CHILD,
  PARENT
};

typedef int Vertex;

struct Edge {
  int u;
  int v;
};

void prefixSum(vector<int> &S, vector<int> &res){
  int n = S.size();
  vector<int> Sstar(n/2);
  vector<int> Sout(n/2);

  if(n == 1){
    res[0] = S[0];
    return;
  }

  cilk_for(int i = 0; i < n/2; i++){
    Sstar[i] = S[2*i] + S[2*i + 1];
  }

  prefixSum(Sstar, Sout);

  cilk_for(int i = 0; i < n; i++){
    if(i == 0){
      res[i] = S[i];
    }else if(i%2 != 0){
      res[i] = Sout[i/2];
    }else{
      res[i] = Sout[(i-1)/2] + S[i];
    }
  }
}

Group randomGroup(){
  return ((double)rand()/RAND_MAX > 0.5) ? Group::PARENT : Group::CHILD;
}

void randomizedConectedComponents(vector<Vertex> &L, vector<Vertex> &V, vector<Edge> &E){
  if (E.size() == 0) return;
  int n = V.size();
  int m = E.size();
  vector<Vertex> tmpS(m);

  Group *C = new Group[2*n+1];
  cilk_for(int i = 0; i < n; i++){
    C[V[i]] = randomGroup();
  }

  cilk_for(int i = 0; i < m; i++){
    if (C[E[i].u] == Group::CHILD && C[E[i].v] == Group::PARENT){
      L[E[i].u] = L[E[i].v]; //filho aponta pro pai
    }
  }

  cilk_for(int i = 0; i < m; i++){
    if ( L[E[i].u] != L[E[i].v] ) tmpS[i] = 1; else tmpS[i] = 0;
  }

  vector<Vertex> S(m);
  prefixSum(tmpS, S);
  tmpS.clear();
  vector<Edge> F(S[m-1]);

  cilk_for(int i = 0; i < m; i++){
    if(L[E[i].u] != L[E[i].v]){
      F[S[i]-1].u = L[E[i].u];
      F[S[i]-1].v = L[E[i].v];
    }
  }

  S.clear();
  delete[] C;

  randomizedConectedComponents(L, V, F);

  cilk_for(int i = 0; i < m; i++){
    if(E[i].v == L[E[i].u]) L[E[i].u] = L[E[i].v];
  }
}

void printCC(vector<int> &L){
  map < int, vector<int> > CC;
  vector< int > result;
  for(int k = 1; k < L.size(); k++){
    CC[L[k]].push_back(k);
  }

  /*for(map<int, vector<int> >::iterator iter = CC.begin(); iter != CC.end(); ++iter){
    vector<int> tmp = iter->second;
    cout << "CC: ";
    for (int i = 0; i < tmp.size(); i++){
      cout << tmp[i] <<  " ";
    }
    cout <<  endl;
  }*/
  cout << "#Found CC: " << CC.size() << endl;
}

int main(){
  int n, m;
  scanf("%d %d", &n, &m);

  vector<Vertex> V(n);
  vector<Edge>   E(m);
  vector<Vertex> L(2*n+1);

  cilk_for(int i = 0; i < n; i++){
    V[i]   = i+1;
    L[i+1] = i+1;
  }

  for(int i = 0; i < m; i++){
    scanf("%d %d", &(E[i].u), &(E[i].v));
  }

  auto start = std::chrono::high_resolution_clock::now();
  randomizedConectedComponents(L, V, E);
  auto finish = std::chrono::high_resolution_clock::now();
  
  printCC(L);
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " s\n";
  return EXIT_SUCCESS;
}
