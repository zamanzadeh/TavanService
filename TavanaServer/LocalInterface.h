#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WFileUpload>
#include <Wt/WString>
#include "TinyXML/tinyxml.h"
#include <Windows.h>
#include <iostream>
#include <string>

using namespace Wt;
using namespace std;

#define NUM_OF_MEMBERS 4
#define MAX_NUM_CHOICE 2
#define MAX_NUM_FACTORS 8

#pragma once

struct DTreeNode
{
	int NodeId;
	int ParentIdNode;
	//wstring QesNum;
	wstring m_Questxt;
	wstring m_Choices[MAX_NUM_CHOICE];
	wstring m_SumScore[MAX_NUM_CHOICE][MAX_NUM_FACTORS];
	string QIndex;
	wstring m_Result[MAX_NUM_CHOICE];

	DTreeNode()
	{
		NodeId = -1;
		ParentIdNode = 0;
		QIndex = "0";
		for(int i=0; i<MAX_NUM_CHOICE; i++)
			for(int j=0; j<MAX_NUM_FACTORS; j++)
				m_SumScore[i][j] = L"0";
	}

	~DTreeNode()
	{
		//delete[] m_ScoresofCoice;
	}
};

class LocalInterface : public WApplication
{
public:
	LocalInterface(const WEnvironment& env, std::vector<DTreeNode>*);
	std::vector<DTreeNode>*  _vec;
	LocalInterface& operator=(const LocalInterface& cSource);
	void OnUploadClicked();
	wofstream fout;
	wfstream fs;
	bool _newFiles;
	bool _from_Server;
	bool LoadFromXML( const wstring& a_strFile, std::vector<DTreeNode>* );
	void LoadTxtFile(const wstring& a_strFile , std::vector<DTreeNode>* _vector);

private:
	
	WContainerWidget *container;
	WPushButton *uploadButton;
	WFileUpload* fu[NUM_OF_MEMBERS];
	WText * out;
	const WEnvironment& env_;
	void Load( TiXmlNode* a_pNode, std::vector<DTreeNode>* _vector);
	void LoadItem( TiXmlNode* a_pNode, std::vector<DTreeNode>* _vector, int _pID );
	void LoadQues( TiXmlNode* a_pNode);
	
};

