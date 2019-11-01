#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>
#include <list>
#include "clique.h"

using namespace std;


bool Exists(list<int> &set, int n)
{
   list<int>::iterator it;
   int u;
   for(it = set.begin(); it != set.end(); it++)
   {
      u = *it;
      if(u==n)
         return true;
   }
   return false;
}

int get_num_neighbourg_in_clique(list<int> &N, list<int> &clique){
   list<int>::iterator it;
   int numN=0;

   for(it = N.begin(); it != N.end(); it++)
   {
      int u = *it;
      if(Exists(clique, u))
         numN++;
   }
   
   return numN;
}

int select_vertex(list<int> *edge, list<int> &clique, int *process, double **residual, int cor){
   list<int>::iterator it;
   double max =0;
   int vertex = -1;

   for(it = clique.begin(); it != clique.end(); it++)
   {
      int u = *it;
      if(process[u]==0){
         double x =residual[u][cor];

         if(x>max){
            max = x;
            vertex = u;
         }
      }
   }  

   return vertex;
}


int remove_from_clique(list<int> &N, list<int> &clique, int me){
   list<int>::iterator it;
   int num=0;
   int u;

   list<int> prov;
   prov.clear();
   for(it = clique.begin(); it != clique.end(); it++)
   {
      prov.push_back(*it);
   }

   for(it = prov.begin(); it != prov.end(); it++)
   {
      u = *it;
      if(u != me && me != -1 && !N.empty() && !Exists(N, u)){
         clique.remove(u);
         num++;
      }

   }
   return num;
}  

// Max-Back Heuristic for MinCut which returns the value of cut obtained
int heuristic(list<int> *edge, list<int> &clique, int nVertex, double **residual, int cor) {
   list<int>::iterator it, it2;
   int *process = new int [nVertex];
   int v=0;
   int n=1;
   for(int i=0;i<nVertex; i++){
      process[i] =0;
   }
   while(v != -1){
      v = select_vertex(edge, clique, process, residual, cor);
      if(v!=-1){
         process[v]=1;
         remove_from_clique(edge[v], clique, v);
      }      
   }

   return clique.size();         
}

