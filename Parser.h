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


#include <string>
#include <vector>

using namespace std;

/* Creates a structure to stores command and its information */
class ParserResultPart
{
	public:
	    string command;
	    vector<string> arguments;
	    string redirIn;
	    string redirOut;
	    string redirAppend;
	bool background;
	bool pipe_with_next;
	
	ParserResultPart()
	{
		background = false;
		pipe_with_next = false;
		redirIn = "";
		redirOut = "";
		redirAppend = "";
	}
	
	string getAbsolutePath(string searchPath);	
};

/* Structure for storing different commands */
class ParserResult
{
	public:
		vector<ParserResultPart> parts;
};

/* This class provides command parsing ability */
class Parser
{
	public:
	    static ParserResult parse(string command);
};

