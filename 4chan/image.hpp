using namespace std;

string readJsonString ( string JsonString, jsmntok_t JsonStringInfo) // takes a jsmn string object and turns it into a string variable
{
	JsonString.erase( 0, JsonStringInfo.start);
	JsonString.erase( JsonStringInfo.end - JsonStringInfo.start, JsonString.size() - 1);
	return JsonString;
}

struct JsonTokenListInfo
{
	int Size;
	jsmntok_t* Tokens;
};

struct JsonTokenListInfo getJsonTokenInfo( char* JsonString, int JsonTokenSize) // takes all that jsmn babble and puts it into a nice, readable structure
{
	
	jsmn_parser JsonParser;
	jsmntok_t JsonTokens[JsonTokenSize];
	jsmn_init( &JsonParser);
	jsmnerr_t JsonParserError = jsmn_parse( &JsonParser, JsonString, strlen( JsonString), JsonTokens, JsonTokenSize);
	int JsonTokenCount;
	if ( JsonParserError > JSMN_SUCCESS)
	{
		cout << JsonParserError << " nigga" << endl;
		JsonTokenCount = JsonParserError;
	}
	struct JsonTokenListInfo TokenInfo;
	TokenInfo.Tokens = JsonTokens;
	TokenInfo.Size = JsonTokenCount;
	return TokenInfo;

}

struct ImageInfoResults
{
	struct fourchanpost_t ImageInfo; // structured info about the post the image is attached to
	string Message; // message to send to the IRC channel
};

struct ImageInfoResults getFourChanImageInfo( string Buffer_s)
{
	string BoardLetter = Buffer_s;
	BoardLetter.erase( 0, BoardLetter.find( "://i.4cdn.org/") + 13);
	string ImageNumber = BoardLetter;
	BoardLetter.erase( BoardLetter.find( "/", 1) + 1, BoardLetter.size() - 1);
	ImageNumber.erase( 0, ImageNumber.find( "/", 1) + 1);
	ImageNumber.erase( ImageNumber.find( ".", 1), ImageNumber.size() - 1);
	string BoardCatalogUrl = "http://a.4cdn.org";
	BoardCatalogUrl.append( BoardLetter);
	BoardCatalogUrl.append( "threads.json");
	cout << "Grabbing " << BoardCatalogUrl << " ...\n";
	char* BoardCatalog = getWebPage( ( char*) BoardCatalogUrl.c_str());
	cout << "The page is " << strlen( BoardCatalog) << " long" << endl;

	struct JsonTokenListInfo BoardCatalogTokenInfo = getJsonTokenInfo( BoardCatalog, 25000);
	jsmntok_t* JsonTokens = BoardCatalogTokenInfo.Tokens;
	int JsonTokenCount = BoardCatalogTokenInfo.Size;
	
	string BoardCatalog_s = ( string) BoardCatalog;
	string ThreadList[160];
	int ThreadCount = 0;
	int i = 0;
	cout << "token count is " << JsonTokenCount << endl;
	for ( bool IsDone = false; IsDone == false; IsDone)
	{
		if ( i + 1 > JsonTokenCount)
		{
			IsDone = true;
			cout << "Done searching.  Found " << ThreadCount << " threads.\n";
		}
		else if ( JsonTokens[i].type == JSMN_STRING)
		{
			if ( readJsonString( BoardCatalog_s, JsonTokens[i]) == "no")
			{
				ThreadList[ThreadCount] = readJsonString( BoardCatalog_s, JsonTokens[i+1]);
				cout << ThreadList[ThreadCount] << "no. " << ThreadCount + 1 << endl;
				ThreadCount++;
			}
		}
		i++;
	//	cout << i << endl;
	}
	cout << "ok\n";
	
	for ( int j = 0; i < ThreadCount; i++)
	{
		cout << "http://boards.4chan.org" << BoardLetter << "thread/" << ThreadList[j] << endl;
	}




	int ThreadCheckCount = 0;
	time_t start;
	time_t stop;
	time( &start);
	string PostNumber;
	string ImageFilename, ImageFilenameExt;

	for ( bool IsDone = false; IsDone == false; IsDone)
	{
		if ( ThreadCheckCount > ThreadCount - 1)
		{
			IsDone = true;
			cout << "ERROR: image not found\n";
		}
		else
		{
			string ThreadUrl = "http://a.4cdn.org";
			ThreadUrl.append( BoardLetter);
			ThreadUrl.append( "thread/");
			ThreadUrl.append( ThreadList[ThreadCheckCount]);
			ThreadUrl.append( ".json");
			char* ThreadWebPage = getWebPage( ( char*) ThreadUrl.c_str());
			cout << "Downloaded thread " << ThreadCheckCount + 1 << endl;
			
			struct JsonTokenListInfo ThreadTokenInfo = getJsonTokenInfo( ThreadWebPage, 20000);
			int ThreadTokenCount = ThreadTokenInfo.Size;
			jsmntok_t* ThreadTokens = ThreadTokenInfo.Tokens;
			
			string ThreadWebPage_s = ( string) ThreadWebPage;
			
			
			int j = 0;
			for ( bool IsDone2 = false; IsDone2 == false; IsDone2)
			{
				if ( j + 1 > ThreadTokenCount)
				{
					cout << "ERROR: Post for image not found in this thread\n";
					IsDone2 = true;
				}
				else if ( ThreadTokens[j].type == JSMN_STRING)
				{
					cout << "It's a string\n";
					cout << readJsonString( ThreadWebPage_s, ThreadTokens[j]) << endl;
					if ( readJsonString( ThreadWebPage_s, ThreadTokens[j]) == "tim")
					{
						cout << "It's an image number\n";
						if ( readJsonString( ThreadWebPage_s, ThreadTokens[j+1]) == ImageNumber)
						{
							cout << "It's the image number we have\n";
							int k = 1;
							for ( IsDone2; IsDone2 == false; IsDone2)
							{
								if ( ThreadTokens[j+1-k].type == JSMN_STRING)
								{
									cout << "This is a string as well...\n";
									if ( readJsonString( ThreadWebPage_s, ThreadTokens[j+1-k]) == "filename")
									{
										cout << "Here's the filename\n";
										ImageFilename = readJsonString( ThreadWebPage_s, ThreadTokens[j+2-k]);
										ImageFilenameExt = readJsonString( ThreadWebPage_s, ThreadTokens[j+4-k]);
									}
									else if ( readJsonString( ThreadWebPage_s, ThreadTokens[j+1-k]) == "no")
									{
										cout << "This must be the post number\n";
										PostNumber = readJsonString( ThreadWebPage_s, ThreadTokens[j+2-k]);
										IsDone2 = true;
										IsDone = true;
									}
								}
								k++;
							}
						}
					}
				}
				else
				{
					cout << "Not a string\n";
				}
				j++;
			}
		}
		ThreadCheckCount++;
	}
	time( &stop);
	time_t TimeElapsed = difftime( stop, start);
	
	struct fourchanpost_t ImagePostTarget;
	ImagePostTarget.BoardLetter = BoardLetter;
	ImagePostTarget.ThreadNumber = ThreadList[ThreadCheckCount-1];
	ImagePostTarget.PostNumber = PostNumber;
	ImagePostTarget.IsHttps = false;

	string TimeMessage = "\"";
	TimeMessage.append( ImageFilename);
	TimeMessage.append( ImageFilenameExt);
	TimeMessage.append( "\" @ http://boards.4chan.org");
	TimeMessage.append( BoardLetter);
	TimeMessage.append( "thread/");
	TimeMessage.append( ThreadList[ThreadCheckCount-1]);
	TimeMessage.append( "/#p");
	TimeMessage.append( PostNumber);
	TimeMessage.append( " (");
	char str[3];
	sprintf( str, "%d", ( int) TimeElapsed);
	TimeMessage.append( ( string) str);
	TimeMessage.append( "s)");

	struct ImageInfoResults Results;
	Results.ImageInfo = ImagePostTarget;
	Results.Message = TimeMessage;
	return Results;

}
