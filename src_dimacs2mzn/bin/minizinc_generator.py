#!/usr/bin/python3

from os import walk, getcwd
import os.path
import subprocess
import time
import sys





def lista_con_path(ruta):
    
    lista_archivos = []
    
    for raiz, directorios, archivos in os.walk(ruta,topdown = True):
        for nombre in sorted(archivos):
            lista_archivos.append(os.path.join(raiz,nombre))
    return lista_archivos




def lanza_converter(problema,salida_solucion): 

    comando = './dim_to_mnz '
    resultado = []
  
    t_uno = time.time()

    print ('Comando: ' + comando)
    print ('Problema a procesar: ' + problema)
    
    comando += problema
    #Se lanza el solver
    #proceso = subprocess.Popen(comando, stdout = subprocess.PIPE, shell = isinstance(comando,str))
    proceso = subprocess.run(comando,stdout=subprocess.PIPE,universal_newlines=True,shell = isinstance(comando,str))
    
    if (proceso.returncode == 0):
        t_dos = time.time()    
        tiempo_s = round(t_dos - t_uno, 3)
        print('Tiempo generando el fichero MiniZinc .......  ' + str(tiempo_s) + '\tseg')

        # Se escribe a fichero los tiempos
        salida_solucion.write('\t')
        salida_solucion.write(str(tiempo_s)) 
    
    if (proceso.returncode == 4):
        print('Numero de variables excedido.')
   
   
    return 






def ejecuto_converter(familia,salida_solucion):
    # Generacion de la lista de ficheros a procesar
    lista = lista_con_path(familia)
    print ('Familia: ' + familia)
    # Generacion del grafo a partir de cada fichero encontrado    
    for fichero_problema in lista:
        extension = os.path.splitext(fichero_problema)[1]
        if (extension == '.clq'):
            ruta,nombre = os.path.split(fichero_problema)
            salida_solucion.write(nombre)
            salida_solucion.write('\t')
            salida_solucion.flush()
            lanza_converter(fichero_problema,salida)
            salida_solucion.write('\n')
            salida_solucion.flush()
            print('\n')
    return







def ejecuto_converter_fichero(fichero,salida_solucion):
    # Se genera un grafo de un fichero individual
    extension = os.path.splitext(fichero)[1]
    if (extension == '.clq'):
        #print (fichero)
        salida_solucion.write(fichero)
        salida_solucion.write('\t')
        salida_solucion.flush()
        lanza_converter(fichero,salida_solucion)
        salida_solucion.write('\n')
        salida_solucion.flush()
        print('\n')
    return





#El Main()

if __name__ == '__main__':

    ubicaciones = []
    fichero_individual = []
    solver_name = 'MINIZINC'

    # Creacion/apertura del fichero de salida
    nombre_fichero = 'output_' + solver_name + '.out'

    if(os.path.isfile(nombre_fichero) == 0):
        print ('Fichero ' + nombre_fichero + ' no existe, lo creamos y anadimos la cabecera.')
        salida = open(nombre_fichero,'w')
    
        # Preparacion de la informacion en el fichero de salida
        salida.write('\n' + solver_name + ' location: ')
        print('\n\n' + solver_name + ' LOCATION: ')
        path = str(os.path.dirname(os.path.abspath(__file__)))
        path = path + '/'
        print(path)
        salida.write('\n'+ path)
        salida.write('\n')
        print('\n')
        salida.write('\nORIGINAL FILES\t\t\t(t)MiniZinc Generation\n\n')
    else:
        print ('Fichero ' + nombre_fichero + ' abierto para anadir resultados.')
        salida = open(nombre_fichero,'a')
		    

    #Procesamiento de la linea de comandos
    
    num_directorios = 0
    num_ficheros = 0
    
    
    if (len(sys.argv) >= 2):
        for i in range(0,(len(sys.argv)-1)):
            if (os.path.isdir(sys.argv[i+1])):
                num_directorios = num_directorios +1
                ubicaciones.append(sys.argv[i+1])
            else:
                if (os.path.isfile(sys.argv[i+1])):
                    #salida.write('\t')
                    num_ficheros = num_ficheros +1
                    fichero_individual.append(sys.argv[i+1])
                else:
                    print (sys.argv[i+1]+' Nombre de fichero o directorio no valido.')
    else:
        ubicaciones.append('.')
        #salida.writelines('No hay argumentos, ejecutamos ' + solver_name + ' sobre los ficheros del directorio local: ' + ubicaciones[0] + '\n')
        print ('No hay argumentos, ejecutamos ' + solver_name + ' sobre los ficheros del directorio local: ' + ubicaciones[0] + '\n')
        num_directorios = num_directorios +1

    
    # if (num_ficheros > 0):
    #     print ('Ficheros: ' + str(num_ficheros))
    #     salida.writelines('Ficheros: '+ str(num_ficheros)+'\n')


    # if (num_directorios > 0):
    #     print ('Directorios: ' + str(num_directorios))
    #     salida.writelines('Directorios: '+ str(num_directorios)+'\n')

    
    
    
    #Ejecuta el solver para cada fichero individual
    for fichero in fichero_individual:
        ejecuto_converter_fichero(fichero,salida)
    
    #Para cada uno de los directorios se crea una lista de ficheros
    # y se lanza el solver sobre ellos.
    for familia in ubicaciones:
        ejecuto_converter(familia,salida)
    
    
    salida.close()

        
