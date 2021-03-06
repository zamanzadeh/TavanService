#include "MyServer.h"
#include <codecvt>
MyServer::MyServer(DatabaseManager _db, std::vector<DTreeNode>* _li)
{
	_SrvDB = _db;
	_game = _li;
}

void MyServer::handleRequest(const Wt::Http::Request& request, Wt::Http::Response&response)
{
	//_ip_client =  request.clientAddress();
	//if(_ip_client == "127.0.0.1")

	if(!request.getParameterValues("m").empty())
	{
		Wt::Http::ParameterValues wnum = request.getParameterValues("m");
		string _wnum = wnum.at(0);
		Wt::Http::ParameterValues wreq = request.getParameterValues("req");
		if(!wreq.empty())
		{
			string _wreq = wreq.at(0);
			//response.out() << _wnum << _wreq << endl;
			//if the user have request to start first check in db. if the req is for first time, insert it
			//Wt::WString::setDefaultEncoding(Wt::UTF8);
			//setlocale(LC_ALL, "en_US");
			string utf8;
			WString _res = checkRequest(_wnum, _wreq);
			response.out() <<_res.toUTF8();
		}
	}
	else
	{
		response.out() << "Invalid Request";
	}
}

vector<int> MyServer::GetDetailScore(string _num)
{
	std::vector<int> outputArr;
	string _s = _SrvDB.SearchUsr(_num, "Score", false);
	if(!_s.compare("False"))
	{
		outputArr.push_back(0);
	}
	else
	{
		std::wstring _score(_s.begin(), _s.end());
		wstring delimiter = L",";
		size_t pos = 0;
		wstring token;
		while ((pos = _score.find(delimiter)) != wstring::npos) {
			token = _score.substr(0, pos);
			_score.erase(0, pos + delimiter.length());
			outputArr.push_back(stoi(token));
		}
		if(_score.compare(L"") == 0)
			outputArr.push_back(stoi(L"0"));
		else
			outputArr.push_back(stoi(_score));
	}
	return outputArr;
}

wstring MyServer::checkRequest(string _num ,string _req)
{
	wstring _response;
	wstring _footer;
	//_footer += L"عدد گزینه رو به همین شماره بفرست";
	_footer += L"\nلغو = L";
	_footer += L"\nهر سوال 100 تومان ";
	_footer += L"\nپشتیبان تیم سیب 88542837";
	_footer += L"\n**";
	_footer += 
		L"\nاطلاع از وضعیت امتیازی خانواده با ارسال s";


	if(!_num.empty())
	{
	try
	{
		//if(_num.size() == 15)
		//{
			if(_req == "S" || _req == "s")
			{
				ScoreArr = GetDetailScore(_num);
				int _sum=0;
				for(int k=0; k<ScoreArr.size(); k++)
					_sum += ScoreArr.at(k);
				_response += L"امتیاز کل: " + to_wstring(_sum);
				if(ScoreArr.size() == MAX_NUM_FACTORS)
				{
					_response += L"\nامتیاز تحکیم خانواده: ";
					_response += to_wstring(ScoreArr.at(0));
					_response += L"\nامتیاز خوش شانسی: ";
					_response += to_wstring(ScoreArr.at(1));
					_response += L"\nامتیاز موقعیت تحصیلی: ";
					_response += to_wstring(ScoreArr.at(2));
					_response += L"\nامتیاز ثروت: ";
					_response += to_wstring(ScoreArr.at(3));
					_response += L"\nامتیاز سلامت: ";
					_response += to_wstring(ScoreArr.at(4));
					_response += L"\nامتیاز موقعیت اجتماعی: ";
					_response += to_wstring(ScoreArr.at(5));
					_response += L"\nامتیاز موقعیت شغلی: ";
					_response += to_wstring(ScoreArr.at(6));
					_response += L"\nامتیاز آینده نگری: ";
					_response += to_wstring(ScoreArr.at(7));
					_response += L"\nبرای لغو L بفرست";
					_response += L"\nهر سوال 100 تومان ";
					_response += L"\nپشتیبان تیم سیب 88542837";
				}
			}
			else if(_req == "fQ" || _req == "mQ" || _req == "sQ" || _req == "dQ")
			{
				ScoreArr = GetDetailScore(_num);
				int _sum=0;
				for(int k=0; k<ScoreArr.size(); k++)
					_sum += ScoreArr.at(k);
				
				string _s = _SrvDB.SearchUsr(_num, _req, true);
				//std::wstring wsTmp(_s.begin(), _s.end());
				//wstring _question = wsTmp;
				//wstring _qnum;
				int _member=0;
				if(_req.compare("fQ") == 0 )
					_member = 0;
				else if(_req.compare("mQ") == 0)
					_member = 1;
				else if(_req.compare("sQ") == 0)
					_member = 2;
				else if(_req.compare("dQ") == 0)
					_member = 3;

				//check vector not null!!!!!!!
				if(_game[_member].empty())
				{

				}
				//if(!_game[_member].empty())
				//{
					int _index = GetIndexQuestion(_s, _game[_member]);
					if(_index == -1)
					{
						//_response += L"مسابقه تمام شده است\n";
						//_response += checkRequest(_num , "S");
						return L"";
					}
					//else if(_index == -2)
					//{
					//	return L"\nمسابقه وجود ندارد\n";
					//}
					//_qnum.append(_game[_member].at(_wtoi(_question.c_str())).QesNum);
					else if(GetQuestionText(_index, _game[_member], _member+1).compare(L"Info") == 0)
					{
						_response = L"برای دریافت سوال بعدی، ";
						_response += to_wstring(_member*10+1);
						_response += L" رو ارسال کن\n";
						_response += checkRequest(_num , "S");
					}
					else
					{
						_response += L"امتیاز کل: " + to_wstring(_sum);
						_response += GetQuestionText(_index , _game[_member], _member+1);
						_response += _footer;
					}
					
				//}
				//else
				//{
				//	_response = L"No Vector";
				//}
			}
			else if(_req == "11" || _req == "12" || _req == "13" || _req == "14")
			{
				wstring _tmpRes; 
				if( (_tmpRes = SendResponse(_num, _req, "fQ", 1, false)).compare(L"Info") == 0 )
				{
					_response = L"برای سوال بعدی، 11 رو ارسال کن\n";
					_response += checkRequest(_num , "S");
				}
				else
				{
					int _sum=0;
					for(int k=0; k<ScoreArr.size(); k++)
						_sum += ScoreArr.at(k);
					_response += L"امتیاز کل: " + to_wstring(_sum);
					_response += _tmpRes  + _footer;
				}
			}
			else if(_req == "21" || _req == "22" || _req == "23" || _req == "24")
			{
				wstring _tmpRes;
				if( (_tmpRes = SendResponse(_num, _req, "mQ", 2, false)).compare(L"Info") == 0 )
				{
					_response = L"برای سوال بعدی، 21 رو ارسال کن\n";
					_response += checkRequest(_num , "S");
				}
				else
				{
					int _sum=0;
					for(int k=0; k<ScoreArr.size(); k++)
						_sum += ScoreArr.at(k);
					_response += L"امتیاز کل: " + to_wstring(_sum);
					_response += _tmpRes  + _footer;
				}
			}
			else if(_req == "31" || _req == "32" || _req == "33" || _req == "34")
			{
				wstring _tmpRes;
				if( (_tmpRes = SendResponse(_num, _req, "sQ", 3, false)).compare(L"Info") == 0 )
				{
					_response = L"برای سوال بعدی، 31 رو ارسال کن\n";
					_response += checkRequest(_num , "S");
				}
				else
				{
					int _sum=0;
					for(int k=0; k<ScoreArr.size(); k++)
						_sum += ScoreArr.at(k);
					_response += L"امتیاز کل: " + to_wstring(_sum);
					_response += _tmpRes + _footer;
				}
			}
			else if(_req == "41" || _req == "42" || _req == "43" || _req == "44")
			{
				wstring _tmpRes;
				if( (_tmpRes = SendResponse(_num, _req, "dQ", 4, false)).compare(L"Info") == 0 )
				{
					_response = L"برای سوال بعدی، 41 رو ارسال کن\n";
					_response += checkRequest(_num , "S");
				}
				else
				{
					int _sum=0;
					for(int k=0; k<ScoreArr.size(); k++)
						_sum += ScoreArr.at(k);
					_response += L"امتیاز کل: " + to_wstring(_sum);
					_response += _tmpRes + _footer;
				}
			}
			}
			catch(...)
			{
			
				_response = L"\nکد درخواست وارد شده اشتباه است\n";
				throw;
			}
		//}
		//else
		//	_response = L"Invalid PhoneNumber";
	}
	else
		_response = L"Empty Number";


	return _response;
}

int MyServer::GetIndexQuestion(string _qindex, std::vector<DTreeNode> _vector)
{
	unsigned int index=0;
	for(index= 0; index < _vector.size(); index++)
	{
		if(_qindex.compare(_vector.at(index).QIndex) == 0)
			return index;
	}
	return -1;
}

wstring MyServer::GetQuestionText(int index,  std::vector<DTreeNode> _vector, int _assign)
{
	wstring _textmsg;
	_textmsg += L"\n";

	if(_vector.at(index).m_Questxt.compare(L"info") == 0 )
	{
		return L"Info";
	}
	else
	{
		if(_assign == 1)
			_textmsg += L"آقای توانا-";
		else if(_assign == 2)
			_textmsg += L"الهه خانم-";
		else if(_assign == 3)
			_textmsg += L"آرمان-";
		else
			_textmsg += L"آرزو-";

		int _sh = (index/2)+1;
		_textmsg += L"سوال " + to_wstring(_sh);
		_textmsg += L"\n";
		_textmsg += _vector.at(index).m_Questxt;
		_textmsg += L"\n";
		for(int i=0; i<MAX_NUM_CHOICE; i++)
		{
			if(!_vector.at(index).m_Choices[i].empty())
			{
				_textmsg+= std::to_wstring(_assign*10+(i+1));
				_textmsg += L". ";
				_textmsg+= _vector.at(index).m_Choices[i];
				_textmsg+= L"\n";
			}
		}
		_textmsg += L"**\n";
		_textmsg += L"عدد گزینه رو بفرست";
	}
	return _textmsg;
}

wstring MyServer::SendResponse(string _num , string _req, string _srch, int _assign, bool Toins)
{
	int _ch = atoi(_req.c_str());		//last q
	int _LastQues[4];
	_LastQues[0] = atoi(_SrvDB.SearchUsr(_num, "fQ", Toins).c_str());
	_LastQues[1] = atoi(_SrvDB.SearchUsr(_num, "mQ", Toins).c_str());
	_LastQues[2] = atoi(_SrvDB.SearchUsr(_num, "sQ", Toins).c_str());
	_LastQues[3] = atoi(_SrvDB.SearchUsr(_num, "dQ", Toins).c_str());

	//std::wstring _question(_s.begin(), _s.end());
	int _member = (_ch / 10) - 1;
	int _child = _ch % 10;
	string _qnum;

	int minQ = min(_LastQues[0], min(_LastQues[1], min(_LastQues[2], _LastQues[3])));
	if( (_LastQues[_assign-1] == minQ) || (_LastQues[_assign-1] == minQ+1) )
	{
		//get next question text and choices
		int _index = GetIndexQuestion(to_string(_LastQues[_assign-1]), _game[_member]);
		if(_index == -1)
		{
			return L"\nمسابقه تمام شده است\n";
		}
		//else if(_index == -2)
		//{
		//	return L"\nمسابقه وجود ندارد\n";
		//}
		else if(_game->at(_index).m_Choices[_child-1].empty() || _child > MAX_NUM_CHOICE)
		{
			return L"\nگزینه ارسال شده، معتبر نیست\n";
		}
		//else if(_game->at(_index).QesNum == L"0")
		//{
		//	_qnum = std::to_string(_child);
		//}
		else
		{
			//_qnum = to_string(atoi(_question.c_str()) * MAX_NUM_CHOICE + _child);  /*tree format*/
			int _prev = _LastQues[_assign-1];
			if(_prev == 0)
				_qnum = to_string(_child);
			else
				_qnum = to_string(((_prev-1)/2 + 1) * MAX_NUM_CHOICE + _child);
			int next_index = GetIndexQuestion(_qnum, _game[_member]);
			wstring _New_Score;
			for(int i=0; i<MAX_NUM_FACTORS; i++)
			{
				_New_Score += 
					_game[_member].at(_index).m_SumScore[_child-1][i];
				_New_Score.append(L",");
			}
			_New_Score.pop_back();
			string _tr;
			if(_srch == "fQ")
				_tr = "fTracking";
			else if(_srch == "mQ")
				_tr = "mTracking";
			else if(_srch == "sQ")
				_tr = "sTracking";
			else
				_tr = "dTracking";

			//update last q in db
			string _tracking = _SrvDB.SearchUsr(_num, _tr, false);
			if(_tracking == L"0")
				_tracking = to_string(_child);
			else
				_tracking += to_string(_child);
			_SrvDB.UpdateUsrState(_num, _srch, _qnum, _New_Score, _tracking);
			ScoreArr = GetDetailScore(_num);

			if(next_index != -1)
			{
				return GetQuestionText(next_index, _game[_member], _assign);
			}
			else
			{
				return L"\nمسابقه تموم شد.\n";
			}
		}
	}
	else
	{
		return L"\nپیش از پاسخ به این سوال، باید به سوالات اعضای دیگر خانواده جواب دهید.\n";
	}
}

