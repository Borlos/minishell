#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "execute.h"
#include "jobs.h"

int execute_piped_command_line(command* cmds, int n)
{
   
    if(!n) return 0;
    
    else{

        int status;
        pid_t id, id1;    
        int i, j;
        int pipefd[2*n];
        
        //Crea el doble de pipes a los pipes escritos,los pares(0,2,4...) de escritura, y los impares(1,3...) de escritura    
        for (i = 0; i < n; i++ ){
            if(pipe(pipefd + i*2) < 0){
                perror("Error al abrir pipe\n");
                return -1;
            }
        }
        //Bucle por cada pipe de comandos
        for(i = 0; i < n; i++){

            if((id = fork()) == 0){
                if(i) { //Si no es el primer hijo
                    if((dup2(pipefd[(i*2)-2], 0)) < 0){ //Abre el pipe de lectura par siguiente
                        printf("Error dup2 de lectura\n");
                        exit(EXIT_FAILURE);
                    }
                } 
                
                if(i < (n-1)){ //Si no es el último hijo
                    if((dup2(pipefd[(i*2)+1], 1)) < 0){ //Abre el pipe de escritura impar siguiente
                        printf("Error dup2 de escritura\n");
                        exit(EXIT_FAILURE);
                    }
                }

                for(j = 0; j < 2*n; j++){ //Cierra todos los pipes
                    close(pipefd[j]);
                }
                
                if (execvp(cmds[i].argv[0], cmds[i].argv) < 0){ //Ejecuta el comando
                    printf("Error al ejecutar comandos\n");
                    exit(EXIT_FAILURE);
                }
            }    

            else if (id == -1) {
                printf("Error al crear Hijo\n");
                return -1;
            }

            if(!i) { //Padre, si es el primer hijo guarda su id y crea el job
                id1 = id; 
                jobs_new(id, cmds[0].argv[0]);                
            }
        }//Fin bucle Hijos
        
        for(i = 0; i < 2*n; i++) close(pipefd[i]); //Padre, tras crear todos los hijos cierra todos los pipes
    
        if(!(cmds->background)){
            for (i = 0; i < n-1; i++) wait(&status);
            jobs_finished(id1);
        }            
    }

    return 0;
}

