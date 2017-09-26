#define POBLACION 5
#define LONG_COD 8
#define LIMITE -5.12
#define PROB_CRUCE 0.3
#define PROB_MUTACION 0.001
#define INTERVALO 10.24/pow(2,LONG_COD/2)
#define NOMINAL 10
 

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime> 
#include <algorithm>
#include "cInstance.hpp"
#include "simpleXMLParser.hpp"

typedef vector<int> Solution;
int NC=0;

class Individuo {
	public:
		Solution solution;		
		float aptitud;
		float prob_mut;
}; 



string command;


bool areAllYellow(string phase) {
  for (int i = 0; i < phase.size(); i++)
    if (phase[i] != 'y')
      return false;
  return true;
}

///////////////////////////////////////////////////////////////////////
/*
FUNCIONES DE COMUNICACION CON FICHEROS
 */
///////////////////////////////////////////////////////////////////////
void writeSolutionFile(const Solution &solution)
{
	ofstream f("tl.txt");
	for(int i = 0; i < solution.size(); i++)
		f <<  solution[i] << " ";
	f.close();
}

void readFitnessFile(float &fitness)
{
	ifstream f("result.txt");
	string s;
	for(int i = 0; i < 6; i++) // skip lines
		getline(f,s);
	f >> fitness;
	f.close();
}

float evaluateSolution(const Solution &solution){
	float fitness;
	writeSolutionFile(solution);
	system(command.c_str());
	readFitnessFile(fitness);
	return fitness;
}


///////////////////////////////////////////////////////////////////////////////////////////
/*
FUNCIONES DEL ALGORITMO
*/
/////////////////////////////////////////////////////////////////////////////////////////////

Individuo generarIndividuo (const  cInstance &c){
    Individuo ind;   
    int i;  
    int pos;
    vector<string> phases;

	/*
	for(int i = 0; i < c.getTotalNumberOfPhases(); i++)
	{
		ind.solution.push_back(rand()%56 + 4);
	}*/
	
	for(int j = 0; j < c.getNumberOfTLlogics(); j++)
	{				
		phases = c.getPhases(j);				
		for(int k = 0; k < phases.size(); k++)
		{
			if(areAllYellow(phases[k]))
				ind.solution.push_back(4*phases[k].size());				
			else if(isSubString(phases[k],"y",pos))
				ind.solution.push_back(4);		
			else
				ind.solution.push_back(rand()%55 + 5);		
		}
	}
	                
    ind.aptitud = evaluateSolution(ind.solution);
    //cout<<"Aptitud: "<<ind.aptitud<<endl;	
    return ind;
}
 
 void generarPoblacion(vector<Individuo> &pob,const cInstance &c)
{    
    int i;   
    for(i=0;i<POBLACION;i++)
    {		
		pob.push_back(generarIndividuo(c));
	}
            
}

void imprimePoblacion (vector<Individuo> &pob)
{
    int i;
    for(i=0;i<pob.size();i++)
    {
        
        cout<<"Individuo :"<<pob[i].aptitud<<endl;     
    }
}
/*
void imprimePoblacion_clon (Individuo * pob)
{
    int i;
 
    for(i=0;i<POBLACION*3;i++)
    {
        printf ("INDIVIDUO NUMERO %i ---> ", i+1);
        imprimeGenotipo(pob[i]);     
        //printf ("<--- Aptitud = %d\n", pob[i].aptitud);
    }
}


*/

bool sort_by_avg(Individuo a1, Individuo a2) 
{
	return a1.aptitud < a2.aptitud; // usamos ">" porque queremos orden descendente
}

void seleccion(vector<Individuo> &pob)
{
	Individuo mayor;
	sort(pob.begin(), pob.end(), sort_by_avg);
	
}


void clonacion(vector<Individuo> &pob)
{	
	int i=POBLACION,j,k=0,contador=0;	
	vector<Individuo> temporal;
	temporal=pob;
	pob.clear();
	for(i=POBLACION;i>=0;i--)
	{			
		for(j=0;j<i;j++)
		{			
			pob.push_back(temporal[k]);						
			contador++;
		}
		k++;
	}
}


void maduracion(vector<Individuo> &pob,const cInstance &c,float generacion, int Nc)
{		
	int i,j;
	float random_prob=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);	
	for(i=0; i<pob.size();i++)
	{		
		if(random_prob<(pob[i].prob_mut/generacion+1))
		{
			float r=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float alpha=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);			
			for(j=0;j<c.getTotalNumberOfPhases();j++)
			{									
				if(pob[i].solution[j]!=4)
				   {
					   float alpha=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);			
					   int op = rand()%100;
					   int range;
					   if(i<5)
						 range=rand()%55+5;
					   else
						 range=pob[0].solution[rand()%c.getTotalNumberOfPhases()];
					   if(op<50)
						  if(r<0.5)
							 pob[i].solution[j]+=(int)((alpha*range*generacion)/Nc);							 
						  else
							 pob[i].solution[j]+=(int)((alpha*range)/(Nc*generacion));
					   else
						  if(r<0.5)
							 pob[i].solution[j]-=(int)((alpha*range*generacion)/Nc);
						  else
							 pob[i].solution[j]-=(int)((alpha*range)/(Nc*generacion));
						if(pob[i].solution[j]<4)
							pob[i].solution[j]=5;
						if(pob[i].solution[j]>60)
							pob[i].solution[j]=60;
					//cout<<pob[i].solution[j]<<"- ";
				   }
			}
			pob[i].aptitud = evaluateSolution(pob[i].solution);
		}		
    }
}

void regulation(vector<Individuo> &pob)
{
	int i;
	vector<Individuo> temporal;
	temporal=pob;
	pob.clear();
	pob.push_back(temporal[0]);
	pob.push_back(temporal[1]);
	pob.push_back(temporal[rand()%15]);
	pob.push_back(temporal[rand()%15]);
	pob.push_back(temporal[rand()%15]);
}

void elitismo(vector<Individuo> &pob,const cInstance &c)
{
	int i;
	vector<Individuo> temporal;
	temporal=pob;
	pob.clear();
	pob.push_back(temporal[0]);
	pob.push_back(temporal[1]);
	for(i=2;i<POBLACION;i++)
	{
		pob.push_back(generarIndividuo(c));
	}
}

void  SetProbMut(vector<Individuo> &pob)
{
	int i;
	float prob_tot=0;
	for(i=0;i<pob.size();i++)	
		prob_tot+=pob[i].aptitud;
	for(i=0;i<pob.size();i++)
		pob[i].prob_mut=(prob_tot/pob[i].aptitud);
}

int main (int argc, char **argv)
{
	srand(time(NULL));
    int i;
    float j;
    for(i=1;i<=POBLACION;i++)
    {
		NC+=(POBLACION-(i-1));
	}
	srand(time(NULL));
	char nombre[50];	
	unsigned steps;
	cInstance c;
	
	if(argc < 3)
	{
		cout << "Usage: " << argv[0] << " <instance> <number of generations>" << endl;
		exit(-1);
	}	
	
	
	c.read(argv[1]);
	steps = atoi(argv[2]);
	cout<<"Pasa";
	
	command = "./sumo-wrapper " + string(argv[1]) + " " + "tl.txt result.txt";
	
	vector <Individuo> poblacion;
    generarPoblacion(poblacion,c);     
    Individuo * pob_clon; 
    float best_aptitud=0;
    int generaciones=0;    
    //while(best_aptitud!=8)
    
    for(int i=0;i<10;i++)
    {
		for(j=0;j<NOMINAL;j++)
		{						
			seleccion(poblacion);
			SetProbMut(poblacion);					
			clonacion(poblacion);						
			maduracion(poblacion,c,j,NC);			
			seleccion(poblacion);			
			regulation(poblacion);													
		}
		best_aptitud=poblacion[0].aptitud;		
		generaciones++;		
		cout<<"Generacion "<<i<<" mejor individuo "<<best_aptitud<<endl;
		elitismo(poblacion,c);
	}                 
    printf("Resultado obtenido en la generacion: %d\n.",generaciones);                         
    printf ("*************************************\n");
    printf ("*          FIN DEL ALGORITMO        *\n");
    printf ("*************************************\n");

}

