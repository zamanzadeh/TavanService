#include "LocalInterface.h"
#include <locale>
#include <codecvt>

string ConvertUnicodeToUTF8(const wstring wstr)
{
	if(wstr.length()>0)
	{
		std::string str( wstr.begin(), wstr.end() );
		return str;
	}
	else
		return "";
}

wstring ConvertUTF8ToUnicodeStr(const string str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
LocalInterface::LocalInterface(const WEnvironment& env, std::vector<DTreeNode>* _game) 
	: WApplication(env), env_(env), fout(wofstream("D:\\output.txt",ios::out))
{
	_vec = new std::vector<DTreeNode>[NUM_OF_MEMBERS];
	_vec = _game;
	_newFiles = FALSE;
	_from_Server = FALSE;
	setTitle(L"مسابقه خانواده توانا");                               // application title

	container = new Wt::WContainerWidget(root());
	container->setMargin(100, Wt::Left | Wt::Top);

	for(int i=0; i<NUM_OF_MEMBERS; i++)
	{
		fu[i] = new Wt::WFileUpload(container);
		fu[i]->setMargin(100, Wt::Left | Wt::Bottom);
		fu[i]->setFileTextSize(50);
		container->addWidget(new WBreak());
		container->addWidget(new WBreak());
	}
	// Provide a button to start uploading.
	uploadButton = new Wt::WPushButton(L"بارگزاری فایل ها", container);
	uploadButton->setMargin(100, Wt::Left);
	uploadButton->setMargin(30, Wt::Top);
	out = new WText(container);
	uploadButton->clicked().connect(this, &LocalInterface::OnUploadClicked);
}

void LocalInterface::OnUploadClicked()
{
	_newFiles = TRUE;

	for(int i=0; i<NUM_OF_MEMBERS; i++)
	{
		fu[i]->upload();
		// React to a file upload problem.
		WText* out1 = new WText(container);
		fu[i]->fileTooLarge().connect(std::bind([=] () {
			out1->setText("File is too large.\n");
		}));
			
		fu[i]->uploaded().connect(std::bind([=] () {
			LoadFromXML(ConvertUTF8ToUnicodeStr(fu[i]->spoolFileName()), &_vec[i]);
		}));
	}
	out->setText("\n File Uploaded Properly.");
	
}

void LocalInterface::LoadTxtFile(const wstring& a_strFile , std::vector<DTreeNode>* _vector)
{
	DTreeNode _fDTNode;
	_fDTNode.NodeId = 0;
	_fDTNode.ParentIdNode = -1;
	_fDTNode.m_Questxt = L"مسابقه توانا";
	_fDTNode.QIndex = "-1";
	_vector->push_back(_fDTNode);

	wifstream fin(a_strFile);
	bool _fof = true;
	while(!fin.eof())
	{
		wstring ss = L"";
		getline(fin, ss);
		if(_fof)
		{
			ss = ss.substr(3);
			_fof = false;
		}
		int _line = fin.tellg();
		const std::locale empty_locale = std::locale::empty();
		typedef std::codecvt_utf8<wchar_t> converter_type;
		const converter_type* converter = new converter_type;
		const std::locale utf8_locale = std::locale(empty_locale, converter);
		fin.imbue(utf8_locale);
		int _q_num = _wtoi(ss.c_str());
		_q_num--;
		int _itr;
		if(_q_num == 0 )
			_itr = 1;
		else
			_itr = 2;

		for(int _lvl=0; _lvl<_itr/*pow(2, _q_num)*/; _lvl++)
		{
			fin.seekg(_line);

			DTreeNode DTNode;
			DTNode.NodeId = _vector->size();
			(DTNode.NodeId-2 > 0 ? DTNode.ParentIdNode = DTNode.NodeId-2 : DTNode.ParentIdNode=DTNode.NodeId-1);
			
			if(_vector->size() > 1) 
			{
				DTNode.QIndex = to_string(DTNode.NodeId - 1 );
				DTNode.m_Questxt = _vector->at(DTNode.ParentIdNode).m_Result[_lvl];
			}
			else
			{
				DTNode.QIndex = "0";
			}
			getline(fin, ss);
			DTNode.m_Questxt += L"\n";
			DTNode.m_Questxt += ss.c_str();

			for(int i=0; i<MAX_NUM_CHOICE; i++)
			{
				getline(fin, ss);
				DTNode.m_Choices[i] = ss.c_str();
				getline(fin, ss);
				DTNode.m_Result[i] = ss.c_str();
				for(int j=0; j<MAX_NUM_FACTORS; j++)
				{
					getline(fin, ss);
					DTNode.m_SumScore[i][j] = ss.c_str();
				}
			}

			_vector->insert(_vector->begin()+DTNode.NodeId, DTNode);
		}
	}
}

bool LocalInterface::LoadFromXML( const wstring& a_strFile , std::vector<DTreeNode>* _vector)
{
	TiXmlNode* pXML = NULL;
	string strTemp = ConvertUnicodeToUTF8(a_strFile);
	if(!_from_Server)
	{
		if(!_newFiles)
			fs.open ("address.txt", std::fstream::out| std::fstream::app);
		else
		{
			fs.open ("address.txt", std::fstream::out | std::ifstream::trunc);
			fs.clear();
			_newFiles = FALSE;
		}
		fs << a_strFile << endl;
		fs.close();
	}

	LoadTxtFile(a_strFile, _vector);
	return true;
	//TiXmlDocument xmlDoc(strTemp.c_str());
	//if( xmlDoc.LoadFile(TIXML_ENCODING_UTF8) )
	//{
	//	// XML root
	//	pXML = xmlDoc.FirstChild( "XML" );
	//	if( NULL == pXML )	return false;
	//	// Load our tree control
	//	Load( pXML, _vector );

	//	return true;
	//}	  
	//return false;
}

void LocalInterface::Load( TiXmlNode* a_pNode, std::vector<DTreeNode>* _vector)
{
	// Get node "Items"
	TiXmlNode* pItems = a_pNode->FirstChild( "Questions" );
	TiXmlNode* pItem = NULL;
	if( NULL == pItems )
		return;

	_vector->clear();
	TiXmlElement* pEl = pItems->ToElement();
	
	DTreeNode DTNode;
	DTNode.NodeId = 0;
	DTNode.ParentIdNode = -1;
	
	const char* _tmpchar = pEl->Attribute("Title");
	std::string new_string(_tmpchar);
	wstring _title = ConvertUTF8ToUnicodeStr(new_string);
	DTNode.m_Questxt = _title;
	//DTNode.QesNum = _title;	
	DTNode.QIndex = "-1";
	_vector->push_back(DTNode);
	
	// Get first item
	pItem = pItems->FirstChild( "Question" );

	// Iterate all siblings
	while( NULL != pItem )
	{
		LoadItem( pItem, _vector, 0);
		pItem = pItem->NextSibling( "Question" );
	}
}

void LocalInterface::LoadItem( TiXmlNode* a_pNode, std::vector<DTreeNode>* _vector, int _pID)
{
	TiXmlElement* pEl = a_pNode->ToElement();
	TiXmlNode* pChild = NULL;

	DTreeNode DTNode;
	DTNode.NodeId = _vector->size();
	DTNode.ParentIdNode = _pID;
	//const char* _tmpchar = pEl->Attribute( "Num" );
	//std::string new_string(_tmpchar);
	//DTNode.QesNum = ConvertUTF8ToUnicodeStr(new_string);
	DTNode.QIndex = pEl->Attribute( "QIndex" );

	if(pEl->GetText() != NULL)
	{
		const char* _tmptxt = pEl->GetText();
		std::string new_text(_tmptxt);
		wstring _qtxt = ConvertUTF8ToUnicodeStr(new_text);
		//fout << DTNode.QIndex << ": " << _qtxt << endl;
		DTNode.m_Questxt = _qtxt;
		TiXmlNode* nChild = pEl->IterateChildren( "Choice", NULL );
		int _ctr=0;
		while (nChild)
		{
			TiXmlElement* chEl = nChild->ToElement();
			const char* _tmpch = chEl->Attribute( "ch" );
			std::string new_ch(_tmpch);
			DTNode.m_Choices[_ctr] = ConvertUTF8ToUnicodeStr(new_ch);
			//fout << " choice" << _ctr << ": " << DTNode.m_Choices[_ctr] << endl;
			string _name = "sc";
			for(int r=0; r<MAX_NUM_FACTORS; r++)
			{	
				_name = "sc";
				_name.append(to_string(r));
				//fout << _name << ":";
				const char* _tmpfactor = chEl->Attribute(_name.c_str());
				std::string new_factor(_tmpfactor);
				DTNode.m_SumScore[_ctr][r] = ConvertUTF8ToUnicodeStr(new_factor);
				//fout << DTNode.m_SumScore[_ctr][r] << endl;
			}
			_ctr++;
			nChild = pEl->IterateChildren( "Choice", nChild );
		}
	}

	_vector->insert(_vector->begin()+DTNode.NodeId, DTNode);
	
	pChild = a_pNode->IterateChildren( "Question", NULL );
	while( pChild )
	{
		LoadItem( pChild, _vector, DTNode.NodeId);
		pChild = a_pNode->IterateChildren( "Question", pChild );
	}
}

LocalInterface& LocalInterface::operator=(const LocalInterface& cSource)
{
    // check for self-assignment
    if (this == &cSource)
        return *this;

    // because m_nLength is not a pointer, we can shallow copy it
	_vec = new std::vector<DTreeNode>[NUM_OF_MEMBERS];
	if (cSource._vec)
	{
		for(int i=0; i<NUM_OF_MEMBERS; i++)
			copy(cSource._vec[i].begin(), cSource._vec[i].end(), _vec[i].begin());
	}
 
    return *this;
}