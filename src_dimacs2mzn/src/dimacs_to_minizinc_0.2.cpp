//basic dimacs to MiniZinc translator 
//date@: 21/04/21
//dev@rleon, pss

#include <stdlib.h>		
#include <string.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <climits>
#include <map>

using namespace std;

#define MAX_LINE_SIZE 250

typedef int BOOL;
typedef unsigned int GTYPE;

struct graph_t {
	unsigned int n;
	GTYPE** adj;
};

typedef struct graph_t GRAPH;

//*************
//* Matrix allocation
//*
//**************
GTYPE** allocate_gt (int n);
void free_gt(GTYPE** gt, int n);
void print_gt(const GTYPE**, int n);

//*************
//  Graph management
//*************

 GRAPH* init_graph(int n);	
 void free_graph(GRAPH*);

 GRAPH* gopen(const char*);
 void gclose(GRAPH*);			//equivalent to free_graph		

 void info(const GRAPH*);
 int  number_of_edges(const GRAPH*);
 int  number_of_vertices(const GRAPH* pg) { return pg->n; }
 
 int add_edge( GRAPH*, int v, int w);
 BOOL isEdge(const GRAPH* pg, int v, int w) { return (pg->adj[v][w] == 1); }


 GRAPH* read_dimacs	(const char* filename);		//read dimacs format, tokenizer philosophy
   int* read_csp	(const char* filename, int& n);		//read dimacs format, tokenizer philosophy
  



//***************
// Global data for MiniZinc models
// **************

vector<string> lista_variables; 	// Guarda la lista de variables.
map<string, int> base_variable;		// Mapa de cada Variable con su coordenada base, debe sustituir a base_array.
map<string, int> rango_variable; 	// Mapa con el rango de valores de las variables.
string tabla_actual;				// Apunta a la tabla que se está utilizando en el momento
int indice_tabla = 0;				// Índice de las tablas que se van creando

// **************
// Minizinc translator functions
// **************

void write_mzn(FILE *fileMzn,string text);
void data_mzn(int *VAR,int nV,FILE *fileMzn);
int  count(string var1,string var2,GRAPH *g);
void tuples_writer(string var1,string var2,int numero_unos,FILE *fileMzn,GRAPH *g);
void create_mnz(FILE *fileMzn,GRAPH *g);
void close_mzn(FILE *fileMzn);



/////////////////////////////////////
// Minizinc Functions
////////////////////////////////////


void write_mzn(FILE *fileMzn,string text){
		fprintf(fileMzn,text.c_str());
	}



void data_mzn(int *VAR,int numVar,FILE *fileMzn){
	string var;
	int siguiente_base = 0;
	string var_line = "var 0..";
	
	for (int i = 0; i < numVar; i++){
		var = "x" + to_string(i);
		lista_variables.push_back(var);
		rango_variable[var] = VAR[i];
		base_variable[var] = siguiente_base;
		siguiente_base += rango_variable[var];
		// cout << "Variable: " << var << endl;
		// cout << "Rango: " << rango_variable[var] << endl;
		// cout << "Base: " << base_variable[var] << endl;
	}
	
	for (int i = 0; i < lista_variables.size(); i++){
		var_line = "var 0..";
		var_line += to_string(rango_variable[lista_variables[i]]-1) + ": " + lista_variables[i] + ";\n";
		write_mzn(fileMzn,var_line);
	}
}



void create_mnz(FILE *fileMzn,GRAPH *g)
	{
		int numero_unos = 0;
		
		
		if (lista_variables.size() > 1)
		{
			for (int i=0; i< lista_variables.size()-1;i++)
			{
				for (int j = i+1; j<lista_variables.size();j++)
				{
					numero_unos = count(lista_variables[i],lista_variables[j],g);
					// cout << "Valor recibido de Número de unos: " << numero_unos << endl;
					if (numero_unos != 0)
					{
						// cout << "Escribo en el fichero las tuplas .............." << endl;
						tuples_writer(lista_variables[i],lista_variables[j],numero_unos,fileMzn,g);
					} else{
						// cout << "No hago nada ..........\n" << endl;
					}
				}
			}
		}
		else
		{
			cout << "no hay variables." << endl;
		}
		
	}




int count(string var1,string var2, GRAPH *g){
	int cuenta = 0;	
		// cout << "base uno: " << base_variable[var1] << endl;

		// cout << "\nEmpiezo con la pareja de variables: " << var1 << " - " << var2 << endl;
		// cout << "Rango variable uno: " << rango_variable[var1] << endl;

		// cout << "base dos: " << base_variable[var2] << endl;
		// cout << "Rango variable dos: " << rango_variable[var2] << endl;

		for (int i = base_variable[var1];i< base_variable[var1] + rango_variable[var1];i++)
		{
			for(int j= base_variable[var2];j < base_variable[var2] + rango_variable[var2];j++)
			{
				// cout << "Coordenadas: " << i << "-" << j << ": " << g->adj[i][j] << " ";
				if (g->adj[i][j] == 1)
					cuenta++;			
			}
		}

		// cout << "Contados: " << cuenta << " unos" << endl;
		/* if ((rango_variable[var1] * rango_variable[var2]) == cuenta)
		{
			cout << "Son todo unos............: " << (rango_variable[var1] * rango_variable[var2]) << endl;
		} */

		if (cuenta == (rango_variable[var1] * rango_variable[var2]) || cuenta == 0)
		{
			return 0;
		}
		else
		{
			return cuenta;
		}
}

void tuples_writer(string var1,string var2,int numero_unos,FILE *fileMzn,GRAPH *g)
	{
		string aux = "\nconstraint table([" + var1 + "," + var2 + "],";
		string auxArray,auxTabla;

		// cout << "\nGENERO TABLA PARA ..............." << aux << endl;
		
		tabla_actual = "table_" + to_string(indice_tabla);
		indice_tabla++;

		aux += tabla_actual + ");\n";

		write_mzn(fileMzn,aux);

		// Declaración de la tabla
		auxArray = "\narray[1.." + to_string(numero_unos) + ", 1..2] of int: " + tabla_actual + ";\n" ;
		write_mzn(fileMzn,auxArray);

		auxTabla = tabla_actual + " = array2d(1.."+ to_string(numero_unos) + ", 1..2, [\n";
		write_mzn(fileMzn,auxTabla);

		for (int i=0; i < rango_variable[var1];i++)
			{
				for (int j = 0; j < rango_variable[var2];j++)
				{
					int indice1 = base_variable[var1]+i;
					int indice2 = base_variable[var2]+j;	
					if (g->adj[indice1][indice2] == 1)
					{
						//cout << "Tuplas: " << i << "," << j << endl;
						write_mzn(fileMzn,to_string(i)+","+to_string(j)+",\n");
					}
				}
			}
		write_mzn(fileMzn,"]);\n");
	}



void close_mzn(FILE *fileMzn){

		std::cout << "Closing file ..............." << std::endl;		
		int i=0;
		string aux;

		write_mzn(fileMzn,"\n\nsolve satisfy;\n\n");
		write_mzn(fileMzn,"output [\"Solution: \",");
		for (i=0;i<lista_variables.size();i++){
			aux = "show(" + lista_variables[i] + "),";
			write_mzn(fileMzn,aux);
		}

		write_mzn(fileMzn,"\"\\n\"];\n");
		fclose(fileMzn);
	}




////////////////////////////////

GTYPE ** allocate_gt(int n)
{
	int i,j;
	GTYPE** gt = NULL;
	gt = (GTYPE**)malloc(sizeof(GTYPE*)*n);
    for (i = 0; i < n; i++) {
		//adj[i] = (GTYPE*)calloc(n, sizeof(GTYPE));
		gt[i] = (GTYPE*)malloc(sizeof(GTYPE)* n);
	}

	////////////////
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			gt[i][j] = 0;
		}
	}
	
	return gt;
}

void free_gt(GTYPE ** gt, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		free(gt[i]);
	}
	free(gt);
	gt = NULL;
}

void print_gt(const GTYPE **gt, int n)
{
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%f ", gt[i][j]);
		}
		puts("");
	}

}

GRAPH* init_graph(int n) {
	GRAPH* pg = (GRAPH*)malloc(sizeof(GRAPH));
	
	//error check if(g==NULL){... return NULL;}

	pg->n = n;
	pg->adj = allocate_gt(n);
	
	//error check if(g==NULL){... return NULL;}


	return pg;
}

void free_graph(GRAPH * pg)
{
	free_gt(pg->adj, pg->n);
	free(pg);
	pg = NULL;

}

GRAPH * gopen(const char * filename)
{
	return read_dimacs(filename);

}




void gclose(GRAPH *pg)
{
	free_gt(pg->adj, pg->n);
	free(pg);
	pg = NULL;
}

int add_edge( GRAPH * pg, int v, int w)
{
	if (v < 0 || v >= pg->n || w < 0 || w >= pg->n || v == w) { return -1; }

	pg->adj[v][w] = 1;
	pg->adj[w][v] = 1;

	return 0;
}

void info(const GRAPH *pg)
{
	printf("(%d, %d)\n", number_of_vertices(pg), number_of_edges(pg));

}

GRAPH * read_dimacs(const char * filename)
{
	GRAPH* g = NULL;
	int n, m, i, v, w;
	char line[MAX_LINE_SIZE + 1];


	FILE* fin = fopen(filename, "r");
	if (!fin) {
		perror(filename);
		return NULL;
	}

	//process up to 'e'
	while (!feof(fin)) {
		fgets(line, MAX_LINE_SIZE, fin);
		
		//TODO- add error code here

		if ((*line) == 0 || *line == '\n') continue;
		if ((*line) == 'p') {
			sscanf(line, "%*s%*s%d%d", &n, &m);

			//allocation
			g = init_graph(n);
			break;
		}
	}

	//tokenizer 
	for (i = 0; i < m; i++) {
		fscanf(fin, "%*s%d%d", &v, &w);

		//TODO- add error code here
		add_edge(g, v - 1, w - 1);

	}

	return g;
}

int* read_csp(const char * filename, int& n)
{
////////////////
//
// PARAMS:
// @n: number of variables
//
// RETURN:
// Array of sizes taken from the *.csp file or NULL if error


	int nElem=0, sV=0 ;
	char line[MAX_LINE_SIZE + 1];
	int* VAR = NULL;
	FILE* fin = fopen(filename, "r");
	if (!fin) {
		perror(filename);
		return NULL;
	}


	//process up to 'x' - read number of variables
	while (!feof(fin)) {
		fgets(line, MAX_LINE_SIZE, fin);

		//TODO- add error code here

		if ((*line) == 0 || *line == '\n') continue;
		if ((*line) == 'x') {
			sscanf(line, "%*s%d", &n);
			VAR = new int[n];
			break;
		}
	}

	//tokenizer 
	for (int i = 0; i < n; i++) {
		fscanf(fin, "%*s%*s%d", &sV);

		//TODO- add error code here
		VAR[i] = sV;
	}
	return VAR;
}

int number_of_edges(const GRAPH *pg)
{
	int i, j, nEdges = 0;
	for (i = 0; i < pg->n - 1; i++) {
		for (j = i+1; j < pg->n; j++) {
			if (pg->adj[i][j] == 1) {
				nEdges++;
			}
		}
	}
	return nEdges;
}


//////////////////////////////////
//MAIN

int main(int argc, char **argv) {
	char fileName[256];
	char *file_aux;
	FILE *fileMzn;

	if (argc != 2) {
		try {
			throw std::runtime_error("file in dimacs format is missing (.clq file)");
		}
		catch (exception& e) {
			cout << e.what();
			exit(-1);
		}
	}

	strcpy(fileName, argv[1]);

	cout << "Opening file: " << fileName << endl;
	// ASSUMPTION: The program will be called with the extension .clq


	////////////////////////////////
	//process DIMACS file (*.clq)
	GRAPH* g;
	g = gopen(fileName);
	// info(g);	//output to screen of (n,m)

	if (g == NULL) {
		cout << "error when reading the dimacs file: " << fileName << " ... exiting" << endl;
		exit(-1);
	}

	////////////////////////////////
	//process partition info in  *.csp file

	file_aux = strrchr(fileName, '.');
	strcpy(file_aux, ".csp");
	cout << "Opening file: " << fileName << endl;
	int nV = 0;
	int* VAR = read_csp(fileName, nV);				//VAR-domain info, nV-number of variables

	if (VAR == NULL) {
		cout << "error when reading the csp file: " << fileName << " ... exiting" << endl;
		exit(-1);
	}

	//I/O
	// std::cout << "Matrix dimension ....... "<< number_of_vertices(g) <<std::endl;

	//////////////////
	// Creating output file in MiniZinc format
	file_aux = strrchr(fileName, '.');
	strcpy(file_aux, ".mzn");
	fileMzn = fopen(fileName, "w");
	cout << "Minizinc file: " << fileName << endl;
	std::cout << "Writing file ................" << std::endl;

	// Writing file header
	write_mzn(fileMzn, "%%  MINIZINC file created from .clq & .csp files \n\n");
	write_mzn(fileMzn, "include \"table.mzn\";\n\n\n");
	write_mzn(fileMzn, "%%  Variables declaration: \n\n");


	data_mzn(VAR, nV, fileMzn);

	create_mnz(fileMzn, g);

	close_mzn(fileMzn);


	gclose(g);
	delete[] VAR;
	return (0);
}


