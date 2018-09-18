#include <Wt/WString>
#include <Wt/WServer>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WApplication>

using namespace Wt;
using namespace std;
#define NUM_OF_MEMBERS 4

#include "DatabaseManager.h"
#include "LocalInterface.h"

#pragma once
class MyServer : public WResource
{
public:
	MyServer(DatabaseManager _db, std::vector<DTreeNode>* _li);
	void handleRequest(const Wt::Http::Request& request, Wt::Http::Response&response);
	wstring checkRequest(string _num ,string _req);
	std::vector<DTreeNode>* _game;

private:
	wstring _ip_client;
	DatabaseManager _SrvDB;	
	std::vector<int> ScoreArr;
	int GetIndexQuestion(string _qindex, std::vector<DTreeNode> _vector);
	wstring GetQuestionText(int index, std::vector<DTreeNode> _vector, int _assign);
	wstring SendResponse(string _num , string _req, string _srch, int _assign, bool);
	vector<int> GetDetailScore(string _num);
};

