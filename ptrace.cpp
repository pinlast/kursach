#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include "socket.cpp"
#include <iostream>
#include "syscall_names.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/reg.h>


char *getdata(pid_t pid, unsigned long addr, int len)
{   char *val = (char *)calloc(len + 1, (len+1) * sizeof(char));
    int read = 0;
    unsigned long tmp;
    for (int i=0; i<len; i++){
        tmp = ptrace(PTRACE_PEEKDATA, pid, addr + read);
        memcpy(val + read, &tmp, sizeof tmp);
        read += sizeof tmp;
    }
    return val;
}


void putdata(pid_t pid, unsigned long addr, char *str, int len)
{
    int read = 0;
    for (int i=0; i<len; i++){
        ptrace(PTRACE_POKEDATA, pid, addr + read, str[i]);
        read += sizeof str[i];
    }
}

void reverse(char *str)
{   int i, j;
    char temp;
    for(i = 0, j = strlen(str) - 2;
        i <= j; ++i, --j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}


int tracer(pid_t pid, user_regs_struct regs, std::string mode)
{
	int status;
	int in_call = 0;
	int counter = 0;
	char* str;

  /*The wait() system call suspends execution of the 
    calling thread until one of its children terminates.*/
  wait(&status);
  while (status == 1407)
  {
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);  // getting regs data
    std::string message = syscalls[regs.orig_rax] + "(" + std::to_string(regs.orig_rax) + ")" + ";";
          message += std::to_string(regs.rdi) + ";";
          message += std::to_string(regs.rsi) + ";";
          message += std::to_string(regs.rdx) + ";";
          message += std::to_string(regs.r10) + ";";
          message += std::to_string(regs.r8) + ";";
          message += std::to_string(regs.r9);
          socket_send(message);

    if (mode == "regs")
    {
        if (!in_call) {
          printf((message + "\n").c_str());
          in_call = 1;
          counter++;
        }
        else {
          in_call = 0;
        }
    } else if (mode == "reverse")
    {
      printf((message + "\n").c_str());
      counter++;
      if(regs.orig_rax == SYS_write) {
         if(in_call == 0) {
             in_call = 1;
            /* Syscall entry */
            char* str = getdata(pid, regs.rsi, regs.rdx);
            reverse(str);
            putdata(pid, regs.rsi, str, regs.rdx);
            }
         else { /* Syscall exit */
            in_call = 0;
         }
      }
      ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }

    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    wait(&status); // updating status
  }

  return counter;
}

int main(int argc, char *argv[])
{
	struct user_regs_struct regs;
	pid_t pid;
    int counter;
    std::string mode = argv[1];
    pid = fork();

	switch (pid)
	{
		case -1:
			/*could not fork */
			perror("fork");
			exit(1);

		case 0:
			/*in the child process */
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			execvp(argv[2], argv + 2);

		default:
			/*in the parent process */
			counter = tracer(pid, regs, mode);
	}

	printf("Total Number of System Calls = %d \n", counter);
	return 0;
}
