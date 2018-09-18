#include "DatabaseManager.h"

using namespace std;

HDBC DatabaseManager::sqlconnectionhandle;
SQLHANDLE DatabaseManager::sqlenvhandle;
SQLHANDLE DatabaseManager::sqlstatementhandle;

DatabaseManager::DatabaseManager(void)
{
	DatabaseManager::sqlconnectionhandle = SQL_NULL_HDBC;
	SQLHANDLE sqlstatementhandle = SQL_NULL_HSTMT;
}

DatabaseManager::~DatabaseManager(void)
{
}

void DatabaseManager::show_error(unsigned int handletype, const SQLHANDLE& handle)
{
	SQLWCHAR sqlstate[1024];
	SQLWCHAR message[1024];
	if(SQL_SUCCESS == SQLGetDiagRecW(handletype, handle, 1, sqlstate, NULL, message, 1024, NULL))
		MessageBoxEx(NULL, message, sqlstate, MB_ICONINFORMATION, 0);
}

void DatabaseManager::GetConnected()
{

	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlenvhandle))
		return;
 
	if(SQL_SUCCESS!=SQLSetEnvAttr(sqlenvhandle,SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) 
		return;
 
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, &DatabaseManager::sqlconnectionhandle))
		return;
 
		
	SQLWCHAR retconstring[1024];
	retcode = SQLDriverConnectW (DatabaseManager::sqlconnectionhandle, NULL, 
			L"DRIVER={SQL Server Native Client 10.0};SERVER=localhost, 1433;DATABASE=mine;UID=zzz;PWD=zzz33523;", 
			SQL_NTS, retconstring, 1024, NULL,SQL_DRIVER_NOPROMPT);
	switch(retcode)
	{
		case SQL_SUCCESS_WITH_INFO:
			//show_error(SQL_HANDLE_DBC, DatabaseManager::sqlconnectionhandle);
			break;
		case SQL_INVALID_HANDLE:
		case SQL_ERROR:
			show_error(SQL_HANDLE_DBC, DatabaseManager::sqlconnectionhandle);
			retcode = -1;
			break;
		default:
			break;
	}
 
	if(retcode == -1)
		return;
 
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, DatabaseManager::sqlconnectionhandle, &sqlstatementhandle))
		return;

	SQLWCHAR commandline[] = 
		L"CREATE TABLE Tavana_Table(PhoneNumber VARCHAR(100), fTracking VARCHAR(50), mTracking VARCHAR(50), sTracking VARCHAR(50), dTracking VARCHAR(50), fQ VARCHAR(11), mQ VARCHAR(11), sQ VARCHAR(11), dQ VARCHAR(11), Score VARCHAR(1000))";
	if(SQL_SUCCESS != SQLExecDirect(sqlstatementhandle, commandline, SQL_NTS))
    {
		//show_error(SQL_HANDLE_STMT ,sqlstatementhandle);
		return ;
    }
}

void DatabaseManager::Disconnected()
{
	if (sqlstatementhandle != SQL_NULL_HSTMT)
        SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	if(sqlconnectionhandle!= SQL_NULL_HDBC)
	{
		SQLDisconnect(DatabaseManager::sqlconnectionhandle);
		SQLFreeHandle(SQL_HANDLE_DBC, DatabaseManager::sqlconnectionhandle);
	}
	if (sqlenvhandle != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);
}

string DatabaseManager::SearchUsr(string _usrnum, string _srch, bool Toins)
{
	SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
	char _resDB[150] = "\0";
	SQLLEN lenresDB=0;
	char SQLStmt[1024] = {0};

	sprintf(SQLStmt, 
		"SELECT %s FROM mine.dbo.Tavana_Table WHERE PhoneNumber='%s'",
		_srch , _usrnum.c_str());
	
	// execute query
	retcode = SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)SQLStmt, SQL_NTS);
	//cout << "Select Exec: " << retcode << endl;
	show_error(SQL_HANDLE_STMT , sqlstatementhandle);

	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(sqlstatementhandle);
		//cout << "Select Fetch: " << retcode << endl;
		if(retcode == SQL_NO_DATA)
		{
			if(_srch.compare("Score")==0)
			{
				SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
				return "False";
			}
			else 
			{
				if(Toins) 
				{
					cout << "insert new" << endl;
					if(InsertNumber(_usrnum))
						strcpy(_resDB, "0");	
					else
						strcpy(_resDB, "Error! Cannot add usr");
				}
				else
				{
					strcpy(_resDB, "You are not a member of this Game!");
				}
			}
		}
		else
		{
			retcode = SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, (SQLCHAR*)_resDB, 150, NULL);
			//cout << "get data!!!!!!"<<endl;
			show_error(SQL_HANDLE_STMT , sqlstatementhandle);
		}
	}
	else
	{
		show_error(SQL_HANDLE_STMT , sqlstatementhandle);
	}
	SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
	return _resDB;
}

bool DatabaseManager::UpdateUsrState(string _usrnum, string _srch, string _upd, wstring _score, string _tracking)
{
	//retcode = SQLCloseCursor(sqlstatementhandle);
	SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
	string _s = SearchUsr(_usrnum, "Score", false);
	std::wstring _lastscore(_s.begin(), _s.end());
	wstring delimiter = L",";
	size_t pos1 = 0;
	size_t pos2 = 0;
	wstring token1, token2, _New_Score;
	while ((pos1 = _lastscore.find(delimiter)) != wstring::npos 
		&& (pos2 = _score.find(delimiter)) != wstring::npos) 
	{
		token1 = _lastscore.substr(0, pos1);
		token2 = _score.substr(0, pos2);
		_lastscore.erase(0, pos1 + delimiter.length());
		_score.erase(0, pos2 + delimiter.length());
		_New_Score += to_wstring( stoi(token1) + stoi(token2) );
		_New_Score.append(L",");
	}
	_New_Score += to_wstring( stoi(_score) + stoi(_lastscore) );
	char SQLStmt[2000] = {0};
	std::string strScore( _New_Score.begin(), _New_Score.end() );

	string _tr;
	if(_srch == "fQ")
		_tr = "fTracking";
	else if(_srch == "mQ")
		_tr = "mTracking";
	else if(_srch == "sQ")
		_tr = "sTracking";
	else
		_tr = "dTracking";

	sprintf(SQLStmt, "UPDATE mine.dbo.Tavana_Table SET %s='%s', Score='%s', %s='%s' WHERE PhoneNumber='%s'",
		_srch, _upd.c_str(), strScore.c_str(), _tr, _tracking.c_str(), _usrnum.c_str());
	retcode = SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)SQLStmt, SQL_NTS);
	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		return TRUE;
	}
	else
	{
		show_error(SQL_HANDLE_STMT , sqlstatementhandle);
		return FALSE;
	}
}

bool DatabaseManager::InsertNumber(string _num)
{
	//retcode = SQLCloseCursor(sqlstatementhandle);
	SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
	char SQLStmt[1024] = {0};
	sprintf(SQLStmt, 
	"INSERT INTO mine.dbo.Tavana_Table (PhoneNumber, fTracking, mTracking, sTracking, dTracking,fQ, mQ, sQ, dQ, Score) VALUES ('%s', '%s','%s','%s','%s','%s','%s','%s','%s','%s')" ,
		_num.c_str(), "0", "0", "0", "0", "0", "0", "0", "0", "0,0,0,0,0,0,0,0");
	retcode = SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)SQLStmt, SQL_NTS);	
	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		//"Insert Success!" ;
		SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
		return TRUE;
	}
	else
	{
		show_error(SQL_HANDLE_STMT , sqlstatementhandle);
		SQLFreeStmt(sqlstatementhandle, SQL_CLOSE);
		return FALSE;
	}
}