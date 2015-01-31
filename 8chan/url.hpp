/*
 * THIS DOESN'T WORK WELL YET EITHER
 */
using namespace std;

struct eightchanpost_t
{
	string BoardLetter;
	string ThreadNumber;
	string PostNumber;
	bool IsHttps;
};

struct eightchanpost_t getEightChanPostInfo( string EightChanURL)
{
	bool IsHttps = false;
	if ( EightChanURL.rfind( "https", EightChanURL.find( "://8chan.co/", 0)) != -1)
	{
		IsHttps = true;
	}
	if ( EightChanURL.rfind( "http", EightChanURL.find( "://8chan.co/", 0)) != -1)
	{
		EightChanURL.erase( 0, EightChanURL.rfind( "http", EightChanURL.find( "://8chan.co/", 0)));
	}
	if ( EightChanURL.find( "\r", 0) != -1)
	{
		EightChanURL.erase( EightChanURL.find( "\r", 0), EightChanURL.size() - 1);
	}
//	cout << EightChanURL << endl;
	
	string BoardLetter = EightChanURL;
	if ( BoardLetter.find( "/", 10) != -1)
	{
		BoardLetter.erase( 0, BoardLetter.find( "/", 10));
	}
	if ( BoardLetter.find( "/res", 0) != -1)
	{
		BoardLetter.erase( BoardLetter.find( "/res", 0) + 1, BoardLetter.size() - 1);
	}
//	cout << BoardLetter << endl;

	string ThreadNumber = EightChanURL;
	if ( ThreadNumber.find( "/res/", 0) != -1)
	{
		ThreadNumber.erase( 0, ThreadNumber.find( "/res/", 0) + 5);
	}
	if ( ThreadNumber.find( "/", 0) != -1)
	{
		ThreadNumber.erase( ThreadNumber.find( "/", 0), ThreadNumber.size() - 1);
	}
	else
	{
		if ( ThreadNumber.find( "#p", 0) != -1)
		{
			ThreadNumber.erase( ThreadNumber.find( "#p", 0) - 5, ThreadNumber.size() - 1);
		}
	}
	if ( ThreadNumber.find( ".html", 0) != -1)
	{
		ThreadNumber.erase( ThreadNumber.find( ".html", 0), ThreadNumber.size() - 1);
	}

//	cout << ThreadNumber << endl;

	string PostNumber = EightChanURL;
	if ( PostNumber.find( "#p", 0) == -1)
	{
		PostNumber = ThreadNumber;
	}
	else
	{
		if ( PostNumber.find( "#p", 0) != -1)
		{
			PostNumber.erase( 0, PostNumber.find( "#p", 0) + 2);
		}
	}

	struct eightchanpost_t EightChanPostInfo;
	EightChanPostInfo.BoardLetter = BoardLetter;
	EightChanPostInfo.ThreadNumber = ThreadNumber;
	EightChanPostInfo.PostNumber= PostNumber;
	EightChanPostInfo.IsHttps = IsHttps;
	
	return EightChanPostInfo;
}
