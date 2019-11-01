#include <ilcplex/ilocplex.h>
#include <list>
#include "clique.h"

#define PRECISAO 0.00001

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix;

//ILOLAZYCONSTRAINTCALLBACK5(cut, NumVarMatrix, x, IloNumVarArray, w, int, nVertex, double**, residual, list<int> *, edge)
ILOUSERCUTCALLBACK6(cut, NumVarMatrix, x, IloNumVarArray, w, int, nVertex, double**, residual, list<int> *, edge, int *, numc)
{
  // x        -> vetor das variaveis do problema
  // dim      -> dimensao do problema (numero de cidades)
  // residual -> armazena grafo com a solucao corrente
  // cutset   -> conjunto S a ser encontrado
  // numc     -> contador de numero de cortes gerados
  
  IloEnv env  = getEnv();
  bool   DEPU = true;
  
  // cria vetor da solucao corrente fracionaria 
  for(int i=0; i < nVertex; i++) {
    for(int j=0; j < nVertex; j++) {
      residual[i][j] = 0;
    }
  }

  for(int i=0; i < nVertex; i++) {
    for(int j=0; j < nVertex; j++) {
      double x_ij = getValue(x[i][j]);

      if (x_ij < PRECISAO)
        x_ij = 0;

      residual[i][j] = x_ij;
    }
  }
  // ******************** metodo para corte minimo *******************************

  list<int> clique;
  for(int j=0; j<nVertex && getValue(w[j])>0 +PRECISAO;j++){
    clique.clear(); 

    for(int i=0; i<nVertex; i++){
      clique.push_back(i);
    }

    list<int>::iterator it;
    int h = heuristic(edge, clique, nVertex, residual, j);
   
    double verifica_corte=0;
    int i;
    for(it = clique.begin(); it != clique.end(); it++){
      i = *it;
      verifica_corte+= residual[i][j];
    } 
    if(h>2 && verifica_corte>1+PRECISAO){
      cout<< "H : " << h << "e verifica_corte eh: "<< verifica_corte<< endl;
      IloExpr corte(env);
      
      for(it = clique.begin(); it != clique.end(); it++) {
        i = *it;
        corte += x[i][j];
      }
      cout<<"aqui?"<<endl;
      add(corte + PRECISAO<=1+PRECISAO).end();
      // libera memoria

      cout<<"eae"<<endl;
      corte.end();
    }
  }
  *numc = *numc + 1;
 /* if(h>2 && verifica_corte>1){
    // constroi corte
    
    int i;

    list<int>::iterator it;
    for(int j=0; j<nVertex;j++){
      IloExpr corte(env);
      double  val=0;
      
      for(it = clique.begin(); it != clique.end(); it++) {
        i = *it;
        corte += x[i][j];
        val += getValue(x[i][j]);
      }
      add(corte < 1).end();
      cout<<"VVVVVVVVVVVAAAAAAAAAAAAAAALLLLLLL "<<val<<endl;
      // libera memoria
      corte.end();

    }
    
    // insere corte
    
    numc = numc + 1;

  }*/

  double ub     = getIncumbentObjValue();  // retorna a melhor solucao inteira (limite primal)
  double lb     = getBestObjValue();       // retorna o melhor limite dual
  double rlx    = getObjValue();           // quando chamada dentro do callback, retorna o valor da relaxacao do noh
  double nNodes = getNremainingNodes();    // retorna o numero restante de nos a serem analisados
  cout<<"--- USER CUT:"<<"relax="<<rlx<<"\t bounds="<<lb<<"<->"<<ub<<"\t n_rest="<<nNodes<<"\t Ucuts="<<*numc<<endl;

}

int main(int argc, char *argv[])
{
  IloEnv             env;   // declara Variável de ambiente do CPLEX
  IloModel     model(env);  // declara Variavel do modelo do CPLEX (que existe dentro do ambiente criado)

  const char* filename  = "teste.pc";
  if(argc>1)
    filename = argv[1];
  ifstream file(filename);

  int nVertex, nEdges;
  
  char trash[10];

  file >> trash >> trash >> nVertex >> nEdges;

  list<int> *edge = new list<int>[nVertex];
  
   while(!file.eof()){
    int i, j;
    file >> trash >> i >> j;
    edge[i-1].push_back(j-1);    
  }

  // 1)Variaveis
  // w[j] color j was used.
  // x[i][j]  vertex i was colored with color j
  IloNumVarArray w(env, nVertex, 0, 1, ILOINT);
  NumVarMatrix x(env, nVertex);
  for (IloInt i = 0; i < nVertex; i++)
     x[i] = IloNumVarArray(env, nVertex, 0, 1, ILOINT);

  // Constraint1:To each vertex  must be assigned exactly one color:
  for (IloInt i = 0; i < nVertex; ++i)
     model.add(IloSum(x[i]) == 1);

  // Constraint2:

  list<int>::iterator it;

  for (IloInt j = 0; j < nVertex; j++) {
    for (IloInt i = 0; i < nVertex-1; i++) {
      for(it = edge[i].begin(); it != edge[i].end(); ++it)
      {
        model.add(x[i][j] + x[*it][j] <= w[j]);
      } 
    }  
  }

  // Constraint3: 
  for (IloInt i = 0; i < nVertex-1; i++) {
     model.add(w[i]>=w[i+1]);
  }

  // Constraint4:
  for (IloInt j = 0; j < nVertex; j++) {
     model.add(w[j] <= IloSum(x[j]));
  }

  //FO
  IloExpr obj(env);
  for (IloInt j = 0; j < nVertex; j++) {
      obj+= w[j];
    } 

  model.add(IloMinimize(env, obj));
  obj.end();

  // ***********************  callback de corte ******************************

  // estrutara usada para os métodos de geracao de cortes 
  double **residual = new double*[nVertex];               // grafo residual usado no corte
  for(int i=0; i < nVertex; i++)
    residual[i] = new double[nVertex];
  int numc=0;
  // Create a solver instance and extract the model to it.
  IloCplex cplex(env);
  
  cplex.extract(model); 

  // -- USER CUTS --
  // Sao cortes que nao eliminam nenhuma solucao viavel (e.g., cortes que eliminam solucoes fracionarias)
  // mas que se nao inseridos nao inviabilizam a corretude do modelo
  cplex.use(cut(env, x, w, nVertex, residual, edge, &numc)); 
  // *****************************************************************************
  // Parametros do CPLEX
  cplex.setParam(IloCplex::WorkMem,1024*2);   //tamanho de RAM utilizada maxima
  cplex.setParam(IloCplex::NodeFileInd, 2);   //quando a RAM acaba, 1-guarda nos na memoria e compactado 2-guarda os nos em disco 3-guarda nos em disco e compactado
  cplex.setParam(IloCplex::TiLim, 3600);      // tempo limite
  cplex.setParam(IloCplex::Threads, 1);       // Numero de threads
  cplex.setParam(IloCplex::MIPInterval, 100); // Log a cada N nos
  //cplex.setOut(env.getNullStream());          // Desabilitando saida do cplex

  // Parametros do B&C
  cplex.setParam(IloCplex::PreInd, 0);        // desliga o preprocessamento, 1-ligado 0-desligado
  cplex.setParam(IloCplex::CutsFactor, 0);    // número de cortes que o CPLEX pode gerar, 0-desliga os cortes
  cplex.setParam(IloCplex::HeurFreq, -1);     // heuristicas primais do CPLEX, -1-desliga 0-liga
    

  cout <<"wtf?"<<endl;
  // Solve the model and display solution.
  // Otimiza
    try
       {
   cplex.solve();
       }
     catch(IloException& e)
       {
   cout << e;
       }

  cplex.out() << "Solution status: " << cplex.getStatus() << endl;
  cplex.out() << "Optimal value: " << cplex.getObjValue() << endl;
  env.end();
  
}