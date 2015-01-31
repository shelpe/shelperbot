using namespace std;

string fixString( string Input, string Problem, string Solution) // shitty implementation to get rid of unicode and html
{
	for ( bool IsFixed = false; IsFixed == false; IsFixed)
	{
		if ( Input.find( Problem, 0) != -1)
		{
			Input.replace( Input.find( Problem, 0), Problem.size(), Solution);
		}
		else
		{
			IsFixed = true;
		}
	}
	return Input;
}

string grabFourChanPostText( struct fourchanpost_t PostInfo) // takes structured post info, builds a URL and returns a string of what the post contents are
{
	
	string BoardLetter = PostInfo.BoardLetter;
	string ThreadNumber = PostInfo.ThreadNumber;
	string PostNumber = PostInfo.PostNumber;
	bool IsHttps = PostInfo.IsHttps;

	string FourChanJsonIn_Temp = "http";
	if ( IsHttps == true)
	{
		FourChanJsonIn_Temp.append( "");
	}
	FourChanJsonIn_Temp.append( "://a.4cdn.org");
	FourChanJsonIn_Temp.append( BoardLetter);
	FourChanJsonIn_Temp.append( "thread/");
	FourChanJsonIn_Temp.append( ThreadNumber);
	FourChanJsonIn_Temp.append( ".json");
	char* FourChanJsonIn = ( char*) FourChanJsonIn_Temp.c_str();
	cout << "Grabbing " << FourChanJsonIn << " ...\n";

	string FourChanJsonString = getWebPage( FourChanJsonIn);
	
	if ( FourChanJsonString != "\r\b\r")
	{
		string PostNumberJson = FourChanJsonString;
		string PostNumberToFind = "\"no\":";
		PostNumberToFind.append( PostNumber);
		if ( PostNumberJson.find( PostNumberToFind, 0) != -1)
		{
			PostNumberJson.erase( 0, PostNumberJson.find( PostNumberToFind, 0));
		}
		if ( PostNumberJson.find( "\"com\":", 0) != -1)
		{
			PostNumberJson.erase( 0, PostNumberJson.find( "\"com\":", 0) + 7);
		}
		if ( PostNumberJson.find( "},{\"no\":", 0) != -1)
		{
			PostNumberJson.erase( PostNumberJson.find( "},{\"no\":", 0), PostNumberJson.size() - 1);
		}
		if ( PostNumberJson.find( ",\"filename\":", 0) != -1)
		{
			 PostNumberJson.erase( PostNumberJson.find( ",\"filename\":", 0) - 1, PostNumberJson.size() - 1);
		}
		else if ( PostNumberJson.find( ",\"time\":", 0) != -1)
		{
			PostNumberJson.erase( PostNumberJson.find( ",\"time\":", 0) - 1, PostNumberJson.size() - 1);
		}
	//	cout << PostNumberJson << endl;

		string Json2Irc = ">>>";
		Json2Irc.append( BoardLetter);
		Json2Irc.append( PostNumber);
		Json2Irc.append( " ---> ");
		Json2Irc.append( PostNumberJson);
	//	cout << Json2Irc << endl << endl;

		Json2Irc = fixString( Json2Irc, "\\/", "/");
		Json2Irc = fixString( Json2Irc, "\\\"", "\"");
		Json2Irc = fixString( Json2Irc, "<br>", "   ");
		Json2Irc = fixString( Json2Irc, "&#039;", "\'");
		Json2Irc = fixString( Json2Irc, "&gt;", ">");
	//	cout << Json2Irc << endl << endl;

		for ( bool IsFixed = false; IsFixed == false; IsFixed)
		{
			if ( ( Json2Irc.find( "<a href=", 0) != -1) && ( Json2Irc.find( "\" class=\"quotelink\">", 0) != -1))
			{
			//	cout << Json2Irc << endl;
				Json2Irc.erase( Json2Irc.begin() + Json2Irc.find( "<a href=", 0), Json2Irc.begin() + Json2Irc.find( "\" class=\"quotelink\">", 0) + 20);
			}
			else
			{
				IsFixed = true;
			}
		}
		/*
		 * this is what happens when you don't have a parser for everything
		 */
		Json2Irc = fixString( Json2Irc, "</a>", "");
		Json2Irc = fixString( Json2Irc, "<a href=\"#p", ">>");

		Json2Irc = fixString( Json2Irc, "<pre class=\"prettyprint\">", "[code]");
		Json2Irc = fixString( Json2Irc, "</pre>", "[/code]");
		Json2Irc = fixString( Json2Irc, "<s>", "");
		Json2Irc = fixString( Json2Irc, "</s>", "");
		Json2Irc = fixString( Json2Irc, "<wbr>", "");
		Json2Irc = fixString( Json2Irc, "<a href=\"", "");
		Json2Irc = fixString( Json2Irc, "<span class=\"quote\">", "");
		Json2Irc = fixString( Json2Irc, "</span>", "");

		Json2Irc = fixString( Json2Irc, "&quot;", "\"");
		Json2Irc = fixString( Json2Irc, "&amp;", "&");
		
		return Json2Irc;	
	}
	else
	{
		return "\r\b\r"; // should be unique enough to never show up in the socket stream
	}
}
