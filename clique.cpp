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
   double fmax =0;
   double fmin =10000000;
   int vertex = -1;
   list<int> LC;
   srand(1);
   //int max =0;
   LC.clear();
   for(it = clique.begin(); it != clique.end(); it++)
   {
      int u = *it;
      if(process[u]==0){
         int nc = get_num_neighbourg_in_clique(edge[u], clique);
         double x = (residual[u][cor]+1) * nc;
       //  int x = get_num_neighbourg_in_clique(edge[u], clique);
         if(x>fmax && nc>1){
            fmax = x;
         }
         if(x<fmin && nc>1){
            fmin = x;
         }
      }
   }

   for(it = clique.begin(); it != clique.end(); it++)
   {
      int u = *it;
      if(process[u]==0){
         int nc = get_num_neighbourg_in_clique(edge[u], clique);
         double x = (residual[u][cor]+1) * nc;
         if(x >= fmin + (fmax-fmin)*0.8 && nc>1)
            LC.push_back(u);
      }
   }
   if(LC.size()>0){
      int random_pos = rand()%LC.size();
      int pos = 0;
      while(pos < random_pos)
      {
        LC.pop_front();
        pos++;
      }

      vertex = LC.front();
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

