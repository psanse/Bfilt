#!/usr/bin/python

from os import walk, getcwd
import os.path
import subprocess
import sys





def lista_con_path(ruta):
    
    lista_archivos = []
    
    for raiz, directorios, archivos in os.walk(ruta,topdown = True):
        for nombre in sorted(archivos):
            lista_archivos.append(os.path.join(raiz,nombre))
    return lista_archivos



def lanza_lzma(fichero):
    #Descomprimo el fichero
    comando = '/usr/bin/lzma -d '
    comando += fichero
    proceso = subprocess.Popen(comando,shell = isinstance(comando,str))
    return
                             


def descomprimo_directorio(familia):
    # Generacion de la lista de ficheros a procesar
    lista = lista_con_path(familia)
    print ('Familia: ' + familia)
    # Descomprimo cada fichero encontrado    
    for fichero_problema in lista:
        extension = os.path.splitext(fichero_problema)[1]
        if (extension == '.lzma'):
            print ('Fichero: ' + fichero_problema)    
            lanza_lzma(fichero_problema)
    return


def descomprimo_fichero(fichero):
    extension = os.path.splitext(fichero)[1]
    if (extension == '.lzma'):
        print ('Fichero: ' + fichero)
        lanza_lzma(fichero)
    else:
        print('Fichero con extension no valida.')
    return




#El Main()

if __name__ == '__main__':

    ubicaciones = []
    fichero_individual = []



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
                    num_ficheros = num_ficheros +1
                    fichero_individual.append(sys.argv[i+1])
                else:
                    print (sys.argv[i+1]+' Nombre de fichero o directorio no valido.')
    else:
        ubicaciones.append('.')
        num_directorios = num_directorios +1

    
        
    
    
    #Ejecuta el solver para cada fichero individual
    for fichero in fichero_individual:
        descomprimo_fichero(fichero)
    
    #Para cada uno de los directorios se crea una lista de ficheros
    # y se lanza el solver sobre ellos.
    for familia in ubicaciones:
        descomprimo_directorio(familia)
    
    
   

        
