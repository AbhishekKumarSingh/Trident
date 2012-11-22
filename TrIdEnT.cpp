/***********************************************************************************************************
*
*                      T R I D E N T    S H E L L   1.0
*                      
*                             [T r I d E n T]      
*
*		      Created by : Abhishek Kumar Singh 
*
*
***********************************************************************************************************/


#include <pwd.h>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include "Parser.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/history.h>
#include <readline/readline.h>

using namespace std;

extern int errno;
extern char** environ;
bool interactive = false;
int waitingpid = 0;
bool color_output = false;
string message = "[TrIdEnT] ";

/* Executes the command */
void execute(ParserResultPart r)
{
	char *searchPath = getenv("PATH");
	char **list_of_args = (char**)malloc(sizeof(char*) * (r.arguments.size() + 1));

	/* Provides colorization to output of ls/grep/fgrep/egrep command */
	if (r.command == "ls" or r.command == "grep"or r.command == "fgrep" or r.command == "egrep")
	{
		r.arguments.push_back("--color=auto");
	}	
	
	/* Finds in current directory */
	if (!searchPath)
	{
		searchPath = (char*)malloc(sizeof(char) * 1);
		strcpy(searchPath, "");
	}

	string path = r.getAbsolutePath(string(searchPath));
	if (!searchPath)
		free(searchPath);
	
	/* Prepares arguments list */
	list_of_args[0] = (char*)malloc(path.length() + 1);
	strcpy(list_of_args[0], path.c_str());
	list_of_args[0][path.length()] = NULL;
	int i;
	for(i = 0;i < r.arguments.size();i++)
	{
		list_of_args[i+1] = (char*)malloc(r.arguments[i].length() + 1);
		strcpy(list_of_args[i+1], r.arguments[i].c_str());
		list_of_args[i+1][r.arguments[i].length()] = NULL;
	}
	list_of_args[r.arguments.size() + 1] = NULL; /* Don't forget the tailing NULL. */
	
	/* Handles redirection */
	FILE* fp;
	int fd;
	
	if (r.redirIn != "")
	{
		fp = fopen(r.redirIn.c_str(), "r");
		fd = fileno(fp);
		if (dup2(fd, STDIN_FILENO) == -1)
			cerr << message << "Unable to redirect STDIN from " << r.redirIn << endl;
	}

	if (r.redirOut != "")
	{
		fp = fopen(r.redirOut.c_str(), "w");
		fd = fileno(fp);
		if (dup2(fd, STDOUT_FILENO) == -1)
			cerr << message << "Unable to redirect STDOUT to " << r.redirOut << endl;
	}

	if (r.redirAppend != "")
	{
		fp = fopen(r.redirAppend.c_str(), "a");
		fd = fileno(fp);
		if (dup2(fd, STDOUT_FILENO) == -1)
			cerr << message << "Unable to redirect and append STDOUT to " << r.redirAppend << endl;
	}
	
	execv(list_of_args[0], list_of_args);

	/* only runs when exec fails */
	cout << message << "Unable to execute command. (code: " << errno << ")" << endl;
	exit(-1);

	for(i = 0;i < r.arguments.size();i++)
		free(list_of_args[i]);
	free(list_of_args);
	
	return;
}

/* Expands "~" or "~username" part in the given string to absolute path and returns expanded version of input string */
string expandHomeDirectory(string str)
{
	/* If already an absolute path then return str */
	if (str[0] == '/')
		return str;
		
	/* User directory */
	if (str[0] == '~')
	{
		if (str.size() == 1)
		{
			string home = string(getenv("HOME"));
			return home;
		}
		else if (str[1] == '/')
		{
			string home = string(getenv("HOME"));
			return home + str.substr(1);
		}
		else
		{
			int index = str.find("/");
			string username = str.substr(1, index - 1);
			struct passwd *record = getpwnam(username.c_str());
			string path;
			if (record && record->pw_dir)
				path = string(record->pw_dir) + str.substr(index);
			else
				path = str;
			return path;
		}
	}
	
	return str;
}

/* truncates the extra whitespace in the begin or end of the string */
void truncate(string& str)
{
  string::size_type index = str.find_last_not_of(' ');
  if(index != string::npos) {
    str.erase(index + 1);
    index = str.find_first_not_of(' ');
    if(index != string::npos) str.erase(0, index);
  }
  else str.erase(str.begin(), str.end());
}

/* Spawns a new process to run a single command by executing ParserResultPart object */
void CreateProcess(ParserResultPart r)
{
	pid_t pid = fork();
	
	if (pid == 0)
		execute(r);
	else if (pid > 0)
	{
		if (!r.background)
		{
			waitingpid = pid;
			waitpid(pid, NULL, NULL);
		}
	}
	else
		cout << message << "Sorry, Can't be forked!" << endl;
	
	return;
}

/* Spawns new process in recursive manner to create specified pipe chain*/
int CreateProcess_pipe(vector<ParserResultPart>::iterator begin, vector<ParserResultPart>::iterator end)
{
	int fd[2];
	if (pipe(fd) == -1)
		cout << message << "Unable to create pipe (code: " << errno << ")" << "\n";
	
	vector<ParserResultPart>::iterator nextIter = begin + 1;
	
	pid_t pid = fork();
	if (pid == 0)
	{
		if (begin->pipe_with_next)
		{
			if (dup2(fd[1], STDOUT_FILENO) == -1)
				cerr << message << "dup2 failed. (STDOUT)\n";
		}

		if (close(fd[0]) == -1)
			cerr << message << "close failed. (PIPE_READ)\n";

		if (close(fd[1]) == -1)
			cerr << message << "close failed. (PIPE_WRITE)\n" ;
			
		execute(*begin);
	}
	else if (pid > 0)
	{
		if (dup2(fd[0], STDIN_FILENO) == -1)
			cerr << message << "dup2 failed. (STDIN)\n";

		if (close(fd[0]) == -1)
			cerr << message << "close failed. (PIPE_READ)\n";

		if (close(fd[1]) == -1)
			cerr << message << "close failed. (PIPE_WRITE)\n";
				
		if (nextIter == end)
		{
			if (!begin->background)
				waitpid(pid, NULL, NULL);
				
			return pid;
		}
		else
			return CreateProcess_pipe(nextIter, end);
	}
	else
		cerr << message << "Sorry, Can't forked!\n";
	
	return -1;
}

/* Handler for SIGINT(Ctrl + C), Kills the waiting process if not in interactive mode else exit TrIdEnT shell */
void sigint_handler(int signal)
{
	if (!interactive)
	{
		cout << endl << message << "Sending SIGINT to Kill the foreground process...\n";
		kill(waitingpid, SIGINT);
		cout << "\n";
	}
	else
	{
		cout << "\n" << message << "Caught SIGINT, Good bye!" << "\n";
		exit(0);
	}
}

int main()
{
	/* Checks if the STDOUT is a terminal and supports colour output */
	if (isatty(STDOUT_FILENO) && strcmp(getenv("TERM"), "dumb"))
		color_output = true;
	
	if (!color_output)
	{
		cout << "Welcome to TrIdEnT Shell[TrIdEnT] 1.0" << endl;
		cout << "Initializing in progress..." << endl;
	}
	else
	{
		cout << "\033[1;35mWelcome to \033[1;31mTrIdEnT Shell\033[m[\033[1;31mTrIdEnT\033[m] \033[1;35m1.0\033[m" << "\n";
		cout << "\033[1;33mInitializing in progress...\033[m" << "\n";
		message = "\033[1;31m[TrIdEnT]\033[m ";
	}

	/* Initialize the readline library and generates command prompt */
	vector<string> history;
	string command;
	Parser parser;
	char *input;
	using_history();
	char hostname[1024] = {0};
	gethostname(hostname, 1023);
	string home = string(getenv("HOME"));
	
	/* SIGINT catcher */
	signal(SIGINT, sigint_handler);
	
	cout << "\033[1;32mDone, dropping you to shell.\033[m" << endl << endl;
	
	/* Command input routine */
	while(true)
	{
		interactive = true;
		ostringstream oss;
		char *path_c = (char*)malloc(sizeof(char) * 1024);
		size_t size = 1024;
		getcwd(path_c, size);
		if (!path_c)
		{
			cout << message << "ERROR: Unable to construct prompt. (code: " << errno << ")" << endl;
			exit(-1);
		}
		
		string path = string(path_c);
		free(path_c);

		/* Constructs command prompt and if it contains home directory substitute it with "~" */
		int pos = path.find(home);
		if (pos != string::npos)
			path = "~" + path.substr(home.length(), path.length() - home.length());
		if (color_output)
			oss << "[\033[31m" << getenv("USER") << "\033[m@\033[32m" << hostname << "\033[m] " << path << "$ ";
		else
			oss << getenv("USER") << "@" << hostname << " " << path << "$ ";
		
		/* Reads input command */
		input = readline(oss.str().c_str());
		command = string(input, strlen(input));
		truncate(command);
		free(input);
		if (command == "") continue;
		add_history(command.c_str());
		interactive = false;
		
		/* Parses the input command */
		ParserResult r = parser.parse(command);
		
		/* Handling shell builtin commands if present else calls CreateProcess function to execute external commands */
		if (r.parts.size() == 0)
			continue;
		
		if (r.parts[0].command == "quit" || r.parts[0].command == "exit")
			break;
		else if (r.parts[0].command == "history")
		{	
			if (r.parts[0].arguments.size() > 0 and r.parts[0].arguments[0] == "-c")
				clear_history();
			else
			{
				for(int i = history_base; i < history_base + history_length; i++)
				{
					HIST_ENTRY *entry = history_get(i);
					printf("%2d  %s\n", i, entry->line);
				}
			}
			
		}
		else if (r.parts[0].command == "cd")
		{
			if (r.parts[0].arguments.size() == 0)
				chdir(home.c_str());
			else
			{
				chdir(expandHomeDirectory(r.parts[0].arguments[0]).c_str());
			}
		}
		else if (r.parts[0].command == "setenv")
		{
			if (r.parts[0].arguments.size() == 1)
				unsetenv(r.parts[0].arguments[0].c_str());
			else if (r.parts[0].arguments.size() == 2)
				setenv(r.parts[0].arguments[0].c_str(), r.parts[0].arguments[1].c_str(), true);
			else
			{
				cout << message << "setenv requires 1 or 2 arguments, you give me " << r.parts[0].arguments.size() << " arguments." << endl;
				cout << message << "Please check \"help\" command." << endl;
			}
		}
		else if (r.parts[0].command == "listenv")
		{
			char **ptr = environ;
			while(*ptr != NULL)
			{
				cout << *ptr << "\n";
				ptr++;
			}
		}
		else if (r.parts[0].command == "help")
		{
			if (color_output)
			{
				cout << "\033[1;37mTrIdEnT Shell[TrIdEnT] 1.0 -Help-\033[m" << "\n" << "\n";
				printf("%10s %s\n", "\033[1;32mcd\033[m", "change directory");
				printf("%10s %s\n", "\033[1;32msetenv\033[m", "(name) (value) Set environment variables, leave value empty to unset.");
				printf("%10s %s\n", "\033[1;32mlistenv\033[m", "List all environment variables");
				printf("%10s %s\n", "\033[1;32mhelp\033[m", "Display this help");
				printf("%10s %s\n", "\033[1;32mhistory\033[m", "Displays history list containing previously typed commands");
				printf("%10s %s\n", "\033[1;32mhistory -c\033[m", "clears entire history list");
				printf("%10s %s\n", "\033[1;32mquit\033[m", "Leave this shell");
				cout << "\n";
			}
			else
			{
				cout << "TrIdEnT Shell(TrIdEnT) 1.0 -Help-" << "\n" << "\n";
				printf("%10s %s\n", "\033[1;32mcd\033[m", "change directory");				
				printf("%10s %s\n", "setenv", "(name) (value) Set environment variables, leave value empty to unset.");
				printf("%10s %s\n", "listenv", "List all environment variables");
				printf("%10s %s\n", "help", "Display this help");
				printf("%10s %s\n", "\033[1;32mhistory\033[m", "Displays history list containing previously typed commands");
				printf("%10s %s\n", "\033[1;32mhistory -c\033[m", "clears entire history list");
				printf("%10s %s\n", "quit", "Leave this shell");
				cout << endl;				
			}
		}
		else
		{
			if (r.parts.size() == 1)
				CreateProcess(r.parts[0]);
			else
			{
				int pid = fork();
				if (pid == 0)
				{
					int lastpid = CreateProcess_pipe(r.parts.begin(), r.parts.end());
					waitpid(lastpid, NULL, NULL);
					exit(0);
				}
				else
				{
					waitingpid = pid;
					waitpid(pid, NULL, NULL);
				}
			}
		}
	}
}
