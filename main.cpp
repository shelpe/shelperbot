#include <iostream>
#include <cstring>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctime>
#include <curl/curl.h>
#include <ctime>
#include <jsmn.h>

#define MAXDATASIZE 256
#define FLAGCOUNT 3

#include "curlbuffer.cpp"
#include "4chanurl.cpp"
#include "4chanposttext.cpp"
#include "4chanimage.cpp"
#include "8chanurl.cpp"
#include "8chanposttext.cpp"

using namespace std;

int handlePossibleFlag( string PossibleFlag) // this is where command line options are defined
{
	if ( PossibleFlag.compare( "--no-image-text") == 0)
	{
		return 1;
	}
		if ( PossibleFlag.compare( "--mock") == 0)
	{
		return 2;
	}
	else
	{
		return -1;
	}
}

bool sendData( int BotSocket, string Message) // send data through a socket
{
	int BytesSent = send( BotSocket, Message.c_str(), Message.length(), 0);
	cout << Message << endl;
	if ( BytesSent == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

char* prepareCommand( string Command, string Message) // according to rfc2812 messages to ircd should end with \r\n
{
	Message.insert( 0, Command);
	Message.append( "\r\n");
	char* Message_out = ( char*) Message.c_str();
	return Message_out;
}

void sayMessage( int BotSocket, string Message, string Channel) // says a message to the channel specified in launch options
{
	Channel.append( " :");
	Channel.append( Message);
	sendData( BotSocket, prepareCommand( "PRIVMSG ", Channel));
}

void sayAction( int BotSocket, string Message, string Channel) // says a message to the channel specified in launch options
{
	Channel.append( " :");
	Channel.append( Message);
	sendData( BotSocket, prepareCommand( "ACTION ", Channel));
}

int connectSocket( char* TargetName, char* TargetPort) // sets up a socket to talk to ircd
{
	struct addrinfo StuffAboutTarget, *TargetInfo;

	memset( &StuffAboutTarget, 0, sizeof StuffAboutTarget);

	StuffAboutTarget.ai_family = AF_UNSPEC;
	StuffAboutTarget.ai_socktype = SOCK_STREAM;
	
	int ProbsWithTarget = getaddrinfo( TargetName, TargetPort, &StuffAboutTarget, &TargetInfo);
	if ( ProbsWithTarget != 0)
	{
		cerr << "getaddrinfo problem: " << gai_strerror( ProbsWithTarget) << endl;
		exit(0);
	}

	int Socket = socket( TargetInfo->ai_family, TargetInfo->ai_socktype, TargetInfo->ai_protocol);
	if ( Socket < 0)
	{
		cerr << "Error initializing socket\n";
		exit(0);
	}

	if ( connect( Socket, TargetInfo->ai_addr, TargetInfo->ai_addrlen) < 0)
	{
		close( Socket);
		cerr << "Error connecting to target\n";
		exit(0);
	}

	freeaddrinfo( TargetInfo);

	return Socket;
}


int main()
{
	char IRCServer[64], IRCPort[5];
	string BotNick, TargetChannel;
	cout << "IRC Server Address: ";
	cin >> IRCServer; //the first argument will be the target name
	cout << "IRC Server Port: ";
	cin >> IRCPort; //the second argument will be the target port
	cout << "Bot Nickname: ";
	cin >> BotNick; //third argument is the initial bot nickname
	cout << "Channel to idle in ";
	cin >> TargetChannel; //fourth argument is the channel where all messages will be handled.  it can even be a user too!
	char ImageComment;
	do
	{
		cout << "Post comment content with images? (y/n): ";
		cin >> ImageComment;
		cout << ImageComment;
	} while ( ImageComment != 'y' && ImageComment != 'n');
	int BotSocket = connectSocket( IRCServer, IRCPort);
	
	int RecieveCount = 0;
	int ByteCount;
	char Buffer[MAXDATASIZE];

	while(1)
	{
		RecieveCount++;
		

		ByteCount = recv( BotSocket, Buffer, MAXDATASIZE - 1, 0);
		Buffer[ByteCount] = '\0';//we string now
		string Buffer_s = string (Buffer);
		cout << Buffer_s;

		if ( RecieveCount == 2)
		{
			sendData( BotSocket, prepareCommand( "NICK ", BotNick));
			string BotNick_user = BotNick;
			BotNick_user.append(" 0 * :");
			BotNick_user.append( BotNick);
			sendData( BotSocket, prepareCommand( "USER ", BotNick_user));
		}
		if ( RecieveCount == 10)
		{
			cout << TargetChannel << endl;
			sendData( BotSocket, prepareCommand( "JOIN ", TargetChannel));
		}
		
		if ( Buffer_s.find( "PING :", 0) != -1)
		{ 
			Buffer_s.erase( Buffer_s.find( "PING :"), Buffer_s.find( "PING :") + 6);
			sendData( BotSocket, prepareCommand( "PONG :", Buffer_s));
		}
		string BotNickCommandFormat = BotNick;
	//	cout << "My name is " << BotNick << endl;
		BotNickCommandFormat.append( "...IRC ");
	//	cout << "I respond to " << BotNickCommandFormat << endl;
		if ( Buffer_s.find( BotNickCommandFormat, 0) != -1)
		{ 
			if ( Buffer_s.find( "NICK ") != -1)
			{
				string BotNickNew = Buffer_s;
				BotNickNew.erase( 0, BotNickNew.find( "NICK ") + 5);
				BotNickNew.erase( BotNickNew.find( "\r"), BotNickNew.find( "\r"));
				cout << "Nick changed to " << BotNickNew << endl;
				BotNick = BotNickNew;
			}
			
			string BotCommand = Buffer_s;
			BotCommand.erase( 0, Buffer_s.find( BotNickCommandFormat, 0) + BotNickCommandFormat.size());
			sendData( BotSocket, BotCommand);
		//	sayMessage( BotSocket, BotCommand, TargetChannel);
		}
	
		if ( Buffer_s.find( "://boards.4chan.org/") != -1)
		{
			struct fourchanpost_t PostInfo = getFourChanPostInfo( Buffer_s);
			string FourChanPostText = grabFourChanPostText( PostInfo);	
			
			if ( FourChanPostText != "\r\b\r")
			{
				sayMessage( BotSocket, FourChanPostText, TargetChannel);
			}
		}
		
		/*
		 * Currently any 8chan functionality is broken
		 */
		if ( Buffer_s.find( "://8chan.co/") != -1)
		{
			struct eightchanpost_t PostInfo = getEightChanPostInfo( Buffer_s);
			string EightChanPostText = grabEightChanPostText( PostInfo);	
			
			if ( EightChanPostText != "\r\b\r")
			{
				sayMessage( BotSocket, EightChanPostText, TargetChannel);
			}
		}

		/*
		 * shelperbot will rejoin on kick in 3 seconds
		 */
		string BotKick = "KICK ";
		BotKick.append( TargetChannel);
		if ( Buffer_s.find( BotKick) != -1)
		{
			sleep( 3);
			string RejoinCommand = "JOIN ";
			RejoinCommand.append( TargetChannel);
			cout << RejoinCommand << endl;
			sendData( BotSocket, prepareCommand( RejoinCommand, ""));
		}

		/*
		 * This if statement has shelperbot react to a 4chan image link
		 */
		if ( Buffer_s.find( "://i.4cdn.org/") != -1)
		{
			
			struct ImageInfoResults Results = getFourChanImageInfo( Buffer_s);
			struct fourchanpost_t ImagePostTarget = Results.ImageInfo;
			string Message = Results.Message;
			if ( ImageComment == 'y')
			{
				Message.append( " ---> ");
				Message.append( grabFourChanPostText( ImagePostTarget));
			}
			sayMessage( BotSocket, Message, TargetChannel); 
		}
		
		string BotNickPrivMsg = "PRIVMSG ";
		BotNickPrivMsg.append( BotNick);
		/*
		 * This if statement lets shelperbot shitposts in response to a highlight
		 */
		if ( ( Buffer_s.find( "PRIVMSG", 0) != -1) && ( Buffer_s.find( BotNick, Buffer_s.find( "PRIVMSG", 0)) != -1) && ( Buffer_s.find( BotNickPrivMsg, 0) == -1))
		{
			srand( time( NULL));
			string ReplyList[6];
			ReplyList[0] = "hi punk";
			ReplyList[1] = "ok bub";
			ReplyList[2] = "sure thing nerd";
			ReplyList[3] = "absolutely";
			ReplyList[4] = "well of course";
			ReplyList[5] = "hello newfriend";
			sayMessage( BotSocket, (string ) ReplyList[rand() % 6], TargetChannel);
		}
	
		string HugPhrase = "PRIVMSG ";
		HugPhrase.append( TargetChannel);
		HugPhrase.append( " :.hug ");
		
		if ( ( Buffer_s.find( HugPhrase, 0) != -1) && ( Buffer_s.find( BotNickPrivMsg, 0) == -1))
		{
			Buffer_s.erase( 0, Buffer_s.find( HugPhrase, 0) + HugPhrase.size());
			if ( Buffer_s.find( "\r\n") != -1)
			{
				Buffer_s.erase( Buffer_s.find( "\r\n"), Buffer_s.size() - 1);
			}
			cout << Buffer_s << endl;

			srand( time( NULL));
			string ReplyList[6];
			ReplyList[0] = "\001ACTION warmly hugs "; ReplyList[0].append( Buffer_s);
			ReplyList[1] = "\001ACTION grabs "; ReplyList[1].append( Buffer_s); ReplyList[1].append( " and squeezes hard");
			ReplyList[2] = "\001ACTION wraps her arms around "; ReplyList[2].append( Buffer_s);
			sayMessage( BotSocket, (string ) ReplyList[rand() % 3], TargetChannel);
		}

		if ( ByteCount == 0)
		{
			cout << "CONNECTION CLOSED\n";
			break;
		}
	}



}
