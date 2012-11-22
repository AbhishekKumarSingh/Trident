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



#include "Parser.h"
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/* CheckFile checks existence of file at path */
bool CheckFile(string path)
{
	FILE *fp = NULL;
	fp = fopen(path.c_str(), "r");
	
	if (fp != NULL)
	{
		fclose(fp);
		return true;
	}
	
	return false;
}

/* Finds command executable file in the given path and returns absolute path of the executable file */
string ParserResultPart::getAbsolutePath(string searchPath)
{
	/* Changing delimiter ":" to single space */	
	for(int i=0;i<searchPath.length();i++)
	{
		if (searchPath[i] == ':')
		{
			searchPath[i] = ' ';
		}
	}

	if (CheckFile(command))
		return command;
		
	if (CheckFile("./" + command))
		return "./" + command;
	
	istringstream iss(searchPath);
	string path;
	while (iss >> path)
	{
		if (CheckFile(path + "/" + command))
			return path + "/" + command;
	}
	
	return command;
}

/* Parses the given command and returns a ParserResult containing the parsed information*/
ParserResult Parser::parse(string command)
{
	int i = 0;	
	istringstream ss(command);
	string input_cmd;
	ParserResult r;
	ParserResultPart rp;
	
	bool IsRedirIn = false;
	bool IsRedirOut = false;
	bool IsRedirAppend = false;
	while(ss >> input_cmd)
	{
		bool OtherPart = false;
		
		if (i++ == 0)
		{
			rp.command = input_cmd;
		}
		else if (input_cmd == "&")
		{
			rp.background = true;
			OtherPart = true;
		}
		else if (input_cmd == "|")
		{
			rp.pipe_with_next = true;
			rp.background = true;
			OtherPart = true;
		}
		else if (input_cmd == "<")
		{
			IsRedirIn = true;
		}
		else if (input_cmd == ">")
		{
			IsRedirOut = true;
		}
		else if (input_cmd == ">>")
		{
			IsRedirAppend = true;
		}
		else
		{
			if (IsRedirIn)
			{
				rp.redirIn = input_cmd;
				IsRedirIn = false;
			}
			else if (IsRedirOut)
			{
				rp.redirOut = input_cmd;
				IsRedirOut = false;				
			}
			else if (IsRedirAppend)
			{
				rp.redirAppend = input_cmd;
				IsRedirAppend = false;				
			}
			else
				rp.arguments.push_back(input_cmd);
		}
		
		if (OtherPart)
		{
			r.parts.push_back(rp);
			rp = ParserResultPart();
			i = 0;
			continue;
		}
	}
	
	if (rp.command != "")
		r.parts.push_back(rp);
	
	return r;
}
