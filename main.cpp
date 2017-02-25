/*
 *******************************************************************************
 ***                                                                         ***
 ***    This program is intended to be a tool for a programmer to asses time ***
 ***  spent working on a project and store such info in an easily accessible ***
 ***  format.                                                                ***
 ***                                                                         ***
 *******************************************************************************
*/

/* Changes:
 *
 * v.1.1.00 2015-08-22 21:08 (EET)
 * - Added esc mode and its "e" "esc" escape sequence functionality
 *
 * v.1.1.10 2015-09-11 23:35 (EET)
 * - Changed esc mode so that :w or :write leaves a note and goes back to menu
 *   :q or :quit quits without leaving a note (leaves datestamp - to be fixed)
 * - Changes and additions to onscreen instructions
 *
 * v.1.1.11 2015-10-19 22:10 (EET)
 * - Time stamp is now shown at the beginning of the comment, as part of the
 * line, so that it is apparent to the user, where the words are gonna get
 * wrapped.
 *
 * v.1.1.11 2016-04-12 01:20 (EET)
 * [just a note - put the project into GitHub (testing data integrity ;) )]
 *
 * v.1.2.0 2016-05-9 22:08 (EET)
 * added the possibility to add time to a project by typing it in
*/

//TODO finalise ":q" sequence, so that the program would not print the datestamp
/*TODO implement feature allowing manaual time logging, the timer should be an
 an explicit command available for the user with the bilities to pause/stop/resume*/
//TODO consider making an insteractive menu for commands alongside esc mode
//TODO implement handling of several projects simultaneously
//TODO (2015-10-19:2210) wrap whole words without cutting, for better readability.
//TODO Add 'clear' functionality to escape mode
//TODO remove the word-by-word checeker, seems redundant (check!)
//ASCII gui (ncurses)
//progress bar, percentage, versions
//TODO Capability to Display the log?
//TODO Capability to pause a session?
//TODO Interactive menu of selections
/*TODO Display how long a session and the project has lasted so far
 *(1.at the new note display screen?)(2.esc mode :t)*/
//TODO think about UI: session actions menu when in session, log display


#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <stdlib.h>
using namespace std;

#include <ncurses.h>

void ncursesInit(); //initialise ncurses
string typing(char timeBuffer[80]); //the typing mode
int secsToDays(int _secs);
void greeting(char _v[]); //prints out the wellcome screen
int escMode();

int main(){
  int ch;
  char version[41] = "Version: 1.1.12 [2015-10-26 22:04 (EET)]";

  ncursesInit();
  greeting(version);

  ch = getch();
  if(ch == '1'){
    ofstream fileOut;
    ifstream fileIn;
    string strBuffer;
    string str1 = "";
    char timeBuffer[80];
    int timeDiff = 0;
    int totalTime = 0;
    int i1 = 0;
    time_t time1;
    time_t time2;
    time_t time3;
    struct tm* timeinfo;

    fileOut.open("TheLog", ios::out | ios::app | ios::binary);
    time(&time1);//set time1 to system time
    timeinfo = localtime(&time1);//set time structure to local time based on arg
    strftime(timeBuffer, sizeof(timeBuffer), "%F[%T]%A", timeinfo);//fill timeBuffer with string
    fileOut << "[NEW SESSION] : " << timeBuffer << endl;
    //main loop
    do{
      clear();
      mvprintw(11, 0, "Session started.");
      mvprintw(12, 0, "Press n - to leave a note (/end to end commenting).");
      mvprintw(13, 0, "Press t - to add time post factum");
      mvprintw(14, 0, "Press q - to end session.");
      refresh;
      i1 = getch();

      if(i1 == 'n'){
        time(&time2);//set time2 to system time
        timeinfo = localtime(&time2);//set time structure to local time based on arg
        strftime(timeBuffer, sizeof(timeBuffer), "[%T]", timeinfo);//fill timeBuffer with string
        str1 = str1 + typing(timeBuffer);
        int itr = 0;
        //save str1 to file and add \n every 80 chars (word wrap)
        for(int i = 0; i < str1.size(); i++){
          fileOut << str1[i];
          itr++;
          if(str1[i] == '\n'){
            itr = 0;
          }
          //FIXME wouldnt if(itr = 80) be as effective and more straightforward?
          if(itr > 0 && itr % 80 == 0){//place endl every 80 chars
            fileOut << endl;
          }
        }
        fileOut << endl;
        str1 = "";
      }
      else if(i1 == 't'){
		char chBuff = 0;
		strBuffer = ""; //clearing buffer just in case
		clear();
		mvprintw(0, 0, "Enter time in mintues:");
		
		while(chBuff != '\n'){
		  int iBuff = getch();
		  
		  refresh();
			  
		  if(iBuff == 27){ //ESC key pressed
			continue;
		  }
		  
		  chBuff = iBuff;
		  if(chBuff == '\n' && strBuffer.size() > 0){
		    int result = stoi(strBuffer) * 60;
		    time1 = time1 - result;
		  }
		  
		  strBuffer = strBuffer + chBuff;
		  mvprintw(1, 0, strBuffer.c_str());
		}
	  }
    }while(i1 != 'q');

    //wrap up
    time(&time2);//set time2 to system time
    timeinfo = localtime(&time2);//set time structure to local time based on arg
    strftime(timeBuffer, sizeof(timeBuffer), "%F[%T]%A", timeinfo);//fill timeBuffer with string
    fileOut << "[END OF SESSION] : " << timeBuffer << endl;
    timeDiff = difftime(time2, time1);//get difference in seconds
    time3 = timeDiff;
    timeinfo = gmtime(&time3);
    strftime(timeBuffer, sizeof(timeBuffer), "%T", timeinfo);//fill timebuffer with string; here timeinfo is set to timeDiff
    fileOut << "Session time: " << timeBuffer << endl;
    fileOut.close();
    fileIn.open("totaltm", ios::in | ios::binary);
    strBuffer = "";//emptying strBuffer to later verify totaltm file data
    getline(fileIn, strBuffer);
    fileIn.close();
    if(strBuffer == ""){
      strBuffer = "0";//if totaltm was empty or contained wrong data, set buffer to "0"
    }
    totalTime = stoi(strBuffer) + timeDiff;
    fileOut.open("totaltm", ios::out | ios::binary);
    fileOut << totalTime;//no eol in totaltm
    fileOut.close();
    fileOut.open("TheLog", ios::out | ios::app | ios::binary);
    time3 = totalTime;
    timeinfo = gmtime(&time3);
    strftime(timeBuffer, sizeof(timeBuffer), "%T", timeinfo);//fill timebuffer with string; here timeinfo set to totalTime
    i1 = totalTime / 3600;
	str1 = to_string(i1) + "h";
    i1 = secsToDays(totalTime);
    str1 = str1 + " (" + to_string(i1) + "d. " + timeBuffer + ")";
    fileOut << "Project time: " << str1 << endl << endl;
    fileOut.close();
  }
  endwin();//end curses mode
  return 0;
}

void ncursesInit(){
  initscr(); //initialise ncurses mode
  raw(); //all input, even the escape sequences, go directly to ncurses and not the terminal call handling
  noecho(); //do not echo to terminal command line
  keypad(stdscr, TRUE); //enable F1, F2, etc
  curs_set(0);//set cursor to invisible
}

string typing(char _timeBuffer[80]){
  int x = 0, y = 0;
  int chi;
  char chc;
  bool refreshed = false;
  string word;
  string pckg;
  string tmp1; //used to temporarily store char[] arrays for converting

  clear();
  tmp1 = _timeBuffer;
  tmp1 = "====>Current time: " + tmp1 + "<=====";
  mvprintw(5, 10, tmp1.c_str());
  mvprintw(7, 10, "type in ' /end' and press enter or space when finished");
  mvprintw(9, 10, "You can allso use the new 'Esc mode.'");
  mvprintw(10, 10, "Press 'Esc' then type ':w' and press 'Enter' to write");
  mvprintw(11, 10, "Press 'Esc' then type ':q' and press 'Enter' to quit");
  mvprintw(0, 10, "===========================================================");
  mvprintw(1, 10, "The instructions will dissapear as soon as you start typing");
  mvprintw(2, 10, "===========================================================");
  move(0,0);
  do{
    word = "";
    do{
      chi = getch();
      chc = chi;//converting chi to char type
      if(!refreshed){
        //display comment time
        word = _timeBuffer;
        word = word + " ";
        //clear instructions from screen if typing
        clear();
        refreshed = true;
      }
      //esc mode
      if(chi == 27){
        switch(escMode()){
          case 0: continue;//esc mode is cancelled (second press of 'Esc')
          case 1: //esc mode command = :write or :w
            pckg = pckg + word;
            word = "/end";
            break;
          case 2: //esc mode command = :quit or :q
            pckg = "";
            return pckg;
          default:
            mvprintw(0, 0, "WARNING: Unhandled return value of EscMode");
            mvprintw(1, 0, "Press any key to continue");
            refresh();
            int ch = getch();//wait for any keypress
            continue;
        }
        refreshed = false;
        if(word == "/end"){
          break;//let's not get back to getchar()
        }
        continue;
      }
      if(chi == KEY_BACKSPACE){
        if(word.size() > 0){
          word.pop_back();
        }
        else{
          pckg.pop_back();
        }
        //reprint modified (one char deleted) text on screen
        mvprintw(0,0, pckg.c_str());
        printw(word.c_str());
        delch();
        refresh();
        continue;
      }
      else{
        word = word + chc;
        mvprintw(0,0, pckg.c_str());
        printw(word.c_str());
        refresh();
      }
    }while(chi != ' ' && chi != '\n');
    //TODO remove the old exit type?
    //if word == /end , return pckg
    if(word[0] == '/' && word[1] == 'e' && word[2] =='n' && word[3] =='d'){
      return pckg;
    }
    pckg = pckg + word;
  }while(1 > 0);
  return pckg;
}

int secsToDays(int _secs){
  int days;

  days = _secs / 86400;

  return days;
}

void greeting(char _v[]){
  mvprintw(5, 10, "Welcome to Prolog, the project logger");
  mvprintw(10, 10, "1 - New Session");
  mvprintw(11, 10, "0 - Exit");
  mvprintw(22, 0, "Written by Egidijus Nedzinskas.");
  mvprintw(23, 0, _v);
}

int escMode(){
  int emiCh = 0;
  char emcCh = 0;
  string emWord = "";

  do{
    move(23, 0);
    clrtoeol();
    printw(emWord.c_str());
    if(emWord[0] == 0){ //print instructions if there's nothing else to print
      mvprintw(23, 0, "Type your commands here");//FIXME(#1#) why is there a delay?
    }
    refresh;
    emiCh = getch();
    emcCh = emiCh;
    if(emiCh != 27){ // char #27 is the 'Esc'
        emWord = emWord + emcCh;
    }
    else if(emiCh == KEY_BACKSPACE && emWord.size() > 0){
      emWord.pop_back();
    }
    // return block vvv
    if(emiCh == '\n'){
      if(emWord.find(":write") != string::npos ||
       emWord[0] == ':' && emWord[1] == 'w'){
        return 1;
      }
      else if(emWord.find(":quit") != string::npos||
       emWord[0] == ':' && emWord[1] == 'q'){
       return 2;
      }
    }
  }while(emiCh != 27); // char #27 is the 'Esc'

  move(23, 0);
  clrtoeol();
  return 0;
}
