#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>


int in_parent_process(pid_t pid, user_regs_struct regs)
{
	int status;
	int in_call = 0;
	int counter = 0;

  /*The wait() system call suspends execution of the 
    calling thread until one of its children terminates.*/
  wait(&status);
  while (status == 1407)
  {
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);  // getting regs data
    if (!in_call) {
      printf("SystemCall %llu called with %llu, %llu, %llu\n", 
             regs.orig_rax, regs.rbx, regs.rcx, regs.rdx);
      in_call = 1;
      counter++;
    }
    else {
      in_call = 0;
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

	switch (pid = fork())
	{
		case -1:
			/*could not fork */
			perror("fork");
			exit(1);

		case 0:
			/*in the child process */
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			execvp(argv[1], argv + 1);

		default:
			/*in the parent process */
			counter = in_parent_process(pid, regs);
	}

	printf("Total Number of System Calls = %d \n", counter);
	return 0;
}
