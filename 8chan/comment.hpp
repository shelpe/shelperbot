/*
 * BROKEN
 * DOES NOT WORK
 * NOT YET
 */
using namespace std;

string grabEightChanPostText( struct eightchanpost_t PostInfo)
{
	
	string BoardLetter = PostInfo.BoardLetter;
	string ThreadNumber = PostInfo.ThreadNumber;
	string PostNumber = PostInfo.PostNumber;
	bool IsHttps = PostInfo.IsHttps;

	string EightChanJsonIn_Temp = "https";
	if ( IsHttps == true)
	{
		EightChanJsonIn_Temp.append( "");
	}
	EightChanJsonIn_Temp.append( "://8chan.co");
	EightChanJsonIn_Temp.append( BoardLetter);
	EightChanJsonIn_Temp.append( "res/");
	EightChanJsonIn_Temp.append( ThreadNumber);
	EightChanJsonIn_Temp.append( ".json");
	char* EightChanJsonIn = ( char*) EightChanJsonIn_Temp.c_str();
	cout << "Grabbing " << EightChanJsonIn << " ...\n";

	string EightChanJsonString = getWebPage( EightChanJsonIn);
	
	if ( EightChanJsonString != "\r\b\r")
	{
		string PostNumberJson;
		struct JsonTokenListInfo ThreadTokenInfo = getJsonTokenInfo( (char* ) EightChanJsonString.c_str(), 25000);
		jsmntok_t* JsonTokens = ThreadTokenInfo.Tokens;
		int JsonTokenCount = ThreadTokenInfo.Size;
		int i = 0;	
		for ( bool IsDone = false; IsDone == false; IsDone)
		{
			if ( i + 1 > JsonTokenCount)
			{
				IsDone = true;
				cout << "Done searching.\n";
			}
			else if ( JsonTokens[i].type == JSMN_STRING)
			{
				if ( readJsonString( EightChanJsonString, JsonTokens[i]) == "no")
				{
					for ( int j = 1; i + j + 1 <= JsonTokenCount; j++)
					{
						if ( JsonTokens[i+j].type == JSMN_STRING)
						{
							if ( readJsonString( EightChanJsonString, JsonTokens[i+j]) == "com")
							{
								PostNumberJson = readJsonString( EightChanJsonString, JsonTokens[i+j+1]);
								IsDone = true;
							}
						}
					}

				}
			}
			i++;
		//	cout << i << endl;
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
		return "\r\b\r";
	}
}
