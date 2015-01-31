using namespace std;

struct fourchanpost_t
{
	string BoardLetter; // the board in which the post resides in
	string ThreadNumber; // the thread in which the post resides in
	string PostNumber; // the post number of the post
	bool IsHttps; // if i decide to implement https 
};

struct fourchanpost_t getFourChanPostInfo( string FourChanURL) // tears apart a 4chan url to a post and gets structured info about the post the url points to
{
	bool IsHttps = false;
	if ( FourChanURL.rfind( "https", FourChanURL.find( "://boards.4chan.org/", 0)) != -1)
	{
		IsHttps = true;
	}
	if ( FourChanURL.rfind( "http", FourChanURL.find( "://boards.4chan.org/", 0)) != -1)
	{
		FourChanURL.erase( 0, FourChanURL.rfind( "http", FourChanURL.find( "://boards.4chan.org/", 0)));
	}
	if ( FourChanURL.find( "\r", 0) != -1)
	{
		FourChanURL.erase( FourChanURL.find( "\r", 0), FourChanURL.size() - 1);
	}
//	cout << FourChanURL << endl;
	
	string BoardLetter = FourChanURL;
	if ( BoardLetter.find( "/", 10) != -1)
	{
		BoardLetter.erase( 0, BoardLetter.find( "/", 10));
	}
	if ( BoardLetter.find( "/thread", 0) != -1)
	{
		BoardLetter.erase( BoardLetter.find( "/thread", 0) + 1, BoardLetter.size() - 1);
	}
//	cout << BoardLetter << endl;

	string ThreadNumber = FourChanURL;
	if ( ThreadNumber.find( "/thread/", 0) != -1)
	{
		ThreadNumber.erase( 0, ThreadNumber.find( "/thread/", 0) + 8);
	}
	if ( ThreadNumber.find( "/", 0) != -1)
	{
		ThreadNumber.erase( ThreadNumber.find( "/", 0), ThreadNumber.size() - 1);
	}
	else
	{
		if ( ThreadNumber.find( "#p", 0) != -1)
		{
			ThreadNumber.erase( ThreadNumber.find( "#p", 0), ThreadNumber.size() - 1);
		}
	}

//	cout << ThreadNumber << endl;

	string PostNumber = FourChanURL;
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

	struct fourchanpost_t FourChanPostInfo;
	FourChanPostInfo.BoardLetter = BoardLetter;
	FourChanPostInfo.ThreadNumber = ThreadNumber;
	FourChanPostInfo.PostNumber= PostNumber;
	FourChanPostInfo.IsHttps = IsHttps;
	
	return FourChanPostInfo;
}
