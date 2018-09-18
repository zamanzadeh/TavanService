#include <iostream>
#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <string>

using namespace std;

#pragma comment(lib, "odbc32.lib")

#pragma once

class DatabaseManager
{
public:
	DatabaseManager(void);
	~DatabaseManager(void);
	void show_error(unsigned int handletype, const SQLHANDLE& handle);
	void GetConnected();
	void Disconnected();
	bool UpdateUsrState(string _usr, string _srch, string _upd, wstring _score, string _tr);
	string SearchUsr(string _usr, string _srch, bool);
	static HDBC sqlconnectionhandle;
	static SQLHANDLE sqlenvhandle;
	static SQLHANDLE sqlstatementhandle;
private:    
	bool InsertNumber(string _num);
	SQLRETURN retcode;
};

