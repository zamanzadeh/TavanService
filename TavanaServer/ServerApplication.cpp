/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include <io.h>
#include <iostream>
#include <Wt/WString>
#include <Wt/WServer>
#include <Wt/WResource>
#include "DatabaseManager.h"
#include "LocalInterface.h"
#include "MyServer.h"

using namespace Wt;
using namespace std;

LocalInterface* createApplication(const WEnvironment& env, std::vector<DTreeNode>* _Game, LocalInterface *_tmp)
{
//  /*
//   * You could read information from the environment to decide whether
//   * the user has permission to start a new application
//   */
	//LocalInterface* _mine = new LocalInterface(env, _Game);
	_tmp = new LocalInterface(env, _Game);
	//_tmp = *_mine;
	return _tmp;
}


int main(int argc, char **argv)
{
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */

	try
	{
		DatabaseManager db;
		db.GetConnected();
		Wt::WServer server(argv[0]);
		server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION); 
		
		std::vector<DTreeNode>* _Server_Game = new std::vector<DTreeNode>[NUM_OF_MEMBERS];
		LocalInterface* _SrvInterface;
		//server.addEntryPoint(Application, 
		//	boost::bind(&createApplication, _1, _Server_Game, _SrvInterface), "/upload");

		////load files to vector, read path of file from address.txt file
		//std::wfstream _g_files;
		//_g_files.open ("address.txt", std::fstream::in);
		//if(_g_files.is_open())
		//{
		//	for(int i =0; i<NUM_OF_MEMBERS; i++)
		//	{
		//		wstring _g_path;
		//		getline(_g_files, _g_path);
		//		std::ifstream ifs (_g_path);
		//		if(ifs.is_open())
		//		{
		//			_SrvInterface->_from_Server = TRUE;
		//			_SrvInterface->LoadFromXML(_g_path, &_Server_Game[i]);
		//		}
		//	}
		//}
		wstring _files[4];
		_files[0] = L"inputs\\tavana.txt";
		_files[1] = L"inputs\\elahe.txt";
		_files[2] = L"inputs\\arman.txt";
		_files[3] = L"inputs\\arezoo.txt";
		for(int i =0; i<NUM_OF_MEMBERS; i++)
		{
			_SrvInterface->_from_Server = TRUE;
			_SrvInterface->LoadFromXML(_files[i], &_Server_Game[i]);
		}

		MyServer* _runSrv = new MyServer(db, _Server_Game);
		server.addResource(_runSrv, "/tavana");

		if (server.start())
		{
			Wt::WServer::waitForShutdown();
			db.Disconnected();
			server.stop();
		}
	}
    catch (WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
	getchar();
}
