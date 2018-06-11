#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "execute.h"
#include "jobs.h"
#include "parser.h"
#include "minishell_input.h"

void show_command(command * p_cmd);
void child_handler(int sig);
void jobs();

int main (int argc, char *argv[])
{
    command* cmds;
    char* raw_command, *split; //Split son los vectores de caracteres en que se divide el command* según encuentre ";"
    int n_cmds=0;
    int n, i, end = 0, show = 0; // Uso end como flag para finalizar el main y show para mostrar o no los comandos introducidos
    
    struct sigaction act = {0}; //Estructura del sigaction
    act.sa_handler = child_handler; //Funcion que hace cada sigaction
    act.sa_flags = SA_RESTART;  //Este flag evita que haya un error si se estaba esperando entrada de usuario durante un sigact
    sigaction(SIGCHLD, &act, NULL); //Comienza sigact con condición cada vez que haya un retorno de un hijo
        
    while (1)
    {
        usleep(20000); //Pequeño delay para que al volver aquí otras salidas de texto de hijos aparezcan antes
        print_prompt(); 
 
        raw_command = NULL;
        n = read_line (&raw_command, stdin); //Entrada de usuario
        if (n==-1){
            printf("error de entrada de user\n");
            end = 1;
        }
        
        //Bucle que realiza por cada ";" que haya, hace un parse del siguiente command* hasta que ve un ";"
        while((split = strsep(&raw_command, ";")) != NULL){

            n_cmds = parse_commands(&cmds, split); //cuenta el numero de comandos 
        
            if (!(strcmp(cmds->argv[0], "exit"))) end = 1; //Si el comando es exit activa flag de fin
            
            else if (!(strcmp(cmds->argv[0], "jobs"))) jobs(); //Si el comando es jobs, muestra los trabajos realizados
            
            else if (!(strcmp(cmds->argv[0], "show"))){
                show = !show;
                if(show) printf("Se mostratrá la información de los comandos introducidos\n");
                else printf("Se ocultará la información de los comandos introducidos\n");
            }
       
            else{
                if ((n = execute_piped_command_line(cmds, n_cmds)) < 0) {
                    end = 1;
                    printf("Salida %d", n);
                }
                else{   
                    if(show){
                        for (i = 0; i < n_cmds; i++){
                        show_command(&cmds[i]);
                        }
                    }      
                }
            }
        
        free_commands(cmds, n_cmds); //Libera comandos
        if(end) break;
        } //fin bucle de tratamiento/Ejecución de comandos

    free(raw_command); //Libera comandos
    if(end) break;

    }//fin de bucle main

    jobs_free_mem(); //libera jobs?
    exit(0);
}

//Muestra los jobs de esta sesión
void jobs(){
    
    int i;
   
    if (jobs_count == 0) printf("No hay jobs\n");
    
    else{
        for ( i = 0; i < jobs_count; i++){
            printf("\t-Trabajo numero %d\n", i+1);
            printf("\t  ·PID: %d\n", jobs_array[i].pid);
            printf("\t  ·Comando: %s\n", jobs_array[i].name); 
            printf("\t  ·Finalizado: %s\n", jobs_array[i].finished ? "Si": "No"); 
        }
    }
}

//Función del sigaction, Evita hijos zombies haciendoles kill cuando devuelvan algún estado
void child_handler(int sig){
    usleep(20000); 
    pid_t wpid;    
    int status;    

    if ((wpid = waitpid(-1, &status, WNOHANG)) > 0){
        jobs_finished(wpid);
        kill(wpid, 0);
   }
}


void show_command(command * p_cmd)
{
    int i;

    printf ("\tRaw command: \"%s\"\n", p_cmd->raw_command);
    printf ("\tNumber of arguments: %d\n", p_cmd->argc);

    for (i=0; i<=p_cmd->argc; i++)
        if (p_cmd->argv[i] != NULL)
            printf ("\t\targv[%d]: \"%s\"\n", i, p_cmd->argv[i]);
        else
            printf ("\t\targv[%d]: NULL\n", i);

    if (p_cmd->input)
        printf ("\tInput: \"%s\"\n", p_cmd->input);

    if (p_cmd->output)
        printf ("\tOutput: \"%s\"\n", p_cmd->output);

    if (p_cmd->output_err)
        printf ("\tError output: \"%s\"\n", p_cmd->output_err);

    printf ("\tExecute in the background: %s\n",
            p_cmd->background ? "Yes" : "No");
}
