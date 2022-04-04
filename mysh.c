#include "kernel/types.h"
#include "user/user.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/wait.h>
//#include <unistd.h>
//#include <string.h>
#include <stddef.h>
//#include <stdio.h>
#include <kernel/fcntl.h>

void type_prompt()
{
    static int first_time = 1;
    if ( first_time ) {
        printf("\n### Ben's Shell ###\n");
        first_time = 0;
    }
    printf(">>>");
}


char* strtok(char *str, const char* delim) {
    static char* _buffer;
    if(str != NULL) _buffer = str;
    if(_buffer[0] == '\0') return NULL;
 
    char *ret = _buffer, *b;
    const char *d;
 
    for(b = _buffer; *b !='\0'; b++) {
        for(d = delim; *d != '\0'; d++) {
            if(*b == *d) {
                *b = '\0';
                _buffer = b+1;
 
                // skip the beginning delimiters
                if(b == ret) { 
                    ret++; 
                    continue; 
                }
                return ret;
            }
        }
    }
 
    return ret;
}

char *ft_strdup(char *src)
{
    char *str;
    char *p;
    int len = 0;

    while (src[len])
        len++;
    str = malloc(len + 1);
    p = str;
    while (*src)
        *p++ = *src++;
    *p = '\0';
    return str;
}

int read_command(char *args[] )
{ 
    char buf[100];
    int nbuf = sizeof(buf), i =0;
    char *array[100], *pch;

    args[0] = "";
    // args[1] = NULL;

    gets(buf, nbuf);
    if (buf[0] == '\n') 
        return -1;


    pch = strtok(buf, " \n");
    while (pch != NULL) {
        array[i++] = ft_strdup(pch);
        pch = strtok(NULL, " \n");
    }
    for (int j = 0; j < i; j++) {
        args[j] = array[j];
        if (j == i-1)
        {
            args[j+1] = '\0'; //null termina
        }
        
    }
    //parse user input with strtok in args[]

    if (strcmp (args[0], "cd") == 0 )               //cd
        chdir(args[1]);

    if ( strcmp (args[0], "exit" ) == 0 )           //exit
        exit(0);

    return 0;
        
}

int pipe_check (char * args[], char *arg[]){

    return 0;
}


int redirect_input(char * args[20], int file)
{
    if(fork() == 0) {
        close(0);
        open(args[file], O_RDONLY);
        exec(args[0], args);
    }
    return 0;
}

int redirect_output(char * args[20], int file)
{
    if(fork() == 0) 
    {
        close(1);
        open(args[file],  O_CREATE | O_WRONLY);
        exec(args[0], args);
    }
    return 0;
}

int redirect_io(char * args[], int file)
{
    redirect_input(args,file);
    redirect_output(args,file);
    return 0;
}

//checks for redirection and returns the case
int redirection_check(char *args[20]){
    int out = 0;
    int in  = 0;
    int index = 0;
    int i = 1;

    while (args[i] != NULL)
    {    
        if (strcmp(args[i], "<")==0 || strcmp(args[i], ">")==0)
        {
            index = i;
            if (strcmp(args[i], "<")==0){
                args[index] = 0;
                in ++;
            }
            if (strcmp(args[i], ">")==0){
                args[index] = 0;
                out++;
            }
        }
        i++;
    }
    
    

    if((out != 0) && (in != 0)){
        //both inut and output redirection
         redirect_io(args, index);
        return 3;
    }else if(out != 0){
        redirect_output(args, index);
        return 2;
    }else if(in != 0){
        redirect_input(args, index);
        return 1;
    }else{
        return -1;
    }
}


void do_pipe(char * arg[20], char * args[20])
{
    int p[2];
    
    pipe(p);
    if (fork()==0){
        close(1);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        exec(arg[0],arg);
    }
    if (fork()==0){
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        exec(args[0],args);
    }
    close(p[0]);
    close(p[1]);
    wait(NULL);
    // close
    // dup
    // close
    // exec
}


int main ()
{                         
    char *arg[20], *args[20]; 
    while (1)
    {
        for (int i = 0; i < 20; i++)
        {
            arg[i] = NULL;
        }
        
        type_prompt();                  //output prompt
        read_command(arg);              //read and parse command
        if ( fork() == 0 )              
        {

            //check redirect
            
            redirection_check(arg);
    
            //check pipes
            if (pipe_check(arg,args))
            {
                //do_pipe(arg,args);
            }
            
            
            //execute command
            exec ( arg[0], arg);
            exit(0);
        }
    }
    return 0;
}