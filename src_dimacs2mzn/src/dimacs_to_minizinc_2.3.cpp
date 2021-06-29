//basic dimacs microstructure graph to MiniZinc translator 
//date@: 21/04/21
//dev@rleon, pss

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
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
//**************

GTYPE** allocate_gt (int n);
void free_gt(GTYPE** gt, int n);
void print_gt(const GTYPE**, int n);
void mi_print_gt(GRAPH* pg);


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

 GRAPH* read_dimacs	(const char* filename);				//read dimacs format, tokenizer philosophy
   int* read_csp	(const char* filename, int& n);		//read dimacs format, tokenizer philosophy


//***************
// Data for MiniZinc creation
// **************

vector<string> lista_variables; 	// it keeps variable list
map<string, int> base_variable;		// Map of each variable with is base matrix coordinate
map<string, int> rango_variable; 	// it keeps the value range of each variable
string tabla_actual;				// Pointer to the table processed each moment
int indice_tabla = 0;				// Variables index


// **************
// Minizinc Functions
// **************

void write_mzn(FILE *fileMzn,string text);
void data_mzn(int *VAR,int nV,FILE *fileMzn);
int  count(string var1,string var2,GRAPH *g);
void tuples_writer(string var1,string var2,int numero_unos,FILE *fileMzn,GRAPH *g);
void tuples_writer_not(string var1,string var2,int numero_unos,FILE *fileMzn,GRAPH *g);
void create_mnz(FILE *fileMzn,GRAPH *g);
void close_mzn(FILE *fileMzn);




//////////////////////////////////
// MAIN-UNIT TESTS
//////////////////////////////////

int main(int argc, char **argv){
	char fileName[256];
	char *file_aux;
	FILE *fileMzn;


	// Opening the source graph file
	if (argc != 2) {
		try {
			throw std::runtime_error("file in dimacs format is missing (.clq file)");
		}
		catch (exception& e) {
			cout << e.what();
			exit(-1);
		}		
	}

	strcpy(fileName,argv[1]);

	cout << "Opening file: " << fileName << endl;
	// ASSUMPTION: The program will be called with the 
	// .clq file.
	
	
	//**********************
	// process DIMACS file
	//**********************

	GRAPH* g;
	

	g = gopen(fileName);
	
	if (g == NULL) {
		cout << "error when reading the dimacs file: " << fileName << " ... exiting" << endl;
		exit(-1);
	}
	

	//*********************
	// Reading CSP file
	//*********************

	file_aux = strrchr(fileName, '.');
	strcpy(file_aux,".csp");
	cout << "Opening file: " << fileName << endl;
	int nV=0;
	int* VAR = read_csp(fileName, nV);				//VAR-domain info, nV-number of variables

	// cout <<"============" << endl;
	// info(g);
	// mi_print_gt(g);
	// cout <<"============" << endl;

	if (VAR == NULL) {
		cout << "error when reading the csp file: " << fileName << " ... exiting" << endl;
		exit(-1);
	}

	// Creating output file
	file_aux = strrchr(fileName, '.');
	strcpy(file_aux,".mzn");
	fileMzn = fopen(fileName,"w");
	cout << "Minizinc file: " << fileName  << endl;
	std::cout << "Writing file ................" << std::endl;
	
	// Writing file header
	write_mzn(fileMzn,"%%  MINIZINC file created from .clq & .csp files \n\n");	
	write_mzn(fileMzn,"include \"table.mzn\";\n\n\n");
	write_mzn(fileMzn,"%%  Variables declaration: \n\n");

	// Writing MiniZinc info
	data_mzn(VAR,nV,fileMzn);
	create_mnz(fileMzn,g);
	close_mzn(fileMzn);
	
	// Deallocate memory
	gclose(g);
	delete[] VAR;
	return (0);
}



/////////////////////////////////////
// Minizinc Functions
////////////////////////////////////


//*********************
// Outputs Minizinc file
//*********************

void write_mzn(FILE *fileMzn,string text){
		fprintf(fileMzn,text.c_str());
	}


//*********************
// Creates the variables data structure
// in the Minizinc file
//*********************

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
	}
	
	for (int i = 0; i < lista_variables.size(); i++){
		var_line = "var 0..";
		var_line += to_string(rango_variable[lista_variables[i]]-1) + ": " + lista_variables[i] + ";\n";
		write_mzn(fileMzn,var_line);
	}
}


//*********************
// Driver- MiniZinc file generation
//*********************

void create_mnz(FILE *fileMzn,GRAPH *g)
	{
		int nOnes = 0;
		
		if (lista_variables.size() > 1){
			for (int i=0; i< lista_variables.size()-1;i++){
				for (int j = i+1; j<lista_variables.size();j++){
					int maxVAL = rango_variable[lista_variables[i]] * rango_variable[lista_variables[j]];
					nOnes = count(lista_variables[i],lista_variables[j],g);
					if (nOnes == maxVAL) { continue; }		//all ones- nothing to do
					if (nOnes == 0){
						tuples_writer_not(lista_variables[i],lista_variables[j], nOnes,fileMzn,g);
						cout << " Found trivially UNSAT" << endl;
						return ;			//early exit-proven UNSAT
					}
					else {
						//A) produces less compact (minizinc) models but the conversion to flatzinc is easier (DEFAULT)
						tuples_writer(lista_variables[i], lista_variables[j], nOnes, fileMzn, g);

						//B) produces more compact models but the conversion to flatzinc is more difficult apparently!
						/*if (2 * numero_unos - maxVAL > 0) {
							tuples_writer_not(lista_variables[i], lista_variables[j], nOnes, fileMzn, g);
						}
						else {
							tuples_writer(lista_variables[i], lista_variables[j], nOnes, fileMzn, g);
						}*/
					}					
				}
			}
		}
		else{
			cout << "no hay variables." << endl;
		}
	}


//*********************
// Counts the number of "1's" (valid tuples) 
// between two variables
//*********************

int count(string var1,string var2, GRAPH *g){
	int nOnes = 0;	
		
		for (int i = base_variable[var1];i< base_variable[var1] + rango_variable[var1];i++){
			for(int j= base_variable[var2];j < base_variable[var2] + rango_variable[var2];j++){
				if (g->adj[i][j] == 1)
					nOnes++;
			}
		}	
			
		return nOnes;
}


//*********************
// Outputs the table constraint to the Minizinc file
//*********************

void tuples_writer(string var1,string var2,int numero_unos,FILE *fileMzn,GRAPH *g){
		string aux = "\nconstraint table([" + var1 + "," + var2 + "],";
		string auxArray,auxTabla;
		
		tabla_actual = "table_" + to_string(indice_tabla);
		indice_tabla++;

		aux += tabla_actual + ");\n";
		write_mzn(fileMzn,aux);

		// Table declaration: MiniZinc has to entries for each table
		auxArray = "\narray[1.." + to_string(numero_unos) + ", 1..2] of int: " + tabla_actual + ";\n" ;
		write_mzn(fileMzn,auxArray);

		auxTabla = tabla_actual + " = array2d(1.."+ to_string(numero_unos) + ", 1..2, [\n";
		write_mzn(fileMzn,auxTabla);

		for (int i=0; i < rango_variable[var1];i++){
				for (int j = 0; j < rango_variable[var2];j++){
					int indice1 = base_variable[var1]+i;
					int indice2 = base_variable[var2]+j;	
					if (g->adj[indice1][indice2] == 1)
						write_mzn(fileMzn,to_string(i)+","+to_string(j)+",\n");
				}
		}
		write_mzn(fileMzn,"]);\n");
	}

//*********************
// Outputs the NOT table constraint to the Minizinc file
//*********************

void tuples_writer_not(string var1,string var2,int numero_unos,FILE *fileMzn,GRAPH *g){

		int numero_ceros = (rango_variable[var1]*rango_variable[var2])-numero_unos;

				
		string aux = "\nconstraint not table([" + var1 + "," + var2 + "],";
		string auxArray,auxTabla;
		
		tabla_actual = "table_" + to_string(indice_tabla);
		indice_tabla++;

		aux += tabla_actual + ");\n";
		write_mzn(fileMzn,aux);

		// Table declaration: MiniZinc has to entries for each table
		auxArray = "\narray[1.." + to_string(numero_ceros) + ", 1..2] of int: " + tabla_actual + ";\n" ;
		write_mzn(fileMzn,auxArray);

		auxTabla = tabla_actual + " = array2d(1.."+ to_string(numero_ceros) + ", 1..2, [\n";
		write_mzn(fileMzn,auxTabla);

		for (int i=0; i < rango_variable[var1];i++){
				for (int j = 0; j < rango_variable[var2];j++){
					int indice1 = base_variable[var1]+i;
					int indice2 = base_variable[var2]+j;	
					if (g->adj[indice1][indice2] == 0)
						write_mzn(fileMzn,to_string(i)+","+to_string(j)+",\n");
				}
		}
		write_mzn(fileMzn,"]);\n");
	}

//*********************
// closes MiniZinc file
//*********************

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
// Functions that read the graph 
// into a matrix
///////////////////////////////

//******************
// Allocates graph memory space
//******************

GTYPE ** allocate_gt(int n)
{
	int i,j;
	GTYPE** gt = NULL;
	gt = (GTYPE**)malloc(sizeof(GTYPE*)*n);
    for (i = 0; i < n; i++) {
		gt[i] = (GTYPE*)malloc(sizeof(GTYPE)* n);
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			gt[i][j] = 0;
		}
	}	
	return gt;
}


//******************
// Free memory space
//******************

void free_gt(GTYPE ** gt, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		free(gt[i]);
	}
	free(gt);
	gt = NULL;
}


//******************
// Prints on Terminal the graph
//******************

void print_gt(GTYPE **gt, int n)
{
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%f ", gt[i][j]);
		}
		puts("");
	}

}

void mi_print_gt(GRAPH * pg)
{
	int i, j;
	for (i = 0; i < pg->n; i++) {
		for (j = 0; j < pg->n; j++) {
			printf("%d ", pg->adj[i][j]);
		}
		puts("");
	}

}


//******************
// Initialize graph
//******************

GRAPH* init_graph(int n) {
	GRAPH* pg = (GRAPH*)malloc(sizeof(GRAPH));
	
	pg->n = n;
	pg->adj = allocate_gt(n);

	return pg;
}


//******************
// Free memory space
//******************

void free_graph(GRAPH * pg)
{
	free_gt(pg->adj, pg->n);
	free(pg);
	pg = NULL;
}


//******************
// Open the graph file and
// it starts processing
//******************

GRAPH * gopen(const char * filename)
{
	return read_dimacs(filename);
}


//******************
// It closes the graph
//******************

void gclose(GRAPH *pg)
{
	free_gt(pg->adj, pg->n);
	free(pg);
	pg = NULL;
}


//******************
// It adds a new edge to the graph
//******************

int add_edge( GRAPH * pg, int v, int w)
{
	if (v < 0 || v >= pg->n || w < 0 || w >= pg->n || v == w) { return -1; }

	pg->adj[v][w] = 1;
	pg->adj[w][v] = 1;

	return 0;
}


//******************
// It returns the vertices and edges values
//******************

void info(const GRAPH *pg)
{
	printf("(%d, %d)\n", number_of_vertices(pg), number_of_edges(pg));

}


//******************
// It reads the graph file
//******************

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

	while (!feof(fin)) {
		fgets(line, MAX_LINE_SIZE, fin);
		
		if ((*line) == 0 || *line == '\n') continue;
		if ((*line) == 'p') {
			sscanf(line, "%*s%*s%d%d", &n, &m);

			g = init_graph(n);
			break;
		}
	}

	//tokenizer 
	for (i = 0; i < m; i++) {
		fscanf(fin, "%*s%d%d", &v, &w);
		add_edge(g, v - 1, w - 1);
	}
	return g;
}


//******************
// It reads the CSP file
//******************

int* read_csp(const char * filename, int& n)
{
////////////////
//
// PARAMS:
// @n: number of variables
//
// RETURN:
// Array of sizes taken from the *.csp file or NULL if error

	// Para matrices todo ceros

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
		VAR[i] = sV;
	}
	
	return VAR;
}


//******************
// Counts the number of edges of the graph
//******************

int number_of_edges(const GRAPH *pg)
{
	int nEdges = 0;
	for (int i = 0; i < pg->n - 1; i++) {
		for (int j = i+1; j < pg->n; j++) {
			if (pg->adj[i][j] == 1) {
				nEdges++;
			}
		}
	}
	return nEdges;
}


