/*
Version Number: 2.2.0
Build Number: 202
Now in Beta.
*/
/*
DESCRIPTION:
OCTPP Chess 2015
12th Standard Computer Science Project
Copyright © 2015 by Abhishek Krishna, Inavamsi BE, Sesh Sadasivam
A game of chess with online and multiplayer capabilities.
*/

//
// OCTPP Chess 2015
// Chess.cpp
//
// Copyright (c) 2015 by DIGS Gam Re-Creation Studios.
// OCTPP Game Studios was founded by Abhishek Krishna, Inavamsi BE and Sesh
// Sadasivam
//
//
// This file is part of OCTPP Chess 2015.
//
// OCTPP Chess 2015 is free software. You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OCTPP Chess 2015 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OCTPP Chess 2015. If not, see <http://www.gnu.org/licenses/>.
//
//
// Originally conceptualized at DIGS Re-Creation Studios, founded by Abhishek
// Krishna, Aditya Giridharan, Inavamsi BE and Sesh Sadasivam with the vision
// to recreate games that have nurtured human genius.
//

//
// Welcome to the source code of OCTPP Chess! Feel free to examine it. Take
// inspiration if you wish. But please comply with the GPL. :)
// We've left helpful comments in some places which would (hopefully)
// help you out.
//

/* --- HEADERS --- */
// wxHeaders
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/string.h>
#include <wx/colordlg.h>
#include <wx/dcclient.h>
#include <wx/textctrl.h>
#include <wx/msgdlg.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/image.h>
#include <wx/dataview.h>

// Other Headers
#include <fstream>		// For files
#include <ctime>		// For seeding the random function
#include <curl/curl.h>		// CurlLib; For connection to php forms
#include <process.h>	// For multithreading
using namespace std;

/* --- PROTOTYPES --- */
// Classes
class MyApp;			// The main application class
class SplashScreen;		// The splash screen on launch
class DrawPane;			// Everything needed to draw items in the window
class KilledPane;		// The panel that displays all killed pieces
class PromotionPane;	// The panel to select the promoted piece
class Square;		// The actual Chess board!
class AI;				// A virtual class defining what an AI must contain
class AI_1;				// Piece of cake! It's random.
class AI_2;				// "Slightly" more intelligent than just random moves
class Window_Game;		// Games Windows
class Window_UI;		// UI Windows
class Game;				// The actual game class
class Account;			// Everything needed for online capabilities
class UI;
class SaveMoves;

// Functions

/* --- GLOBAL VARIABLES --- */
// These are pointers which refer to windows, panels and panes
SplashScreen *windowSplash;
Window_Game *windowGame;
Window_UI *windowStart, *windowNewGame, *windowLogin, *windowLogout, *windowCreateAccount, *windowNewGameOnePlayer, *windowNewGameTwoPlayer, *windowNewGameTwoPlayerOnline, *windowNewGameTwoPlayerLocal, *windowCheckRequest, *windowAccount, *windowLoad;
wxPanel *panelStart, *panelNewGame, *panelLogin, *panelLogout, *panelCreateAccount, *panelNewGameOnePlayer, *panelNewGameTwoPlayer, *panelNewGameTwoPlayerOnline, *panelNewGameTwoPlayerLocal, *panelCheckRequest, *panelAccount, *panelLoad;
DrawPane *drawPane;
KilledPane *killedPane;
PromotionPane *promotionPane;

wxButton *NewGameButton, *LoadGameButton, *MyAccountButton, *LoginButton, *LogoutButton, *NewAccountButton;
wxImage NewGamePic, LoadGamePic, MyAccountPic, LoginPic, LogoutPic, NewAccountPic;

AI *CPWACN; //Chess Player Without A Cool Name
wxTextCtrl *LogWindow;

wxStatusBar *StatusBarGame, *StatusBarStart, *StatusBarUI;

// Other variable which store data:
int x, y;									// The mid-points of the drawing area, in pixels
int i, j;									// Temporary variables for random use
int xking = 0, yking = 0;
bool highlight = false;						// highlight indicates whether a piece is highlighted
int highlighted_x = 0, highlighted_y = 0;	// Stores the grid co-ordinates of the highlighted square
wxColour colour_dark(220, 20, 20), colour_light(0, 240, 240);	// Stores the two player colours
int EP_x = 0, EP_y = 0;						// For storing En Passant coordinates
int stale50_light = 0, stale50_dark = 0;

// Parameters for Online Two Player
int moveDigit = 0;
int pingDigit = 0;
int otherMoveDigit = 9;
int otherPingDigit = 9;

// Options that can be toggled by the user:
bool board_rotation = true, touch_piece = false, beginnerMode = false;

// Enumerators to make coding easier
enum piece_code{ // Used to mark what a square on the chessboard holds
	EMPTY = 0, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};
enum piece_colour{ // Used to indicate which side's turn it is during the game
	LIGHT = true, DARK = false
};
enum game_state { // The kind of game that's being played, to enable different functionality
	NONE = 0, ONE_PLAYER, TWO_PLAYER_LOCAL, TWO_PLAYER_INTERNET_PLAYER_1, TWO_PLAYER_INTERNET_PLAYER_2
} sessionState;

enum internet_modes {	// Used to specify file upload/delete functions.
	COMPLETED = 1, SAVE = 2, DELETE_SAVE = 4, DELETE_COMPLETE = 8, MOVE = 16
};

// Status flags so that we can check if a thread/function has finished.
bool isExitSoundPlaying;
bool isPingWorking;
bool isExit = true, isLoginStartupWorking = true;

bool GameEnded = false, isCheck = false;
bool acceptRequestCheckOn = false, pingPutOn = false, pingGetOn = false, isPromoteRegisterRunning = false;
bool internetConnected = false, serverConnected = false, initInternetCheck = false, initServerCheck = false;

// Other miscellaneous variables
int nameLength = 0, PromoteRegisterParameter = 0, soundMute = 0, firstLaunch = 1;
float moves = 0;

wxIcon icon; // Holds the application icon location

// The arrays holding the kileld pieces. This is displayed in the killedPane.
piece_code killed_white[16] = { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };
piece_code killed_black[16] = { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };

// Structures to encapsulate simlpe objects
struct move_coords { // Stores a move. Used during the game, and by the AI to check possible moves
	int x1, x2, y1, y2;
	piece_code final;
};
struct Player { // Stores information about the player
	piece_colour Colour;
	wxString Name = "GUEST";
}Player1, Player2;
struct numberof { // Used by the AI to determine good moves
	int power, pawn, powpts;
};
// The following structures are used to send parameters to multi-threaded functions
struct argsForUploadFile {
	wxString userName;
	wxString fileName;
	int mode;
} argListForUploadFile;
struct argsForPing {
	wxString userName;
	wxString otherUser;
} argListForPing;
struct argsForMoveRegister {
	wxString UserMaking, UserReceiving;
	int x1, y1, x2, y2;
} argListForMoveRegister;
struct argsForMoveCheck {
	wxString UserChecking, UserMaking;
} argListForMoveCheck;
struct argsForStatUpdate {
	wxString username;
	int mode;
} argListForStatUpdate;

/* --- CLASS DEFINITIONS --- */
// The main class
class MyApp : public wxApp{
public:
	bool OnInit(); // This function is the first to be called.
};

class Square{ // Each square on the cheesboard is a Square
public:
	int x, y;				// The x and y co-ordinates of the square on the gameboard
	piece_code piece;		// 1 for pawn, 2 for knight, 3 for bishop, 4 for rook, 5 for queen, 6 for king
	piece_colour colour;	// LIGHT for white, DARK for black

	void Draw(int);								// The function for drawing pieces in any pane
	void Pawn(wxClientDC*, int, int, int);		// Draws pawn
	void Knight(wxClientDC*, int, int, int);	// Draws knight
	void Bishop(wxClientDC*, int, int, int);	// Draws bishop
	void Rook(wxClientDC*, int, int, int);		// Draws rook
	void Queen(wxClientDC*, int, int, int);		// Draws queen
	void King(wxClientDC*, int, int, int);		// Draws king
};

class Game {
public:
	Square game[9][9];
	int Validity(int, int, int, int);		// Checks validity of move from initial co-ordinates to final co-ordinates
	bool Check(int, int);					// Checks if the piece at given co-ordinates can be killed
	void Move(int, int, int, int);			// Moves the piece from initial co-ordinates to final co-ordinates
	int Checkmate(bool, int);				// Checks for checkmate or stalemate of given colour
	void PawnPromotion();					// Displays the PromotionPane and lets the player select a piece
	void PawnPromotion(int);				// Simulates PawnPromotion given a piece to promote to. Used by the AI, and in online multiplayer.
	void static Control(int, int, int);
	void static Replay(void*);
	void static Save();
	void static Load();
	void static Initialize();
	void static Shutdown();					// 
	void static EndGame(int);				// Determines that the game has ended and displays appropriate messages
	void UpdateLogWindow();					// Updates the game log window during the game
	void QuitGame();

	void StartOnePlayer();					// Begins a one-player game
	void StartTwoPlayerLocal();				// Begins a local two-player game

	piece_colour turn;						// Stores whose turn it is in the game
	bool castle_white_queen, castle_white_king, castle_black_queen, castle_black_king; // Stores what castling options are available
	bool castle; // Stores if castling has taken place
	int king_white_x, king_white_y, king_black_x, king_black_y; // Used to track the position of the king throughout the game.
}RealGame;

class SplashScreen : public wxFrame {
public:
	SplashScreen(const wxString &title);
	~SplashScreen(){
		Game::Shutdown();
	}
};

class Window_Game : public wxFrame { // The frame for the game
	// All of these functions are toggled options, or are event-driven 
public:
	Window_Game(const wxString &title);
	~Window_Game(){
		Game::Shutdown();
	}
	void Rotation(wxCommandEvent &event);	// Enable/Disable board rotation
	void TouchPiece(wxCommandEvent &event);	// Enable/Disable touch piece
	void Beginner(wxCommandEvent &event);	// Enable/Disable beginner mode
	void Replay(wxCommandEvent &event);		// Replay saved game
	void Save(wxCommandEvent &event);		// Save game
	void Load(wxCommandEvent &event);		// Load game
	void QuitGame(wxCommandEvent &event);	// QuitGame button
	DECLARE_EVENT_TABLE()
};

class Window_UI : public wxFrame {
public:
	~Window_UI(){
		Game::Shutdown();
	}
	Window_UI(const wxString &title);

	// Events
	void paintEvent(wxPaintEvent &evt);							// For redrawing the window
	void OnQuit(wxCommandEvent &event);							// Quit the game
	void About(wxCommandEvent &event);							// About the game
	void ConnectionStatus(wxCommandEvent &event);				// Check connection status
	void SupportAndFeedback(wxCommandEvent &event);				// Opens the webpage support link
	void WhatsNew(wxCommandEvent &event);						// What's new in this version
	void Help(wxCommandEvent &event);							// Help

	// Buttons and form-fields
	void NewGame(wxCommandEvent &event);						// New Game Screen
	void NewGameOnePlayer(wxCommandEvent &event);				// New One Player Game
	void StartOnePlayer(wxCommandEvent &event);					// Start One Player Game
	void NewGameTwoPlayer(wxCommandEvent &event);				// New Two Player Game
	void NewGameTwoPlayerLocal(wxCommandEvent &event);			// New Local Two Player Game
	void NewGameTwoPlayerOnline(wxCommandEvent &event);			// New Online Two Player Game
	void StartTwoPlayerLocal(wxCommandEvent &event);			// Start Two Player Local Game
	void NewGameTwoPlayerRadioPlayer1(wxCommandEvent &event);	// RadioBox for Two Player-Player 1 Colour
	void NewGameTwoPlayerRadioPlayer2(wxCommandEvent &event);	// RadioBox for Two Player-Player 2 Colour
	void NewGameTwoPlayerChoicePlayer1(wxCommandEvent &event);	// Select Two Player-Player 1
	void NewGameTwoPlayerChoicePlayer2(wxCommandEvent &event);	// Select Two Player-Player 2
	void SendRequest(wxCommandEvent &event);					// Send Online Game Request
	void CheckRequest(wxCommandEvent &event);					// Check for Online Game Request
	void LoadGame(wxCommandEvent &event);						// REMOVE
	void CreateAccount(wxCommandEvent &event);					// Create Account Button
	void CreateAccount_Text(wxCommandEvent &event);				// For Enter to Process
	void CreateAccount_Action(wxCommandEvent &event);			// Actually Create New Account
	void Login(wxCommandEvent &event);							// Login Button
	void Login_Action(wxCommandEvent &event);					// Actually Login
	void Login_Text(wxCommandEvent &event);						// For Enter to Process
	void ForgotUsername(wxCommandEvent &event);					// Get information for ForgotUsername
	void ForgotPassword(wxCommandEvent &event);					// Get information for ForgotPassword
	void Logout(wxCommandEvent &event);							// Logout Button
	void Logout_Action(wxCommandEvent &event);					// Actually Logout
	void Account(wxCommandEvent &event);						// 
	void GameTable(wxCommandEvent &event);						// 
	void Stats(wxCommandEvent &event);							// Show user statistics online
	void ChangeColour(wxCommandEvent &event);					// Change colour of selected account
	void ChangePassword(wxCommandEvent &event);					// Get information for ChangePassword
	void DeleteAccount(wxCommandEvent &event);					// Delete the user account
	void Load(wxCommandEvent &event);							// Load game button
	void Home(wxCommandEvent &event);							// Home Button

	// Settings
	void Setting_ChangeColourLight(wxCommandEvent &event);
	void Setting_ChangeColourDark(wxCommandEvent &event);
	void Setting_ResetColourLight(wxCommandEvent &event);
	void Setting_ResetColourDark(wxCommandEvent &event);
	void Sounds(wxCommandEvent &event);							// To mute/unmute sounds
	DECLARE_EVENT_TABLE()
};

class DrawPane : public wxPanel {// DrawPane for drawing the gameboard
public:
	int unit, square, width, height;	// The stuff required to draw onto the screen

	DrawPane(wxFrame* parent);			// Constructor
	void paintEvent(wxPaintEvent &evt);	// Called when game window is modified & starts
	void Reload();						// Redraws everything on the game window; Used for paintEvent, refreshing
	void Click(wxMouseEvent &evt);		// Receives click and processes it
	void Chessboard();					// Draws / Updates chessboard
	void Units();						// Gets DrawPane units
	void Markings(int);					// Draws markings around chessboard
	int convertx(int);					// Converts grid X co-ordinates to pixel X co-ordinates
	int converty(int);					// Converts grid Y co-ordinates to pixel Y co-ordinates
	DECLARE_EVENT_TABLE()
};

class KilledPane : public wxPanel {
public:
	int unit, square, width, height;// Drawing parameters
	piece_colour kill_colour;
	piece_code killed_piece;
	int kill_no;					// How many pieces have been killed

	KilledPane(wxFrame* parent);	// Constructor
	void Killed();					// Draws killed pieces on KilledPane
	void Units();					// Gets KilledPane Units
};

class PromotionPane : public wxPanel {
public:
	int unit, square, width, height;		// Drawing Parameters
	piece_colour promote_colour;			// Whether the player is LIGHT or DARK
	int piece_number, promote_x, promote_y; // The coordinates, and the promoted piece selected
	piece_code promote_piece;				// The promoted piece selected
	bool promotion_state;					// Whether the PromotionPane should be displayed or not

	PromotionPane(wxFrame* parent);
	void Click(wxMouseEvent &evt);			// Conrols click
	void Units();							// Sets units for PromotionPane

	DECLARE_EVENT_TABLE()
};

class AI { // Abstract Class with a pure virutal function
protected:
	Square gameTemp[9][9];
	Square gameTemp1[9][9];
	int No_of_moves;
	struct AvailableMoves {
		int x1, y1;
		int x2, y2;
		AvailableMoves* NEXT;
	} *TOP, *temp;

	void FindMoves();
	void ClearMoves();
	void Backup();
	void Backup1();
	void VirtualMove(int, int, int, int);
	void Restore();
	void Restore1();
	void MakeTheMove(int, int, int, int);

	move_coords Checkmate(piece_colour, int);
	bool CheckmatePossible(piece_colour);
	move_coords PawnPromotion(piece_colour);
	numberof Threats(int, int);
	numberof Protects(int, int);
	move_coords Fork(piece_colour, int);
	move_coords Attack(int, int, int);
	bool RandomMove(piece_code);
	bool RandomMovei(piece_code);
	bool Rank(piece_code, piece_code);

public:
	virtual void moveAI() = 0;		// The main AI function. It's pure virtual.
	void static FirstMove(void*){	// Multithread function for AI first move when AI is light colour
		wxMilliSleep(1500);
		if (Player1.Colour == LIGHT){
			CPWACN->moveAI();
		}
	}


};

class AI_1 : public AI { // The random AI. It's like a monkey that just knows to make a valid move.
public:
	AI_1();
	void moveAI(); // This function defines the behaviour of this AI
};

class AI_2 : public AI { // A slightly more intelligent AI
public:
	AI_2();
	void moveAI();
	void Move_1();
	void AI_QUEEN();
	void AI_ROOK();
	void AI_BISHOP();
	void AI_KNIGHT();
	void AI_PAWN();
	void AI_RANDOM();
	void AI_CHECKMATE();
};

class AI_3 : public AI { // A much more intelligent AI, but isn't complete.
public:
	void moveAI();
	void Move_1();
	void Move_2();
};

class SaveMoves{	// Retrieves and saves information for the log file.
private:
	int x1, y1, x2, y2;
	piece_code piece_moved, piece_killed;
	int special_code;	//0 for none, 2 for knight, 3 for bishop, 4 for rook, 5 for queen, 7 for castle, 8 for En Passant
public:
	void Initial(int x, int y){
		x1 = x;
		y1 = y;
	}
	void Killed(int x, int y){
		x2 = x;
		y2 = y;
	}
	void Piece1(piece_code piece){
		piece_moved = piece;
	}
	void Piece2(piece_code piece){
		piece_killed = piece;
	}
	void Special(int special){
		special_code = special;
	}
	int GetSpecial(){
		return special_code;
	}
	void Save();
}SaveObj;

class Account { // Everything online.
public:
	void static InternetCheck(void*);				// Ensures that the user is connected to the Internet
	void static ServerCheck(void*);					// Ensures that the OCTPP server is up and responding
	void static Login();							// Acceses the user login form
	void static LoginStartup(void*);				// Logs in users on startup, without a password
	void static Logout();							// Logs out users
	void static CreateAccount();					// Accesses the newUser form
	void static ForgotUsername(wxString &emai);		// Accesses ForgotUsername form
	void static ForgotPassword(wxString &uname);	// Accesses ForgotPassword form
	void static ChangePassword(wxString &newPwd);	// Accesses Change Password form
	void static DeleteAccount(wxString &emai);		// Acceses the UnRegister Form
	void static SendRequest();						// Sends a two-player online request
	void static AcceptRequest();					// Accepts a downloaded request
	void static AcceptRequestCheck(void*);			// Downloads a request
	void static CheckRequest();						// Check if there is a new request
	void static OnlineUsers();						// Downloads a list of all online users
	void static Ping(void*);						// Pings the server during the online game, and ensures that other player is still online
	void static MoveRegister(void*);				// Registers a move made on this side
	void static MoveCheck(void*);					// Checks for a new move by the other player, downlaods it, plays it, then unregisters i
	void static PromoteRegister(void*);				// Uploads the promoted piece selected, if necessary
	void static PromoteCheck(void*);				// Waits for the promoted piece selected by the other player, if necessary
	void static StatUpdate(void*);					// Updates user statistics online
	void static UpdateCheck(void*);					// Checks if an update to the game is available
	void static UploadGame(void*);					// Uploads/Deletes game on the server
}AccountObj;

class UI {
private:
	// Buttons and Boxes!
	wxTextCtrl *Login_Username_field, *Login_Password_field;
	wxTextCtrl *CreateAccount_Username_field, *CreateAccount_Password_field, *CreateAccount_ConfirmPassword_field, *CreateAccount_Email_field;
	wxRadioBox *MyColour, *AILevel, *MultiplayerOption;
	wxChoice *NewOnePlayerUsers, *LogoutUsers, *AccountUsers;
	wxTextCtrl *InfoBar;
	wxCheckBox *KeepLoggedIn;
	wxChoice *NewTwoPlayerUser1, *NewTwoPlayerUser2, *NewTwoPlayerUserOnline;
	wxRadioBox *Player1Colour, *Player2Colour, *PlayerOnlineColour;
	wxBoxSizer *Local, *Online;
	wxButton *StartTwoPlayer;
	wxChoice *OnlinePlayerToPlay;
	wxTextCtrl *Message;
	wxListBox *GameTable_Number, *GameTable_Name, *GameTable_Player, *GameTable_Date;
public:
	void Splash();					// Creates the SplashScreen
	void Startup();					// Creates the home window
	void NewGame();					// Creates new new game window
	void NewGameOnePlayer();		// Creates new game one player window
	void StartOnePlayer();			// Creates one player game window
	void NewGameTwoPlayer();		// Creates new game two player window
	void NewGameTwoPlayerOnline();	// Creates new game two player online window
	void NewGameTwoPlayerLocal();	// Creates new game two player local window
	void StartTwoPlayerLocal();		// Creates two player local game window
	void StartTwoPlayerOnline();	// Creates two player online game window
	void CreateAccount();			// Create the new account window
	void Login();					// Create the login window
	void Logout();					// Create the logout window
	void Account();					// Create the my account window
	void Load();					// Create the load game window

	friend class Account;
	friend class Game;
	friend class Window_UI;
}UIObj;

/*Constant IDs*/
enum EventHandlerIDs{
	ID_NEW = 1000, ID_LOAD, ID_ACCOUNT, ID_LOGIN, ID_LOGOUT, ID_CREATE_ACCOUNT,
	ID_LOGIN_ACTION, ID_LOGIN_TEXT, ID_LOGOUT_ACTION, ID_CREATE_ACCOUNT_ACTION, ID_CREATE_ACCOUNT_TEXT,
	ID_EXIT, ID_ABOUT, ID_HOME, ID_BOARD_ROTATION, ID_BEGINNER_MODE, ID_TOUCH_PIECE, ID_CONNECTION_STATUS, ID_SUPPORT_FEEDBACK, ID_SETTING_SOUND, ID_WHATS_NEW, ID_HELP,
	ID_NEW_GAME_ONE_PLAYER, ID_NEW_GAME_TWO_PLAYER,
	ID_RADIO_PLAYER1_COLOUR, ID_RADIO_PLAYER2_COLOUR,
	ID_CHOICE_PLAYER1_NAME, ID_CHOICE_PLAYER2_NAME,
	ID_START_ONE_PLAYER, ID_START_TWO_PLAYER_LOCAL, ID_NEW_GAME_TWO_PLAYER_LOCAL, ID_NEW_GAME_TWO_PLAYER_ONLINE,
	ID_SEND_REQUEST, ID_CHECK_REQUEST,
	ID_FORGOT_USERNAME, ID_FORGOT_PASSWORD, ID_CHANGE_PASSWORD, ID_CHANGE_COLOUR,
	ID_GAME_TABLE, ID_STATS, ID_QUITGAME, ID_DELETE_ACCOUNT,
	ID_SETTING_CHANGE_COLOUR_LIGHT, ID_SETTING_CHANGE_COLOUR_DARK, ID_SETTING_RESET_COLOUR_LIGHT, ID_SETTING_RESET_COLOUR_DARK
};

enum ModeIDs{
	MODE_DRAW_NORMAL = 0, MODE_DRAW_HIGHLIGHT = 1, MODE_DRAW_KILLED = 2, MODE_DRAW_PROMOTION = 3, MODE_DRAW_BEGINNER = 4, MODE_DRAW_CHECK = 5,
	MODE_ENDGAME_CHECKMATE_1 = 1, MODE_ENDGAME_CHECKMATE_2 = 2, MODE_ENDGAME_STALEMATE = 3, MODE_ENDGAME_DRAW = 4,
	MODE_STAT_UPDATE_WIN, MODE_STAT_UPDATE_LOSE, MODE_STAT_UPDATE_DRAW
};

/*EVENT TABLES*/
BEGIN_EVENT_TABLE(Window_Game, wxFrame)
EVT_MENU(ID_EXIT, Window_UI::OnQuit)
EVT_MENU(ID_ABOUT, Window_UI::About)
EVT_MENU(ID_CONNECTION_STATUS, Window_UI::ConnectionStatus)
EVT_MENU(ID_SUPPORT_FEEDBACK, Window_UI::SupportAndFeedback)
EVT_MENU(ID_WHATS_NEW, Window_UI::WhatsNew)
EVT_MENU(ID_HELP, Window_UI::Help)
EVT_MENU(ID_SETTING_CHANGE_COLOUR_LIGHT, Window_UI::Setting_ChangeColourLight)
EVT_MENU(ID_SETTING_CHANGE_COLOUR_DARK, Window_UI::Setting_ChangeColourDark)
EVT_MENU(ID_SETTING_RESET_COLOUR_LIGHT, Window_UI::Setting_ResetColourLight)
EVT_MENU(ID_SETTING_RESET_COLOUR_DARK, Window_UI::Setting_ResetColourDark)
EVT_MENU(ID_SETTING_SOUND, Window_UI::Sounds)
EVT_MENU(ID_BOARD_ROTATION, Window_Game::Rotation)
EVT_MENU(ID_BEGINNER_MODE, Window_Game::Beginner)
EVT_MENU(ID_TOUCH_PIECE, Window_Game::TouchPiece)
EVT_MENU(3, Window_Game::Replay)
EVT_MENU(4, Window_Game::Save)
EVT_MENU(5, Window_Game::Load)
EVT_BUTTON(ID_QUITGAME, Window_Game::QuitGame)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(Window_UI, wxFrame)
EVT_PAINT(Window_UI::paintEvent)
EVT_MENU(ID_EXIT, Window_UI::OnQuit)
EVT_MENU(ID_ABOUT, Window_UI::About)
EVT_MENU(ID_CONNECTION_STATUS, Window_UI::ConnectionStatus)
EVT_MENU(ID_SUPPORT_FEEDBACK, Window_UI::SupportAndFeedback)
EVT_MENU(ID_WHATS_NEW, Window_UI::WhatsNew)
EVT_MENU(ID_HELP, Window_UI::Help)
EVT_MENU(ID_SETTING_CHANGE_COLOUR_LIGHT, Window_UI::Setting_ChangeColourLight)
EVT_MENU(ID_SETTING_CHANGE_COLOUR_DARK, Window_UI::Setting_ChangeColourDark)
EVT_MENU(ID_SETTING_RESET_COLOUR_LIGHT, Window_UI::Setting_ResetColourLight)
EVT_MENU(ID_SETTING_RESET_COLOUR_DARK, Window_UI::Setting_ResetColourDark)
EVT_MENU(ID_SETTING_SOUND, Window_UI::Sounds)
EVT_BUTTON(ID_HOME, Window_UI::Home)
EVT_BUTTON(ID_NEW, Window_UI::NewGame)
EVT_BUTTON(ID_NEW_GAME_ONE_PLAYER, Window_UI::NewGameOnePlayer)
EVT_BUTTON(ID_START_ONE_PLAYER, Window_UI::StartOnePlayer)
EVT_BUTTON(ID_NEW_GAME_TWO_PLAYER, Window_UI::NewGameTwoPlayer)
EVT_BUTTON(ID_NEW_GAME_TWO_PLAYER_LOCAL, Window_UI::NewGameTwoPlayerLocal)
EVT_BUTTON(ID_NEW_GAME_TWO_PLAYER_ONLINE, Window_UI::NewGameTwoPlayerOnline)
EVT_BUTTON(ID_START_TWO_PLAYER_LOCAL, Window_UI::StartTwoPlayerLocal)
EVT_CHOICE(ID_CHOICE_PLAYER1_NAME, Window_UI::NewGameTwoPlayerChoicePlayer1)
EVT_CHOICE(ID_CHOICE_PLAYER2_NAME, Window_UI::NewGameTwoPlayerChoicePlayer2)
EVT_RADIOBOX(ID_RADIO_PLAYER1_COLOUR, Window_UI::NewGameTwoPlayerRadioPlayer1)
EVT_RADIOBOX(ID_RADIO_PLAYER2_COLOUR, Window_UI::NewGameTwoPlayerRadioPlayer2)
EVT_BUTTON(ID_SEND_REQUEST, Window_UI::SendRequest)
EVT_BUTTON(ID_CHECK_REQUEST, Window_UI::CheckRequest)
EVT_BUTTON(ID_LOGIN, Window_UI::Login)
EVT_BUTTON(ID_LOGIN_ACTION, Window_UI::Login_Action)
EVT_BUTTON(ID_FORGOT_USERNAME, Window_UI::ForgotUsername)
EVT_BUTTON(ID_FORGOT_PASSWORD, Window_UI::ForgotPassword)
EVT_TEXT(ID_LOGIN_TEXT, Window_UI::Login_Text)
EVT_TEXT_ENTER(ID_LOGIN_TEXT, Window_UI::Login_Action)
EVT_BUTTON(ID_LOGOUT, Window_UI::Logout)
EVT_BUTTON(ID_LOGOUT_ACTION, Window_UI::Logout_Action)
EVT_BUTTON(ID_CREATE_ACCOUNT, Window_UI::CreateAccount)
EVT_BUTTON(ID_CREATE_ACCOUNT_ACTION, Window_UI::CreateAccount_Action)
EVT_TEXT(ID_CREATE_ACCOUNT_TEXT, Window_UI::CreateAccount_Text)
EVT_TEXT_ENTER(ID_CREATE_ACCOUNT_TEXT, Window_UI::CreateAccount_Action)
EVT_BUTTON(ID_CHANGE_COLOUR, Window_UI::ChangeColour)
EVT_BUTTON(ID_ACCOUNT, Window_UI::Account)
EVT_LISTBOX(ID_GAME_TABLE, Window_UI::GameTable)
EVT_BUTTON(ID_STATS, Window_UI::Stats)
EVT_BUTTON(ID_CHANGE_PASSWORD, Window_UI::ChangePassword)
EVT_BUTTON(ID_DELETE_ACCOUNT, Window_UI::DeleteAccount)
EVT_BUTTON(ID_LOAD, Window_UI::Load)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(DrawPane, wxPanel)
EVT_LEFT_DOWN(DrawPane::Click)
EVT_PAINT(DrawPane::paintEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(PromotionPane, wxPanel)
EVT_LEFT_DOWN(PromotionPane::Click)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

/* --- FUNCTION DEFINTIONS --- */

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void ExitSound(void*){
	isExitSoundPlaying = true;
	if (!soundMute){
		PlaySound(TEXT("Sounds/Exit.wav"), NULL, SND_FILENAME);
	}
	isExitSoundPlaying = false;
}
// Encrypts text files used by the game to store data
void Encryption(wxString fileName){
	char name[100];
	strcpy(name, fileName.mb_str());
	char temp;
	int flag = 0;

	wxArrayString fileData;
	ifstream ifile(name, ios::in);
	while (!ifile.eof()){
		ifile.get(temp);
		temp = temp ^ flag % 100;
		flag++;
		fileData.Add(temp);
	}
	ifile.close();

	int size = fileData.GetCount();
	fileData.RemoveAt(size - 1);

	ofstream ofile(name, ios::out);
	for (int i = 0; i < size - 1; i++){
		ofile << fileData.Item(i);
	}
	ofile.close();
}
// Converts wxString to char* data type; Used for POSTing data to forms with cURL; Parameter requires char*
char* wxStringToCharStar(wxString &myString){
	const char* temp = (const char*)myString.mb_str();
	char* data = (char*)temp;
	return data;
}

void SplashBoot(void*){
	windowStart->Show(false);
	windowSplash->Show(true);

	wxMilliSleep(250);
	while (isLoginStartupWorking == true){
		wxMilliSleep(10);
	}

	int w, h;
	panelStart->GetSize(&w, &h);

	//wxImage TitlePic("Title.jpg", wxBITMAP_TYPE_JPEG);
	static wxImage NewGamePicOriginal("Images/NewGame.jpg", wxBITMAP_TYPE_JPEG);
	static wxImage LoadGamePicOriginal("Images/LoadGame.jpg", wxBITMAP_TYPE_JPEG);
	static wxImage MyAccountPicOriginal("Images/MyAccount.jpg", wxBITMAP_TYPE_JPEG);
	static wxImage LoginPicOriginal("Images/Login.jpg", wxBITMAP_TYPE_JPEG);
	static wxImage LogoutPicOriginal("Images/Logout.jpg", wxBITMAP_TYPE_JPEG);
	static wxImage NewAccountPicOriginal("Images/NewAccount.jpg", wxBITMAP_TYPE_JPEG);

	//TitlePic = TitlePic.Scale(w, h/7, wxIMAGE_QUALITY_HIGH);
	NewGamePic = NewGamePicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
	LoadGamePic = LoadGamePicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
	MyAccountPic = MyAccountPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
	LoginPic = LoginPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
	LogoutPic = LogoutPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
	NewAccountPic = NewAccountPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);

	//Title->SetImageLabel(TitlePic);
	NewGameButton->SetImageLabel(NewGamePic);
	LoadGameButton->SetImageLabel(LoadGamePic);
	MyAccountButton->SetImageLabel(MyAccountPic);
	LoginButton->SetImageLabel(LoginPic);
	LogoutButton->SetImageLabel(LogoutPic);
	NewAccountButton->SetImageLabel(NewAccountPic);

	windowStart->Show(true);
	windowSplash->Show(false);

	if (firstLaunch == 1){
		wxMessageBox("NEW\nNothing :(\n\nUPDATED\nNothing :(\n\nFIXED\nIssue where computer takes over and plays multiple moves in Easy mode", "What's New in Version 2.2.0");
		ofstream ofile("Settings/firstLaunch.setting");
		ofile << 0;
		ofile.close();
	}
}

/*MyApp Functions*/
bool MyApp::OnInit(){
	_beginthread(Account::ServerCheck, 0, NULL);
	_beginthread(Account::InternetCheck, 0, NULL);
	wxInitAllImageHandlers();
	icon.LoadFile("Icon.ico", wxBITMAP_TYPE_ICO, 32, 32);

	ofstream ofile("LoggedInPlayersTemp.txt", ios::out);
	ofile.close();
	ofile.open("Log.txt");
	ofile.close();
	ofile.open("move.txt");
	ofile.close();

	ifstream ifile;
	wxUint32 col;
	ifile.open("Settings/colour_light.setting");
	ifile >> col;
	ifile.close();
	colour_light.SetRGB(col);
	ifile.open("Settings/colour_dark.setting");
	ifile >> col;
	ifile.close();
	colour_dark.SetRGB(col);

	ifile.open("Settings/soundMute.setting");
	ifile >> soundMute;
	ifile.close();

	ifile.open("Settings/firstLaunch.setting");
	ifile >> firstLaunch;
	ifile.close();

	UIObj.Startup();
	UIObj.Splash();
	_beginthread(SplashBoot, 0, NULL);

	sessionState = NONE;

	if (!soundMute){
		PlaySound(TEXT("Sounds/Startup.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	isExitSoundPlaying = false;

	_beginthread(Account::LoginStartup, 0, NULL);
	_beginthread(Account::UpdateCheck, 0, NULL);
	return true;
}

/*SplashScreen Functions*/
SplashScreen::SplashScreen(const wxString& title) : wxFrame((wxFrame*)NULL, wxID_ANY, title){
}

wxMenu* controlMenu;
/*Window_Game Functions*/
Window_Game::Window_Game(const wxString& title) : wxFrame((wxFrame*)NULL, wxID_ANY, title){
	wxMenu* gameMenu = new wxMenu;
	wxMenu* viewMenu = new wxMenu;
	controlMenu = new wxMenu;
	wxMenu* helpMenu = new wxMenu;

	gameMenu->Append(ID_ABOUT, "&About\tAlt+A", "About the game", wxITEM_NORMAL);
	gameMenu->Append(ID_EXIT, "E&xit\tAlt+X", "Quit the game", wxITEM_NORMAL);
	viewMenu->AppendCheckItem(ID_BEGINNER_MODE, "Beginner Mode\tAlt+B", "Enables move highlighting.");
	viewMenu->AppendCheckItem(ID_BOARD_ROTATION, "Disable Board &Rotation\tAlt+R", "AKA Tablet Mode. Disable the rotation of chessboard for every turn.");
	controlMenu->AppendCheckItem(ID_TOUCH_PIECE, "Touch Piece\tAlt+T+P", "You can't change your piece once you select it.");
	//controlMenu->Append(3, "Replay", "Replay Game", wxITEM_NORMAL);
	//controlMenu->Append(4, "Save", "Save Game", wxITEM_NORMAL);
	//controlMenu->Append(5, "Load", "Load Game", wxITEM_NORMAL);
	helpMenu->Append(ID_CONNECTION_STATUS, "&Connection Status \tAlt+C", "Check connection status");
	helpMenu->Append(ID_SUPPORT_FEEDBACK, "Support and &Feedback \tAlt+F", "Contact us for support and feedback");
	helpMenu->Append(ID_WHATS_NEW, "&What's New \tAlt+W", "What's new in this update");
	helpMenu->Append(ID_HELP, "&Help\tAlt+H", "Help for OCTPP Chess");

	wxMenu* Settings = new wxMenu;
	wxMenu* Light = new wxMenu();
	wxMenu* Dark = new wxMenu();

	wxMenu* ChangeLight = new wxMenu();
	wxMenu* ChangeDark = new wxMenu();
	wxMenu* ResetLight = new wxMenu();
	wxMenu* ResetDark = new wxMenu();

	Light->Append(ID_SETTING_CHANGE_COLOUR_LIGHT, "Change LIGHT Colour", "Change the LIGHT colour");
	Light->Append(ID_SETTING_RESET_COLOUR_LIGHT, "Reset LIGHT Colour", "Reset the LIGHT colour to default");
	Dark->Append(ID_SETTING_CHANGE_COLOUR_DARK, "Change DARK Colour", "Change the DARK colour");
	Dark->Append(ID_SETTING_RESET_COLOUR_DARK, "Reset DARK Colour", "Reset the DARK colour to default");

	Settings->AppendSubMenu(Light, "LIGHT Colour");
	Settings->AppendSubMenu(Dark, "DARK Colour");

	Settings->AppendCheckItem(ID_SETTING_SOUND, "Mute Sounds", "Disable all sounds in the game");
	if (soundMute){
		Settings->Check(ID_SETTING_SOUND, true);
	}
	else {
		Settings->Check(ID_SETTING_SOUND, false);
	}

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(gameMenu, "&Game");
	menuBar->Append(viewMenu, "&View");
	menuBar->Append(controlMenu, "&Control");
	menuBar->Append(Settings, "&Settings");
	menuBar->Append(helpMenu, "&Help");
	SetMenuBar(menuBar);

	if (sessionState == ONE_PLAYER){
		viewMenu->Enable(ID_BOARD_ROTATION, false);
	}
	else if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 || sessionState == TWO_PLAYER_INTERNET_PLAYER_2){
		controlMenu->Enable(ID_TOUCH_PIECE, false);
	}
}
void Window_Game::Rotation(wxCommandEvent& WXUNUSED(event)){
	board_rotation = !board_rotation;
	drawPane->Reload();
}
void Window_Game::TouchPiece(wxCommandEvent& WXUNUSED(event)){
	touch_piece = !touch_piece;
	if (highlighted_x != 0 && highlighted_y != 0){
		int x = highlighted_x, y = highlighted_y;
		highlighted_x = highlighted_y = 0;
		RealGame.game[x][y].Draw(MODE_DRAW_NORMAL);
	}
}
void Window_Game::Beginner(wxCommandEvent& WXUNUSED(event)){
	beginnerMode = !beginnerMode;
}
void Window_Game::Replay(wxCommandEvent& WXUNUSED(event)){
	_beginthread(Game::Replay, 0, NULL);
}
void Window_Game::Save(wxCommandEvent& WXUNUSED(event)){
	Game::Save();
}
void Window_Game::Load(wxCommandEvent& WXUNUSED(event)){
	Game::Load();
}
void Window_Game::QuitGame(wxCommandEvent& WXUNUSED(event)){
	wxMessageDialog message(NULL, "Are you sure you want to quit?", "Chess | Alert", wxOK | wxCANCEL | wxICON_NONE);
	message.SetOKCancelLabels("Yes", "No");
	int returnValue = message.ShowModal();
	if (returnValue != wxID_OK){
		return;
	}
	RealGame.QuitGame();
}

/*Window_UI Functions*/
void Window_UI::paintEvent(wxPaintEvent &evt){
	if (windowStart->IsActive()){
		static int a = 0, b = 0;
		int w, h;
		panelStart->GetSize(&w, &h);

		//wxImage TitlePic("Title.jpg", wxBITMAP_TYPE_JPEG);
		static wxImage NewGamePicOriginal("Images/NewGame.jpg", wxBITMAP_TYPE_JPEG);
		static wxImage LoadGamePicOriginal("Images/LoadGame.jpg", wxBITMAP_TYPE_JPEG);
		static wxImage MyAccountPicOriginal("Images/MyAccount.jpg", wxBITMAP_TYPE_JPEG);
		static wxImage LoginPicOriginal("Images/Login.jpg", wxBITMAP_TYPE_JPEG);
		static wxImage LogoutPicOriginal("Images/Logout.jpg", wxBITMAP_TYPE_JPEG);
		static wxImage NewAccountPicOriginal("Images/NewAccount.jpg", wxBITMAP_TYPE_JPEG);

		if (a != w && b != h){
			//TitlePic = TitlePic.Scale(w, h/7, wxIMAGE_QUALITY_HIGH);
			NewGamePic = NewGamePicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
			LoadGamePic = LoadGamePicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
			MyAccountPic = MyAccountPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
			LoginPic = LoginPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
			LogoutPic = LogoutPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);
			NewAccountPic = NewAccountPicOriginal.Scale(w / 6, h, wxIMAGE_QUALITY_HIGH);

			//Title->SetImageLabel(TitlePic);
			NewGameButton->SetImageLabel(NewGamePic);
			LoadGameButton->SetImageLabel(LoadGamePic);
			MyAccountButton->SetImageLabel(MyAccountPic);
			LoginButton->SetImageLabel(LoginPic);
			LogoutButton->SetImageLabel(LogoutPic);
			NewAccountButton->SetImageLabel(NewAccountPic);
		}
		a = w, b = h;
	}
}
Window_UI::Window_UI(const wxString& title) : wxFrame((wxFrame*)NULL, wxID_ANY, title){
	wxMenu* Menu = new wxMenu;
	Menu->Append(ID_ABOUT, "&About \tAlt+A", "About the game", wxITEM_NORMAL);
	Menu->Append(ID_EXIT, "E&xit \tAlt+X", "Quit the game", wxITEM_NORMAL);

	wxMenu* Settings = new wxMenu;
	wxMenu* Light = new wxMenu();
	wxMenu* Dark = new wxMenu();

	wxMenu* ChangeLight = new wxMenu();
	wxMenu* ChangeDark = new wxMenu();
	wxMenu* ResetLight = new wxMenu();
	wxMenu* ResetDark = new wxMenu();

	Light->Append(ID_SETTING_CHANGE_COLOUR_LIGHT, "Change LIGHT Colour", "Change the LIGHT colour");
	Light->Append(ID_SETTING_RESET_COLOUR_LIGHT, "Reset LIGHT Colour", "Reset the LIGHT colour to default");
	Dark->Append(ID_SETTING_CHANGE_COLOUR_DARK, "Change DARK Colour", "Change the DARK colour");
	Dark->Append(ID_SETTING_RESET_COLOUR_DARK, "Reset DARK Colour", "Reset the DARK colour to default");

	Settings->AppendSubMenu(Light, "LIGHT Colour");
	Settings->AppendSubMenu(Dark, "DARK Colour");

	Settings->AppendCheckItem(ID_SETTING_SOUND, "Mute Sounds", "Disable all sounds in the game");
	if (soundMute){
		Settings->Check(ID_SETTING_SOUND, true);
	}
	else {
		Settings->Check(ID_SETTING_SOUND, false);
	}

	wxMenu* Help = new wxMenu;
	Help->Append(ID_CONNECTION_STATUS, "&Connection Status \tAlt+C", "Check connection status");
	Help->Append(ID_SUPPORT_FEEDBACK, "Support and &Feedback \tAlt+F", "Contact us for support and feedback");
	Help->Append(ID_WHATS_NEW, "&What's New \tAlt+W", "What's new in this update");
	Help->Append(ID_HELP, "&Help\tAlt+H", "Help for OCTPP Chess");

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(Menu, "&Game");
	menuBar->Append(Settings, "&Settings");
	menuBar->Append(Help, "&Help");
	SetMenuBar(menuBar);
}
void Window_UI::OnQuit(wxCommandEvent& WXUNUSED(event)){
	Game::Shutdown();
}
void Window_UI::About(wxCommandEvent& WXUNUSED(event)){
	wxMessageBox("OCTPP Chess 2015 \n\nCreated by DIGS Game Re-creation Studios \nDevelopers: Abhishek Krishna, Inavamsi BE, Sesh Sadasivam \nVersion: 2.2.0 \nBuild Number: 202", "Chess | About", wxICON_NONE);
}
void Window_UI::ConnectionStatus(wxCommandEvent& WXUNUSED(event)){
	wxString message = "Internet: ";
	message += internetConnected == true ? "Connected" : "Not connected";
	message += "\nServer: ";
	message += serverConnected == true ? "Connected" : "Not connected";
	wxMessageBox(message, "Chess | Connection Status");
}
void Window_UI::SupportAndFeedback(wxCommandEvent& WXUNUSED(event)){
	ShellExecute(0, 0, L"http://www.octpp.tk/#support", 0, 0, SW_SHOW);
}
void Window_UI::WhatsNew(wxCommandEvent& WXUNUSED(event)){
	wxMessageBox("NEW\nNothing :(\n\nUPDATED\nNothing :(\n\nFIXED\nIssue where computer takes over and plays multiple moves in Easy mode", "What's New in Version 2.2.0");
}
void Window_UI::Help(wxCommandEvent& WXUNUSED(event)){
	ShellExecute(0, 0, L"help.html", 0, 0, SW_SHOW);
}
void Window_UI::NewGame(wxCommandEvent& WXUNUSED(event)){
	UIObj.NewGame();
	windowNewGame->Show(true);
	windowStart->Show(false);
}
void Window_UI::NewGameOnePlayer(wxCommandEvent& WXUNUSED(event)){
	UIObj.NewGameOnePlayer();
	windowNewGameOnePlayer->Show(true);
	windowNewGame->Show(false);
}
void Window_UI::StartOnePlayer(wxCommandEvent& WXUNUSED(event)){
	RealGame.StartOnePlayer();
	UIObj.StartOnePlayer();
	windowGame->Show(true);
	windowNewGameOnePlayer->Show(false);
	_beginthread(AI::FirstMove, 0, NULL);
}
void Window_UI::NewGameTwoPlayer(wxCommandEvent& WXUNUSED(event)){
	UIObj.NewGameTwoPlayer();
	windowNewGameTwoPlayer->Show(true);
	windowNewGame->Show(false);
}
void Window_UI::NewGameTwoPlayerLocal(wxCommandEvent& WXUNUSED(event)){
	UIObj.NewGameTwoPlayerLocal();
	windowNewGameTwoPlayerLocal->Show(true);
	windowNewGameTwoPlayer->Show(false);
}
void Window_UI::NewGameTwoPlayerOnline(wxCommandEvent& WXUNUSED(event)){
	if (internetConnected == false){
		wxMessageBox("No internet connection!", "Chess Alert");
		return;
	}
	if (serverConnected == false){
		wxMessageBox("No server connection!", "Chess Alert");
		return;
	}
	UIObj.NewGameTwoPlayerOnline();
	if (windowNewGameTwoPlayerOnline != NULL){	// If the window was created...
		windowNewGameTwoPlayerOnline->Show(true);
		windowNewGameTwoPlayer->Show(false);
	}
}
void Window_UI::StartTwoPlayerLocal(wxCommandEvent& WXUNUSED(event)) {
	RealGame.StartTwoPlayerLocal();
	UIObj.StartTwoPlayerLocal();
	windowGame->Show(true);
	windowNewGameTwoPlayerLocal->Show(false);
}
void Window_UI::NewGameTwoPlayerRadioPlayer1(wxCommandEvent& WXUNUSED(event)){
	if (UIObj.Player1Colour->GetSelection() == 0){
		UIObj.Player2Colour->SetSelection(1);
	}
	else {
		UIObj.Player2Colour->SetSelection(0);
	}
}
void Window_UI::NewGameTwoPlayerRadioPlayer2(wxCommandEvent& WXUNUSED(event)){
	if (UIObj.Player2Colour->GetSelection() == 0){
		UIObj.Player1Colour->SetSelection(1);
	}
	else {
		UIObj.Player1Colour->SetSelection(0);
	}
}
void Window_UI::NewGameTwoPlayerChoicePlayer1(wxCommandEvent& WXUNUSED(event)){
	if (UIObj.NewTwoPlayerUser1->GetSelection() == UIObj.NewTwoPlayerUser2->GetSelection()){
		UIObj.NewTwoPlayerUser2->SetSelection(0);
	}
}
void Window_UI::NewGameTwoPlayerChoicePlayer2(wxCommandEvent& WXUNUSED(event)){
	if (UIObj.NewTwoPlayerUser1->GetSelection() == UIObj.NewTwoPlayerUser2->GetSelection()){
		UIObj.NewTwoPlayerUser1->SetSelection(0);
	}
}
void Window_UI::SendRequest(wxCommandEvent& WXUNUSED(event)){
	AccountObj.SendRequest();
}
void Window_UI::CheckRequest(wxCommandEvent& WXUNUSED(event)){
	AccountObj.CheckRequest();
}
void Window_UI::LoadGame(wxCommandEvent& WXUNUSED(event)){

}
void Window_UI::Login(wxCommandEvent& WXUNUSED(event)){
	if (internetConnected == false){
		wxMessageBox("No internet connection!", "Chess Alert");
		return;
	}
	if (serverConnected == false){
		wxMessageBox("No server connection!", "Chess Alert");
		return;
	}
	UIObj.Login();
	windowLogin->Show(true);
	windowStart->Show(false);
}
void Window_UI::Login_Action(wxCommandEvent& WXUNUSED(event)){
	AccountObj.Login();
}
void Window_UI::ForgotUsername(wxCommandEvent& WXUNUSED(event)){
	wxString emailID = wxGetTextFromUser("Enter your email ID and we will send you your username.", "Chess | Forgot Username", "");
	if (emailID == ""){
		return;
	}
	Account::ForgotUsername(emailID);
}
void Window_UI::ForgotPassword(wxCommandEvent& WXUNUSED(event)){
	wxString Username = "";
	Username = wxGetTextFromUser("Enter your username and we will send your password to your email ID.", "Chess | Forgot Password");
	if (Username == ""){
		return;
	}
	Account::ForgotPassword(Username);
}
void Window_UI::Logout(wxCommandEvent& WXUNUSED(event)){
	if (internetConnected == false){
		wxMessageBox("No internet connection!", "Chess Alert");
		return;
	}
	if (serverConnected == false){
		wxMessageBox("No server connection!", "Chess Alert");
		return;
	}
	UIObj.Logout();
	if (windowLogout != NULL){	// If the window was created...
		windowLogout->Show(true);
		windowStart->Show(false);
	}
}
void Window_UI::Login_Text(wxCommandEvent& WXUNUSED(event)){
	wxString temp = UIObj.Login_Username_field->GetLineText(0);
	temp = temp.Upper();
	UIObj.Login_Username_field->ChangeValue(temp);
	UIObj.Login_Username_field->SetInsertionPointEnd();
}
void Window_UI::Logout_Action(wxCommandEvent& WXUNUSED(event)){
	AccountObj.Logout();
	windowStart->Show(true);
	windowLogout->Show(false);
}
void Window_UI::CreateAccount(wxCommandEvent& WXUNUSED(event)){
	if (internetConnected == false){
		wxMessageBox("No internet connection!", "Chess Alert");
		return;
	}
	if (serverConnected == false){
		wxMessageBox("No server connection!", "Chess Alert");
		return;
	}
	UIObj.CreateAccount();
	windowCreateAccount->Show(true);
	windowStart->Show(false);
}
void Window_UI::CreateAccount_Text(wxCommandEvent& WXUNUSED(event)){
	wxString temp = UIObj.CreateAccount_Username_field->GetLineText(0);
	temp = temp.Upper();
	UIObj.CreateAccount_Username_field->ChangeValue(temp);
	UIObj.CreateAccount_Username_field->SetInsertionPointEnd();
}
void Window_UI::CreateAccount_Action(wxCommandEvent& WXUNUSED(event)){
	AccountObj.CreateAccount();
}
void Window_UI::Account(wxCommandEvent& WXUNUSED(event)){
	UIObj.Account();
	if (windowAccount){
		windowAccount->Show(true);
		windowStart->Show(false);
	}
}
void Window_UI::GameTable(wxCommandEvent& event){
	wxObject *obj = event.GetEventObject();
	if (UIObj.GameTable_Number == obj){
		int n = UIObj.GameTable_Number->GetSelection();
		if (n == 0){
			n = UIObj.GameTable_Name->GetSelection();
			UIObj.GameTable_Number->SetSelection(n);
			return;
		}
		UIObj.GameTable_Name->SetSelection(n);
		UIObj.GameTable_Player->SetSelection(n);
	}
	else if (UIObj.GameTable_Name == obj){
		int n = UIObj.GameTable_Name->GetSelection();
		if (n == 0){
			n = UIObj.GameTable_Player->GetSelection();
			UIObj.GameTable_Name->SetSelection(n);
			return;
		}
		UIObj.GameTable_Number->SetSelection(n);
		UIObj.GameTable_Player->SetSelection(n);
	}
	else if (UIObj.GameTable_Player == obj){
		int n = UIObj.GameTable_Player->GetSelection();
		if (n == 0){
			n = UIObj.GameTable_Name->GetSelection();
			UIObj.GameTable_Player->SetSelection(n);
			return;
		}
		UIObj.GameTable_Number->SetSelection(n);
		UIObj.GameTable_Name->SetSelection(n);
	}
}
void Window_UI::Stats(wxCommandEvent& WXUNUSED(event)){
	wxString URL = "http://www.octpp.tk/stats.php?q=";
	URL += UIObj.AccountUsers->GetStringSelection();
	ShellExecute(0, 0, URL, 0, 0, SW_SHOW);
}
void Window_UI::ChangePassword(wxCommandEvent& WXUNUSED(event)){
	wxString password = wxGetTextFromUser("Enter your new password.", "Chess | Change Password Alert");
	if (password == ""){
		wxMessageBox("Password cannot be blank! Password not changed.", "Chess | Change Password Alert");
		return;
	}
	Account::ChangePassword(password);
}
void Window_UI::ChangeColour(wxCommandEvent& WXUNUSED(event)){
	wxColour colour = wxGetColourFromUser(NULL, wxNullColour, "Pick your colour");
}
void Window_UI::DeleteAccount(wxCommandEvent& WXUNUSED(event)){
	wxMessageDialog message(NULL, "Are you sure you want to delete your account?", "Chess | Delete Account", wxOK | wxCANCEL | wxICON_NONE);
	message.SetOKCancelLabels("Yes", "No");
	int returnValue = message.ShowModal();
	if (returnValue != wxID_OK){
		return;
	}
	Account::DeleteAccount(UIObj.AccountUsers->GetStringSelection());
}
void Window_UI::Load(wxCommandEvent& WXUNUSED(event)){
	wxMessageBox("Coming soon!");
	//UIObj.Load();
}
void Window_UI::Home(wxCommandEvent& WXUNUSED(event)){
	isExit = false;
	windowStart->Show(true);
	if (windowNewGame) {
		delete windowNewGame;
		windowNewGame = NULL;
	}
	if (windowNewGameOnePlayer)	{
		delete windowNewGameOnePlayer;
		windowNewGameOnePlayer = NULL;
	}
	if (windowNewGameTwoPlayer) {
		delete windowNewGameTwoPlayer;
		windowNewGameTwoPlayer = NULL;
	}
	if (windowNewGameTwoPlayerLocal) {
		delete windowNewGameTwoPlayerLocal;
		windowNewGameTwoPlayerLocal = NULL;
	}
	if (windowNewGameTwoPlayerOnline) {
		delete windowNewGameTwoPlayerOnline;
		windowNewGameTwoPlayerOnline = NULL;
	}
	if (windowLogin) {
		delete windowLogin;
		windowLogin = NULL;
	}
	if (windowLogout) {
		delete windowLogout;
		windowLogout = NULL;
	}
	if (windowCreateAccount) {
		delete windowCreateAccount;
		windowCreateAccount = NULL;
	}
	if (windowAccount) {
		delete windowAccount;
		windowAccount = NULL;
	}
	if (windowLoad) {
		delete windowLoad;
		windowLoad = NULL;
	}
	if (windowAccount) {
		delete windowGame;
		windowGame = NULL;
	}
	isExit = true;
}
void Window_UI::Setting_ChangeColourLight(wxCommandEvent& WXUNUSED(event)){
	wxColour tempColour = wxGetColourFromUser(NULL, wxNullColour, "Pick a LIGHT colour");
	if (tempColour == wxNullColour){
		return;
	}
	colour_light = tempColour;
	ofstream ofile("Settings/colour_light.setting");
	ofile << colour_light.GetRGB();
	ofile.close();
}
void Window_UI::Setting_ChangeColourDark(wxCommandEvent& WXUNUSED(event)){
	wxColour tempColour = wxGetColourFromUser(NULL, wxNullColour, "Pick a DARK colour");
	if (tempColour == wxNullColour){
		return;
	}
	colour_dark = tempColour;
	ofstream ofile("Settings/colour_dark.setting");
	ofile << colour_dark.GetRGB();
	ofile.close();
}
void Window_UI::Setting_ResetColourLight(wxCommandEvent& WXUNUSED(event)){
	colour_light = wxColour(0, 240, 240);
	ofstream ofile("Settings/colour_light.setting");
	ofile << colour_light.GetRGB();
	ofile.close();
	wxMessageBox("Changed!");
}
void Window_UI::Setting_ResetColourDark(wxCommandEvent& WXUNUSED(event)){
	colour_dark = wxColour(220, 20, 20);
	ofstream ofile("Settings/colour_dark.setting");
	ofile << colour_dark.GetRGB();
	ofile.close();
	wxMessageBox("Changed!");
}
void Window_UI::Sounds(wxCommandEvent& WXUNUSED(event)){
	if (soundMute == 1){
		soundMute = 0;
	}
	else {
		soundMute = 1;
	}
	ofstream ofile("Settings/soundMute.setting");
	ofile << soundMute;
	ofile.close();
}

/*DrawPane Functions*/
DrawPane::DrawPane(wxFrame* parent) : wxPanel(parent){
}
void DrawPane::paintEvent(wxPaintEvent &evt){
	Reload();
}
void DrawPane::Reload(){
	wxClientDC *dc;
	dc = new wxClientDC(drawPane);
	dc->SetBackground(wxColor(40, 40, 40));
	dc->Clear();
	drawPane->Units();
	Chessboard();
	delete dc;

	dc = new wxClientDC(killedPane);
	killedPane->Units();
	dc->SetBackground(wxColor(40, 40, 40));
	dc->Clear();
	killedPane->Killed();
	delete dc;

	dc = new wxClientDC(promotionPane);
	dc->SetBackground(wxColor(40, 40, 40));
	dc->Clear();
	delete dc;

	if (sessionState == ONE_PLAYER){
		if (Player1.Colour == LIGHT){	// If AI is playing Light
			drawPane->Markings(2);
		}
		else {
			drawPane->Markings(1);
		}
	}
	else if (sessionState == TWO_PLAYER_LOCAL){
		if (board_rotation == false){
			drawPane->Markings(1);
		}
		else {
			if (Player1.Colour == RealGame.turn){
				drawPane->Markings(1);
			}
			else {
				drawPane->Markings(2);
			}
		}
	}
	else if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1){
		if (Player1.Colour == DARK){
			drawPane->Markings(2);
		}
		else {
			drawPane->Markings(1);
		}
	}
	else if (sessionState == TWO_PLAYER_INTERNET_PLAYER_2){
		if (Player2.Colour == DARK){
			drawPane->Markings(2);
		}
		else {
			drawPane->Markings(1);
		}
	}
}
void DrawPane::Click(wxMouseEvent &evt){
	if (promotionPane->promotion_state == true || GameEnded == true){
		return;
	}
	wxClientDC dc(drawPane);
	// Gets the clicked coordinates
	int xClick = evt.GetX(), yClick = evt.GetY();

	// Converts to grid coordinates
	xClick += drawPane->square; yClick += drawPane->square;
	xClick -= x - (4 * drawPane->square);
	yClick -= y - (4 * drawPane->square);
	xClick /= drawPane->square;
	yClick /= drawPane->square;
	// Returns if the click is outside the board
	if (xClick < 1 || xClick > 8 || yClick < 1 || yClick > 8){
		return;
	}
	// Disable click in AI turn
	if (sessionState == ONE_PLAYER && Player1.Colour == RealGame.turn){
		return;
	}
	// Disable click in two player online when its the other person's turn
	if ((sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && RealGame.turn == Player2.Colour) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && RealGame.turn == Player1.Colour)){
		return;
	}
	Game::Control(xClick, yClick, 1);
}
void DrawPane::Units(){
	wxClientDC dc(drawPane);
	int xCo, yCo;
	dc.GetSize(&xCo, &yCo);
	drawPane->unit = xCo / 96;	//Designed for 16x9 aspect ratio screens. Therefore there are 96 units horizontally, 54 vertically.
	y = yCo / 2;
	x = xCo / 2;
	drawPane->unit *= (float)yCo / (50 * drawPane->unit);
	drawPane->square = 5 * drawPane->unit;
}
void DrawPane::Chessboard(){
	wxClientDC dc(drawPane);
	dc.SetPen(wxPen(wxColour(150, 150, 150), 0));	//Grey colour for the border
	int lines = 1;		//Lines of the board printed
	int squares = 1;		//Total squares printed
	for (int i = -4; i < 4; i++){
		for (int j = -4; j < 4; j++){
			if (lines % 2 == 1){							//Setting color based on current line and square
				if (squares % 2 == 0)dc.SetBrush(*wxBLACK_BRUSH);
				else dc.SetBrush(*wxWHITE_BRUSH);
			}
			else if (lines % 2 == 0){
				if (squares % 2 == 0)dc.SetBrush(*wxWHITE_BRUSH);
				else dc.SetBrush(*wxBLACK_BRUSH);
			}
			dc.DrawRectangle(x + (drawPane->square*i), y + (drawPane->square*j), drawPane->square, drawPane->square);
			squares++;
		}
		lines++;
	}

	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			RealGame.game[i][j].Draw(MODE_DRAW_NORMAL);
			if (isCheck == true && i == xking && j == yking){
				RealGame.game[i][j].Draw(MODE_DRAW_CHECK);
			}
		}
	}
}
int DrawPane::convertx(int gridx){
	return (x - 5 * drawPane->square) + (gridx * drawPane->square) + (0.5 * drawPane->square);
}
int DrawPane::converty(int gridy){
	return (y - 5 * drawPane->square) + (gridy * drawPane->square) + (0.5 * drawPane->square);
}
void DrawPane::Markings(int mode){
	//1 for normal (light at bottom), 2 for reverse (light at top)
	wxClientDC dc(drawPane);
	dc.SetTextForeground(wxColour(255, 255, 255));
	if (mode == 1){
		dc.DrawText("8", x - 22 * drawPane->unit, y - 18 * drawPane->unit);
		dc.DrawText("7", x - 22 * drawPane->unit, y - 13 * drawPane->unit);
		dc.DrawText("6", x - 22 * drawPane->unit, y - 8 * drawPane->unit);
		dc.DrawText("5", x - 22 * drawPane->unit, y - 3 * drawPane->unit);
		dc.DrawText("4", x - 22 * drawPane->unit, y + 2 * drawPane->unit);
		dc.DrawText("3", x - 22 * drawPane->unit, y + 7 * drawPane->unit);
		dc.DrawText("2", x - 22 * drawPane->unit, y + 12 * drawPane->unit);
		dc.DrawText("1", x - 22 * drawPane->unit, y + 17 * drawPane->unit);

		dc.DrawText("8", x + 21 * drawPane->unit, y - 18 * drawPane->unit);
		dc.DrawText("7", x + 21 * drawPane->unit, y - 13 * drawPane->unit);
		dc.DrawText("6", x + 21 * drawPane->unit, y - 8 * drawPane->unit);
		dc.DrawText("5", x + 21 * drawPane->unit, y - 3 * drawPane->unit);
		dc.DrawText("4", x + 21 * drawPane->unit, y + 2 * drawPane->unit);
		dc.DrawText("3", x + 21 * drawPane->unit, y + 7 * drawPane->unit);
		dc.DrawText("2", x + 21 * drawPane->unit, y + 12 * drawPane->unit);
		dc.DrawText("1", x + 21 * drawPane->unit, y + 17 * drawPane->unit);

		dc.DrawText("a", x - 18 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("b", x - 13 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("c", x - 8 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("d", x - 3 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("e", x + 2 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("f", x + 7 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("g", x + 12 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("h", x + 17 * drawPane->unit, y - 22 * drawPane->unit);

		dc.DrawText("a", x - 18 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("b", x - 13 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("c", x - 8 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("d", x - 3 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("e", x + 2 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("f", x + 7 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("g", x + 12 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("h", x + 17 * drawPane->unit, y + 21 * drawPane->unit);
	}
	else {
		dc.DrawText("1", x - 22 * drawPane->unit, y - 18 * drawPane->unit);
		dc.DrawText("2", x - 22 * drawPane->unit, y - 13 * drawPane->unit);
		dc.DrawText("3", x - 22 * drawPane->unit, y - 8 * drawPane->unit);
		dc.DrawText("4", x - 22 * drawPane->unit, y - 3 * drawPane->unit);
		dc.DrawText("5", x - 22 * drawPane->unit, y + 2 * drawPane->unit);
		dc.DrawText("6", x - 22 * drawPane->unit, y + 7 * drawPane->unit);
		dc.DrawText("7", x - 22 * drawPane->unit, y + 12 * drawPane->unit);
		dc.DrawText("8", x - 22 * drawPane->unit, y + 17 * drawPane->unit);

		dc.DrawText("1", x + 21 * drawPane->unit, y - 18 * drawPane->unit);
		dc.DrawText("2", x + 21 * drawPane->unit, y - 13 * drawPane->unit);
		dc.DrawText("3", x + 21 * drawPane->unit, y - 8 * drawPane->unit);
		dc.DrawText("4", x + 21 * drawPane->unit, y - 3 * drawPane->unit);
		dc.DrawText("5", x + 21 * drawPane->unit, y + 2 * drawPane->unit);
		dc.DrawText("6", x + 21 * drawPane->unit, y + 7 * drawPane->unit);
		dc.DrawText("7", x + 21 * drawPane->unit, y + 12 * drawPane->unit);
		dc.DrawText("8", x + 21 * drawPane->unit, y + 17 * drawPane->unit);

		dc.DrawText("h", x - 18 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("g", x - 13 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("f", x - 8 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("e", x - 3 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("d", x + 2 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("c", x + 7 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("b", x + 12 * drawPane->unit, y - 22 * drawPane->unit);
		dc.DrawText("a", x + 17 * drawPane->unit, y - 22 * drawPane->unit);

		dc.DrawText("h", x - 18 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("g", x - 13 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("f", x - 8 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("e", x - 3 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("d", x + 2 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("c", x + 7 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("b", x + 12 * drawPane->unit, y + 21 * drawPane->unit);
		dc.DrawText("a", x + 17 * drawPane->unit, y + 21 * drawPane->unit);
	}
}

/*KilledPane Functions*/
KilledPane::KilledPane(wxFrame* parent) : wxPanel(parent){
}
void KilledPane::Killed(){
	//White
	killedPane->kill_colour = LIGHT;
	for (int i = 0; i <= 15; i++){
		if (killed_white[i] != EMPTY){
			killedPane->kill_no = i;
			killedPane->killed_piece = killed_white[i];
			RealGame.game[0][0].Draw(MODE_DRAW_KILLED);
		}
		else break;
	}
	//Black
	killedPane->kill_colour = DARK;
	for (int i = 0; i <= 15; i++){
		if (killed_black[i] != EMPTY){
			killedPane->kill_no = i;
			killedPane->killed_piece = killed_black[i];
			RealGame.game[0][0].Draw(MODE_DRAW_KILLED);
		}
		else break;
	}
}
void KilledPane::Units(){
	wxClientDC dc(killedPane);
	int xCo, yCo;
	dc.GetSize(&xCo, &yCo);
	killedPane->unit = yCo / 80;
	killedPane->square = 5 * killedPane->unit;
	killedPane->width = xCo;
	killedPane->height = yCo;
}

/*PromotionPane Functions*/
PromotionPane::PromotionPane(wxFrame* parent) : wxPanel(parent){
	promotion_state = false;
}
void PromotionPane::Click(wxMouseEvent &evt){
	if (promotionPane->promotion_state == false){
		return;
	}
	if ((sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && RealGame.turn == Player1.Colour) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && RealGame.turn == Player2.Colour)){
		return;
	}
	wxClientDC dc(promotionPane);
	dc.SetBackground(wxColor(50, 50, 50));
	//Gets the clicked coordinates
	int xClick = evt.GetX(), yClick = evt.GetY();
	promotionPane->Units();

	yClick += promotionPane->square * 0.5;
	yClick /= promotionPane->square;
	RealGame.PawnPromotion(yClick);
}
void PromotionPane::Units(){
	wxClientDC dc(promotionPane);
	int xCo, yCo;
	dc.GetSize(&xCo, &yCo);
	promotionPane->unit = xCo / 5;
	promotionPane->square = 5 * promotionPane->unit;
}

/*Square Functions*/
void Square::Draw(int mode){	//Parameter is 0 to draw, 1 to highlight, 2 for killed pieces, 3 for pawn promotion, 4 for beginner mode, 5 for check
	wxClientDC* dc;
	int unit_draw = drawPane->unit, square_draw = drawPane->square;
	int x_center, y_center;
	piece_code piece_draw = piece;

	if (mode == MODE_DRAW_KILLED){
		dc = new wxClientDC(killedPane);
		x_center = killedPane->kill_colour == LIGHT ? killedPane->width * 0.25 : killedPane->width * 0.75;
		y_center = (killedPane->square / 2) + killedPane->kill_no * killedPane->square;
		unit_draw = killedPane->unit;
		square_draw = killedPane->square;

		if (killedPane->kill_colour == LIGHT){
			dc->SetPen(wxPen(colour_light, 0));
			dc->SetBrush(colour_light);
		}
		else{
			dc->SetPen(wxPen(colour_dark, 0));
			dc->SetBrush(colour_dark);
		}
		piece_draw = killedPane->killed_piece;
	}
	else if (mode == MODE_DRAW_PROMOTION){
		dc = new wxClientDC(promotionPane);
		x_center = promotionPane->square / 2;
		y_center = promotionPane->piece_number * promotionPane->square;
		unit_draw = promotionPane->unit;
		square_draw = promotionPane->square;

		if (promotionPane->promote_colour == LIGHT){
			dc->SetPen(wxPen(colour_light, 0));
			dc->SetBrush(colour_light);
		}
		else{
			dc->SetPen(wxPen(colour_dark, 0));
			dc->SetBrush(colour_dark);
		}
		piece_draw = promotionPane->promote_piece;
	}
	else if (mode == MODE_DRAW_BEGINNER){
		dc = new wxClientDC(drawPane);
		x_center = drawPane->convertx(x);
		y_center = drawPane->converty(y);
		unit_draw = drawPane->unit;
		square_draw = drawPane->square;

		//Redraws the square
		if (RealGame.turn == LIGHT){
			dc->SetPen(wxPen(colour_light));
			dc->SetBrush(wxBrush(colour_light, wxBRUSHSTYLE_CROSSDIAG_HATCH));
		}
		else{
			dc->SetPen(wxPen(colour_dark));
			dc->SetBrush(wxBrush(colour_dark, wxBRUSHSTYLE_CROSSDIAG_HATCH));
		}

		if ((sessionState != ONE_PLAYER && RealGame.turn == DARK && board_rotation == true) || (sessionState == ONE_PLAYER && Player1.Colour == LIGHT) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && Player1.Colour == DARK) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && Player2.Colour == DARK)){
			x = 9 - x; y = 9 - y;
		}
		x_center = drawPane->convertx(x), y_center = drawPane->converty(y);
		dc->DrawRectangle(x_center - 0.5 * drawPane->square, y_center - 0.5 * drawPane->square, drawPane->square, drawPane->square);

		//For the piece
		if (colour == LIGHT){	//Sets the colour of the piece depending on colour of the piece
			dc->SetPen(wxPen(colour_light, 0));
			dc->SetBrush(colour_light);
		}
		else {
			dc->SetPen(wxPen(colour_dark, 0));
			dc->SetBrush(colour_dark);
		}
	}
	else if (mode == MODE_DRAW_NORMAL || mode == MODE_DRAW_HIGHLIGHT || mode == MODE_DRAW_CHECK){
		dc = new wxClientDC(drawPane);
		x_center = drawPane->convertx(x);
		y_center = drawPane->converty(y);
		unit_draw = drawPane->unit;
		square_draw = drawPane->square;

		//Redraws the square
		if ((x + y) % 2 == 0){
			dc->SetBrush(*wxWHITE_BRUSH);
		}
		else {
			dc->SetBrush(*wxBLACK_BRUSH);
		}
		if (mode == MODE_DRAW_HIGHLIGHT){		//For highlighting the square, grey square background
			dc->SetBrush(*wxGREY_BRUSH);
			highlight = true;
			highlighted_x = x;
			highlighted_y = y;
		}
		else if (mode == MODE_DRAW_NORMAL){
			highlight = false;
			highlighted_x = highlighted_y = 0;
		}
		if (mode == MODE_DRAW_CHECK){
			wxColour tempColour = RealGame.turn == LIGHT ? colour_dark : colour_light;
			wxBrush tempBrush(tempColour, wxBRUSHSTYLE_SOLID);
			dc->SetBrush(tempBrush);
		}
		if (RealGame.turn == LIGHT){
			dc->SetPen(wxPen(colour_light));
		}
		else{
			dc->SetPen(wxPen(colour_dark));
		}

		if ((sessionState != ONE_PLAYER && RealGame.turn == DARK && board_rotation == true) || (sessionState == ONE_PLAYER && Player1.Colour == LIGHT) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && Player1.Colour == DARK) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && Player2.Colour == DARK)){
			x = 9 - x; y = 9 - y;
		}
		x_center = drawPane->convertx(x), y_center = drawPane->converty(y);
		dc->DrawRectangle(x_center - 0.5 * drawPane->square, y_center - 0.5 * drawPane->square, drawPane->square, drawPane->square);

		//For the piece
		if (colour == LIGHT){	//Sets the colour of the piece depending on colour of the piece
			dc->SetPen(wxPen(colour_light, 0));
			dc->SetBrush(colour_light);
		}
		else {
			dc->SetPen(wxPen(colour_dark, 0));
			dc->SetBrush(colour_dark);
		}
	}


	//Draw the piece depending on the piece code
	switch (piece_draw){
	case EMPTY: {
		break;
	}
	case PAWN: {//Pawn
		Pawn(dc, x_center, y_center, unit_draw);
		break;
	}
	case KNIGHT: {//Knight
		Knight(dc, x_center, y_center, unit_draw);
		break;
	}
	case BISHOP: {//Bishop
		Bishop(dc, x_center, y_center, unit_draw);
		break;
	}
	case ROOK: {//Rook
		Rook(dc, x_center, y_center, unit_draw);
		break;
	}
	case QUEEN: {//Queen
		Queen(dc, x_center, y_center, unit_draw);
		break;
	}
	case KING: {//King
		King(dc, x_center, y_center, unit_draw);
		break;
	}
	}
	if ((sessionState != ONE_PLAYER && RealGame.turn == DARK && board_rotation == true) || (sessionState == ONE_PLAYER && Player1.Colour == LIGHT) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && Player1.Colour == DARK) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && Player2.Colour == DARK)){
		x = 9 - x; y = 9 - y;
	}
	delete dc;
}
void Square::Pawn(wxClientDC* dc, int x, int y, int unit){
	dc->DrawCircle(x, y, 1.5 * unit);
}
void Square::Knight(wxClientDC* dc, int x, int y, int unit){
	wxPointList* Points = new wxPointList;
	wxPoint* Point1 = new wxPoint(x - 1.875*unit + 0.2 * unit, y - 1.25*unit + 0.1 * unit);
	wxPoint* Point2 = new wxPoint(x - 1.875 * unit + 0.2 * unit, y + 0.1 * unit);
	wxPoint* Point3 = new wxPoint(x + 0.35 * unit + 0.2 * unit, y - 2.1 * unit + 0.1 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x - 1.675 * unit, y + 0.1 * unit);
	Point2 = new wxPoint(x + 0.55 * unit, y - 2.1 * unit + 0.1 * unit);
	Point3 = new wxPoint(x + 0.55 * unit, y - 1.25 * unit + 0.1 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x + 0.45 * unit + 0.1 * unit, y - 2.1 * unit + 0.1 * unit);
	Point2 = new wxPoint(x + 0.45 * unit + 0.1 * unit, y - 1.7 * unit + 0.1 * unit);
	Point3 = new wxPoint(x + 1.45 * unit + 0.1 * unit, y - 1.25 * unit + 0.1 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x + 0.45 * unit + 0.1 * unit, y - 2.1 * unit + 0.1 * unit);
	Point2 = new wxPoint(x - 0.45*unit, y + 1.7 * unit);
	Point3 = new wxPoint(x + 0.85 * unit, y + 1.7 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;
}
void Square::Bishop(wxClientDC* dc, int x, int y, int unit){
	wxPointList* Points = new wxPointList;
	wxPoint* Point1 = new wxPoint(x, y - 1.8 * unit);
	wxPoint* Point2 = new wxPoint(x, y - 0.6 * unit);
	wxPoint* Point3 = new wxPoint(x - 1.4 * unit, y + 0.3 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x, y - 1.8 * unit);
	Point2 = new wxPoint(x, y - 0.6 * unit);
	Point3 = new wxPoint(x + 1.4 * unit, y + 0.3 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	dc->DrawRectangle(x - 0.25 * unit, y - 1 * unit, 0.5 * unit, 2.7 * unit);
}
void Square::Rook(wxClientDC *dc, int x, int y, int unit){
	dc->DrawRectangle(x - 1 * unit, y - 0.75 * unit, 0.3 * unit, 2.75 * unit);
	dc->DrawRectangle(x + 0.7 * unit, y - 0.75 * unit, 0.3 * unit, 2.75 * unit);
	dc->DrawRectangle(x - 2 * unit, y - 1.8 * unit, 0.6 * unit, 3.8 * unit);
	dc->DrawRectangle(x - 0.3 * unit, y - 1.8 * unit, 0.6 * unit, 3.8 * unit);
	dc->DrawRectangle(x + 1.4 * unit, y - 1.8 * unit, 0.6 * unit, 3.8 * unit);
}
void Square::Queen(wxClientDC *dc, int x, int y, int unit){
	wxPointList* Points = new wxPointList;
	wxPoint* Point1 = new wxPoint(x - 1.5 * unit, y);
	wxPoint* Point2 = new wxPoint(x - 0.8 * unit, y);
	wxPoint* Point3 = new wxPoint(x - 1.5 * unit, y - 1.4 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x + 1.5 * unit, y);
	Point2 = new wxPoint(x + 0.8 * unit, y);
	Point3 = new wxPoint(x + 1.5 * unit, y - 1.4 * unit);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x, y - 1.4 * unit);
	Point2 = new wxPoint(x, y);
	Point3 = new wxPoint(x - 1.5 * unit, y);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	Points = new wxPointList;
	Point1 = new wxPoint(x, y - 1.4 * unit);
	Point2 = new wxPoint(x, y);
	Point3 = new wxPoint(x + 1.5 * unit, y);
	Points->Append(Point1);
	Points->Append(Point2);
	Points->Append(Point3);
	dc->DrawPolygon(Points);
	delete Point1, Point2, Point3, Points;

	dc->DrawRectangle(x - 1.5 * unit, y + 0.6 * unit, 3 * unit, 0.9 * unit);
}
void Square::King(wxClientDC* dc, int x, int y, int unit){
	dc->DrawRectangle(x - 0.4 * unit, y - 1.8 * unit, 0.8 * unit, 3.6 * unit);
	dc->DrawRectangle(x - 1.8 * unit, y - 0.4 * unit, 3.6 * unit, 0.8 * unit);
}

/*AI Functions*/
void AI::Backup(){
	// Backup existing state
	for (int i = 1; i <= 8; i++){
		for (int j = 0; j <= 8; j++){
			gameTemp[i][j] = RealGame.game[i][j];
		}
	}
}
void AI::Backup1(){
	// Backup existing state
	for (int i = 1; i <= 8; i++){
		for (int j = 0; j <= 8; j++){
			gameTemp1[i][j] = RealGame.game[i][j];
		}
	}
}
void AI::VirtualMove(int x1, int y1, int x2, int y2){
	RealGame.game[x2][y2] = RealGame.game[x1][y1];
	RealGame.game[x1][y1].piece = EMPTY;
}
void AI::Restore(){
	// Restore original state
	for (int i = 1; i <= 8; i++){
		for (int j = 0; j <= 8; j++){
			RealGame.game[i][j] = gameTemp[i][j];
		}
	}
}
void AI::Restore1(){
	// Restore original state
	for (int i = 1; i <= 8; i++){
		for (int j = 0; j <= 8; j++){
			RealGame.game[i][j] = gameTemp1[i][j];
		}
	}
}
void AI::MakeTheMove(int x1, int y1, int x2, int y2){
	wxMilliSleep(600);
	RealGame.Control(x1, y1, 1);
	wxMilliSleep(700);
	RealGame.Control(x2, y2, 1);

	if (promotionPane->promotion_state == true){
		RealGame.PawnPromotion(1);	// Queen
	}
}
move_coords AI::Checkmate(piece_colour colour, int mode){//mode 1 for value which will chkmate, mode 2 for stalemate 
	move_coords move;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.game[i][j].colour == colour){
				for (int m = 1; m <= 8; m++){
					for (int n = 1; n <= 8; n++){
						Backup();
						if (RealGame.Validity(i, j, m, n) == true){
							VirtualMove(i, j, m, n);
						}
						if (RealGame.Checkmate(colour, 1) == 1 && mode == 1){
							move.x1 = i;
							move.x2 = m;
							move.y1 = j;
							move.y2 = n;
							return move;
						}
						else if (RealGame.Checkmate(colour, 1) == 2 && mode == 2){
							move.x1 = i;
							move.x2 = m;
							move.y1 = j;
							move.y2 = n;
							return move;
						}
						Restore();
					}
				}
			}
		}
	}

	// Return struct with everything 0
	move.x1 = 0;
	move.y1 = 0;
	move.x2 = 0;
	move.y2 = 0;
	move.final = EMPTY;
	return move;

}
bool AI::CheckmatePossible(piece_colour colour){	//Checks if colour can checkmate in one move
	move_coords move = Checkmate(colour, 1);
	if (!move.x1 && !move.y1){
		return true;
	}
}
move_coords AI::PawnPromotion(piece_colour colour){
	move_coords move;
	for (int i = 1; i <= 8; i++){
		for (int j = 2; j <= 7; j += 5){
			if (RealGame.game[i][j].colour == colour && RealGame.game[i][j].piece != EMPTY){
				for (int m = 1; m <= 8; m++){
					for (int n = 1; n <= 8; n += 7){
						if (RealGame.Validity(i, j, m, n) == true){
							move.x1 = i;
							move.y1 = j;
							move.x2 = m;
							move.y2 = n;
							return move;
						}
					}
				}
			}
		}
	}
	move.x1 = 0;
	move.x2 = 0;
	move.y1 = 0;
	move.y2 = 0;
	return move;
}
numberof AI::Threats(int x, int y){
	//return no of threats and sum ofpowerpts attcking a certain square
	numberof chubby;
	chubby.power = 0; chubby.pawn = 0; chubby.powpts = 0;
	Backup1();
	RealGame.game[x][y].piece = EMPTY;

	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (((RealGame.Validity(i, j, x, y) && RealGame.game[i][j].colour != RealGame.turn) || (RealGame.game[i][j + 1].piece == KING&&RealGame.game[i][j + 1].colour != RealGame.turn || RealGame.game[i + 1][j + 1].piece == KING&&RealGame.game[i + 1][j + 1].colour != RealGame.turn || RealGame.game[i - 1][j + 1].piece == KING&&RealGame.game[i - 1][j + 1].colour != RealGame.turn || RealGame.game[i + 1][j - 1].piece == KING&&RealGame.game[i + 1][j - 1].colour != RealGame.turn || RealGame.game[i + 1][j].piece == KING&&RealGame.game[i + 1][j].colour != RealGame.turn || RealGame.game[i][j - 1].piece == KING&&RealGame.game[i][j - 1].colour != RealGame.turn || RealGame.game[i - 1][j - 1].piece == KING&&RealGame.game[i - 1][j - 1].colour != RealGame.turn || RealGame.game[i + 1][j - 1].piece == KING&&RealGame.game[i + 1][j - 1].colour != RealGame.turn))){
				if (RealGame.game[i][j].piece == PAWN){
					chubby.pawn++;
				}
				else if (RealGame.game[i][j].piece != PAWN){
					if (RealGame.game[i][j].piece == BISHOP || KNIGHT){
						chubby.powpts += 3;
					}
					else if (RealGame.game[i][j].piece == ROOK){
						chubby.powpts += 5;
					}
					else if (RealGame.game[i][j].piece == QUEEN){
						chubby.powpts += 9;
					}
					else if (RealGame.game[i][j].piece == KING){
						chubby.powpts += 1;
					}
					chubby.power++;
				}
			}
		}
	}
	Restore1();
	return chubby;
}
numberof AI::Protects(int x, int y){
	//return no of pieces protecting a square and sum of power pts
	numberof chubby;
	chubby.power = 0; chubby.pawn = 0; chubby.powpts = 0;
	Backup1();
	RealGame.game[x][y].piece = EMPTY;

	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (((RealGame.Validity(i, j, x, y) && RealGame.game[i][j].colour == RealGame.turn) || (RealGame.game[i][j + 1].piece == KING&&RealGame.game[i][j + 1].colour == RealGame.turn || RealGame.game[i + 1][j + 1].piece == KING&&RealGame.game[i + 1][j + 1].colour == RealGame.turn || RealGame.game[i - 1][j + 1].piece == KING&&RealGame.game[i - 1][j + 1].colour == RealGame.turn || RealGame.game[i + 1][j - 1].piece == KING&&RealGame.game[i + 1][j - 1].colour == RealGame.turn || RealGame.game[i + 1][j].piece == KING&&RealGame.game[i + 1][j].colour == RealGame.turn || RealGame.game[i][j - 1].piece == KING&&RealGame.game[i][j - 1].colour == RealGame.turn || RealGame.game[i - 1][j - 1].piece == KING&&RealGame.game[i - 1][j - 1].colour == RealGame.turn || RealGame.game[i + 1][j - 1].piece == KING&&RealGame.game[i + 1][j - 1].colour == RealGame.turn))){
				if (RealGame.game[i][j].piece == PAWN){
					chubby.pawn++;
				}
				else if (RealGame.game[i][j].piece != PAWN){
					if (RealGame.game[i][j].piece == BISHOP || KNIGHT){
						chubby.powpts += 3;
					}
					else if (RealGame.game[i][j].piece == ROOK){
						chubby.powpts += 5;
					}
					else if (RealGame.game[i][j].piece == QUEEN){
						chubby.powpts += 9;
					}
					else if (RealGame.game[i][j].piece == KING){
						chubby.powpts += 1;
					}
					chubby.power++;
				}
			}
		}
	}
	Restore1();
	return chubby;
}
move_coords AI::Fork(piece_colour colour, int mode){
	//mode 1= check fork - bishop(king,rook|king,knight), knight(king,queen|king,rook|king,bishop)
	//mode 2= queen fork - knight(q,r|q,b)
	//mode 3= power fork - knight(r,r|r,b|b,b), bishop(r,r|k,k|r,k)
	Backup(); move_coords attack1, attack2, temp;
	move_coords arr[15];
	for (int i = 1; i <= 8; i++){
		arr[i].final = EMPTY;
	}
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == colour){
				for (int m = i - 2; m <= i + 2; m++){
					for (int n = j - 2; n <= j + 2; n++){
						numberof des;
						if (RealGame.Validity(i, j, m, n) == 1){
							Backup();
							RealGame.game[m][n].piece = RealGame.game[i][j].piece;
							RealGame.game[m][n].colour = RealGame.game[i][j].colour;
							des = Threats(m, n);
							attack1 = (Attack(m, n, 1));
							attack2 = (Attack(m, n, 2));
							temp.x1 = i;
							temp.y1 = j;
							temp.x2 = m;
							temp.y2 = n;
							//find opp king and call his coords x_king,y_king
							int x_king = 1, y_king = 1;
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack1.final == QUEEN)){
								temp.final = QUEEN;
								arr[0] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack2.final == QUEEN)){
								temp.final = QUEEN;
								arr[1] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack1.final == ROOK)){
								temp.final = QUEEN;
								arr[2] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack2.final == ROOK)){
								temp.final = QUEEN;
								arr[3] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack1.final == BISHOP)){
								temp.final = QUEEN;
								arr[4] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack2.final == BISHOP)){
								temp.final = QUEEN;
								arr[5] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack1.final == KNIGHT)){
								temp.final = QUEEN;
								arr[6] = temp;
							}
							if (!(des.pawn + des.power + des.powpts) && RealGame.Check(x_king, y_king) && (attack2.final == KNIGHT)){
								temp.final = QUEEN;
								arr[7] = temp;
							}
							Restore();
						}
					}
				}
			}
		}
	}
	int i;
	for (i = 0; i <= 7; i++){
		if (arr[i].final != EMPTY) {
			return arr[i];
		}
	}
	return arr[i];
}
move_coords AI::Attack(int x, int y, int mode){
	//return attck coords;
	//mode 1 first occurance of piece
	//mode 2 last occcurance of piece
	//example if x,y has knight in mode 1 it return the coords of the first quuen it can attck 
	//in mode 2 it return the coords of the last queen it encounters in the iteration
	move_coords bestm;
	bestm.final = EMPTY;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.Validity(x, y, i, j) && RealGame.game[i][j].piece == QUEEN){
				bestm.x1 = x;
				bestm.y1 = y;
				bestm.x2 = i;
				bestm.y2 = j;
				bestm.final = QUEEN;
				if (mode == 1)return bestm;
			}
		}
	}
	if (mode == 2 && bestm.final != EMPTY)return bestm;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.Validity(x, y, i, j) && RealGame.game[i][j].piece == ROOK){
				bestm.x1 = x;
				bestm.y1 = y;
				bestm.x2 = i;
				bestm.y2 = j;
				bestm.final = ROOK;
				if (mode == 1)return bestm;
			}
		}
	}
	if (mode == 2 && bestm.final != EMPTY)return bestm;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.Validity(x, y, i, j) && RealGame.game[i][j].piece == BISHOP){
				bestm.x1 = x;
				bestm.y1 = y;
				bestm.x2 = i;
				bestm.y2 = j;
				bestm.final = BISHOP;
				if (mode == 1)return bestm;
			}
		}
	}
	if (mode == 2 && bestm.final != EMPTY)return bestm;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.Validity(x, y, i, j) && RealGame.game[i][j].piece == KNIGHT){
				bestm.x1 = x;
				bestm.y1 = y;
				bestm.x2 = i;
				bestm.y2 = j;
				bestm.final = KNIGHT;
				if (mode == 1)return bestm;
			}
		}
	}
	if (mode == 2 && bestm.final != EMPTY)return bestm;

	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.Validity(x, y, i, j) && RealGame.game[i][j].piece == PAWN){
				bestm.x1 = x;
				bestm.y1 = y;
				bestm.x2 = i;
				bestm.y2 = j;
				bestm.final = PAWN;
				if (mode == 1)return bestm;
			}
		}
	}
	if (mode == 2 && bestm.final != EMPTY)return bestm;

	bestm.x1 = 0;
	bestm.x2 = 0;
	bestm.y1 = 0;
	bestm.y2 = 0;
	bestm.final = EMPTY;
	return bestm;
}
bool AI::RandomMovei(piece_code piece){
	int sesh = 0;
	do{
		FindMoves();
		srand(time(NULL));
		int	Move_no = rand() % No_of_moves;

		for (Move_no; Move_no > 0; Move_no--){
			temp = TOP;
			TOP = TOP->NEXT;
			delete temp;
		}

		if ((RealGame.game[TOP->x1][TOP->y1].piece == piece)){
			sesh = 31;
			MakeTheMove(TOP->x1, TOP->y1, TOP->x2, TOP->y2);

			ClearMoves();
			return true;
		}
		sesh++;
	} while (sesh < 30);

	return false;
}
bool AI::RandomMove(piece_code piece){	// 0 for any piece
	int sesh = 0;
	do{
		FindMoves();
		srand(time(NULL));
		int	Move_no = rand() % No_of_moves;

		for (Move_no; Move_no > 0; Move_no--){
			temp = TOP;
			TOP = TOP->NEXT;
			delete temp;
		}
		if (piece == EMPTY){
			MakeTheMove(TOP->x1, TOP->y1, TOP->x2, TOP->y2);

			ClearMoves();
			return true;
		}

		Backup();
		RealGame.game[TOP->x2][TOP->y2].piece = RealGame.game[TOP->x1][TOP->y1].piece;
		RealGame.game[TOP->x2][TOP->y2].colour = RealGame.game[TOP->x1][TOP->y1].colour;
		numberof chubby = Threats(TOP->x2, TOP->y2);
		Restore();
		if ((chubby.pawn == 0 && chubby.powpts == 0 && piece == KING) || (RealGame.game[TOP->x1][TOP->y1].piece == piece)){
			sesh = 31;
			MakeTheMove(TOP->x1, TOP->y1, TOP->x2, TOP->y2);

			ClearMoves();
			return true;
		}
		sesh++;
	} while (sesh < 30);

	return false;
}
bool AI::Rank(piece_code p1, piece_code p2){
	//checks whether p1>=p2 in pts
	if (p1 == QUEEN){
		if (p2 == QUEEN)return TRUE;
		else return FALSE;
	}
	if (p1 == ROOK){
		if (p2 == ROOK || p2 == QUEEN)return TRUE;
		else return FALSE;
	}
	if (p1 == BISHOP || p1 == KNIGHT){
		if (p2 == PAWN)return FALSE;
		else return TRUE;
	}
	if (p1 == PAWN)return TRUE;
}

AI_1::AI_1() {
	TOP = NULL;
	No_of_moves = 0;
}
void AI::FindMoves() {
	No_of_moves = 0;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.game[i][j].colour == Player1.Colour){
				for (int m = 1; m <= 8; m++){
					for (int n = 1; n <= 8; n++){
						if (RealGame.Validity(i, j, m, n) == true){
							temp = new AvailableMoves;
							temp->x1 = i; temp->y1 = j;
							temp->x2 = m; temp->y2 = n;
							temp->NEXT = TOP;
							TOP = temp;
							No_of_moves++;
						}
					}
				}
			}
		}
	}
}
void AI::ClearMoves(){
	while (TOP != NULL){
		temp = TOP;
		TOP = TOP->NEXT;
		delete temp;
	}
}
void AI_1::moveAI(){
	FindMoves();
	srand(time(NULL));
	int Move_no = rand() % No_of_moves;

	for (Move_no; Move_no > 0; Move_no--){
		temp = TOP;
		TOP = TOP->NEXT;
		delete temp;
	}

	wxMilliSleep(600);
	RealGame.Control(TOP->x1, TOP->y1, 1);
	wxMilliSleep(700);
	RealGame.Control(TOP->x2, TOP->y2, 1);

	ClearMoves();
	if (promotionPane->promotion_state == true){
		int select = rand() % 4 + 1;
		RealGame.PawnPromotion(select);
	}
}

AI_2::AI_2(){
	TOP = NULL;
	No_of_moves = 0;
}
void AI_2::AI_QUEEN(){	//QUEEN
		{
			// Any lower piece kills queen
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if ((RealGame.game[i][j].piece == PAWN || RealGame.game[i][j].piece == KNIGHT || RealGame.game[i][j].piece == BISHOP || RealGame.game[i][j].piece == ROOK || RealGame.game[i][j].piece == KING) && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == QUEEN){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}

			// Queen kills queen
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == QUEEN){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}


			//QUEEN threathned part
	{
		//if threatning piece is rook which is not protected and it can be killed 

		// Queen threatened, kill rook to escape
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						numberof grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == ROOK && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
						bestMove = Attack(i, j, 2);
						grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == ROOK  && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}

		//if threatning piece is rook and it can be killed by piece of rank

		//if threatning piece is bishop which is not protected and it can be killed 

		// Queen threatened, kill bishop to escape
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						numberof grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == BISHOP && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
						bestMove = Attack(i, j, 2);
						grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == BISHOP  && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}

		//if threatning piece is bishop and it can be killed by piece of rank

		//if threatning piece is knight which is not protected and it can be killed 

		// Queen threatened, kill knight to escape
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						numberof grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == KNIGHT && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
						bestMove = Attack(i, j, 2);
						grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == KNIGHT  && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}

		//if threatning piece is knight and it can be killed by piece of rank

		//if threatning piece is pawn which is not protected and it can be killed 

		// Queen threatened, kill pawn to escape
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}

		//kill threatning piece with lower piece or equal piece
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].colour == RealGame.turn&&RealGame.game[i][j].piece == QUEEN){
					for (int m = 1; m <= 8; m++){
						for (int n = 1; n <= 8; n++){
							if (RealGame.Validity(i, j, m, n) && Rank(RealGame.game[i][j].piece, RealGame.game[m][n].piece)){
								Backup();
								int flug = 0;
								//MAke Virtual Move
								RealGame.game[m][n].piece = RealGame.game[i][j].piece;
								RealGame.game[m][n].colour = RealGame.game[i][j].colour;
								RealGame.game[i][j].piece = EMPTY;
								for (int k = 1; k <= 8; k++){
									for (int j = 0; j <= 8; j++){
										if (RealGame.game[i][j].piece == QUEEN&&RealGame.game[i][j].colour == RealGame.turn){
											numberof gum = Threats(i, j);
											if (gum.power + gum.pawn == 0){
												flug = 1; break;
											}
										}
									}
									if (flug)break;
								}
								Restore();
								if (flug){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}

		// Queen threatened, move to a place where threats is zero
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								if (RealGame.Validity(i, j, m, n)){
									threat = Threats(m, n);
									if (!(threat.power + threat.pawn)){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}
		}
		//kill Queen threathning piece with another piece to stop threathning
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].colour == RealGame.turn){
					for (int m = 1; m <= 8; m++){
						for (int n = 1; n <= 8; n++){
							if (RealGame.Validity(i, j, m, n)){
								Backup();
								int flug = 0;
								//make virtual move
								RealGame.game[m][n].piece = RealGame.game[i][j].piece;
								RealGame.game[m][n].colour = RealGame.game[i][j].colour;
								RealGame.game[i][j].piece = EMPTY;
								for (int k = 1; k <= 8; k++){
									for (int j = 0; j <= 8; j++){
										if (RealGame.game[i][j].piece == QUEEN&&RealGame.game[i][j].colour == RealGame.turn){
											numberof gum = Threats(i, j);
											if (gum.power + gum.pawn == 0){
												flug = 1; break;
											}
										}
									}
									if (flug)break;
								}
								Restore();
								if (flug){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}

		// If queen is threatened and cannot escape, then kill rook
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == ROOK){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}
		// If queen is threatened and cannot escape, then kill bishop
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == BISHOP){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}
		// If queen is threatened and cannot escape, then kill knight
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == KNIGHT){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}
		// If queen is threatened and cannot escape, then kill pawn
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}
	}
		}
}
void AI_2::AI_CHECKMATE(){
	//checkmate
		{
			move_coords chk;

			chk = Checkmate(RealGame.turn, 1);
			if (chk.x1){// Checkmate
				MakeTheMove(chk.x1, chk.y1, chk.x2, chk.y2);
				return;
			}
			chk = Checkmate(RealGame.turn, 2);
			if (chk.x1){// Stalemates
				MakeTheMove(chk.x1, chk.y1, chk.x2, chk.y2);
				return;
			}
			chk = PawnPromotion(RealGame.turn);
			if (chk.x1){
				MakeTheMove(chk.x1, chk.y1, chk.x2, chk.y2);
				return;
			}
		}
}
void AI_2::AI_ROOK(){
	//ROOK	
		{
			// Lower piece kills rook
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if ((RealGame.game[i][j].piece == PAWN || RealGame.game[i][j].piece == KNIGHT || RealGame.game[i][j].piece == BISHOP || RealGame.game[i][j].piece == KING) && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == ROOK){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
			// rook kills rook
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == ROOK){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
			// Queen kills rook which is not protected
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if ((RealGame.game[i][j].piece == QUEEN) && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == ROOK){
							numberof grey = Threats(bestMove.x1, bestMove.y1);
							if (!(grey.pawn + grey.powpts)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}

			//ROOK threatned part
		{
			// ROOK threatened, kill bishop to escape
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							move_coords bestMove = Attack(i, j, 1);
							numberof grey = Threats(bestMove.x2, bestMove.y2);
							if (bestMove.final == BISHOP && !(grey.power + grey.pawn)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
							bestMove = Attack(i, j, 2);
							grey = Threats(bestMove.x2, bestMove.y2);
							if (bestMove.final == BISHOP  && !(grey.power + grey.pawn)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}

			// ROOK threatened, kill knight to escape
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							move_coords bestMove = Attack(i, j, 1);
							numberof grey = Threats(bestMove.x2, bestMove.y2);
							if (bestMove.final == KNIGHT && !(grey.power + grey.pawn)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
							bestMove = Attack(i, j, 2);
							grey = Threats(bestMove.x2, bestMove.y2);
							if (bestMove.final == KNIGHT  && !(grey.power + grey.pawn)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}

			// ROOK threatened, kill pawn to escape
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							for (int m = 1; m <= 8; m++){
								for (int n = 1; n <= 8; n++){
									numberof threat2 = Threats(m, n);
									if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}

			//kill threatning piece with lower piece or equal piece
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].colour == RealGame.turn&&RealGame.game[i][j].piece == ROOK){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								if (RealGame.Validity(i, j, m, n) && Rank(RealGame.game[i][j].piece, RealGame.game[m][n].piece)){
									Backup();
									int flug = 0;
									//MAke Virtual Move
									RealGame.game[m][n].piece = RealGame.game[i][j].piece;
									RealGame.game[m][n].colour = RealGame.game[i][j].colour;
									RealGame.game[i][j].piece = EMPTY;
									for (int k = 1; k <= 8; k++){
										for (int j = 0; j <= 8; j++){
											if (RealGame.game[i][j].piece == ROOK&&RealGame.game[i][j].colour == RealGame.turn){
												numberof gum = Threats(i, j);
												if (gum.power + gum.pawn == 0){
													flug = 1; break;
												}
											}
										}
										if (flug)break;
									}
									Restore();
									if (flug){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}

			// ROOK threatened, move to a place where threats is zero
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							for (int m = 1; m <= 8; m++){
								for (int n = 1; n <= 8; n++){
									if (RealGame.Validity(i, j, m, n)){
										threat = Threats(m, n);
										if (!(threat.power + threat.pawn)){
											MakeTheMove(i, j, m, n);
											return;
										}
									}
								}
							}
						}
					}
				}
			}
			//kill ROOK threathning piece with another piece to stop threathning
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].colour == RealGame.turn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								if (RealGame.Validity(i, j, m, n)){
									Backup();
									int flug = 0;
									//make virtual move
									RealGame.game[m][n].piece = RealGame.game[i][j].piece;
									RealGame.game[m][n].colour = RealGame.game[i][j].colour;
									RealGame.game[i][j].piece = EMPTY;
									for (int k = 1; k <= 8; k++){
										for (int j = 0; j <= 8; j++){
											if (RealGame.game[i][j].piece == ROOK&&RealGame.game[i][j].colour == RealGame.turn){
												numberof gum = Threats(i, j);
												if (gum.power + gum.pawn == 0){
													flug = 1; break;
												}
											}
										}
										if (flug)break;
									}
									Restore();
									if (flug){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}

			// If rook is threatened and cannot escape, then kill bishop
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							move_coords bestMove = Attack(i, j, 1);
							if (bestMove.final == BISHOP){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}
			// If rook is threatened and cannot escape, then kill knight
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							move_coords bestMove = Attack(i, j, 1);
							if (bestMove.final == KNIGHT){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}
			// If ROOK is threatened and cannot escape, then kill pawn
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == ROOK && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							for (int m = 1; m <= 8; m++){
								for (int n = 1; n <= 8; n++){
									numberof threat2 = Threats(m, n);
									if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}
		}
		}
}
void AI_2::AI_BISHOP(){	//BISHOP
		{
			// Lower piece kills bishop
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if ((RealGame.game[i][j].piece == PAWN || RealGame.game[i][j].piece == KNIGHT || RealGame.game[i][j].piece == KING) && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == BISHOP){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
			// bishop kills bishop
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == BISHOP){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
			// Queen or rook kills bishop which is not protected
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if ((RealGame.game[i][j].piece == QUEEN || RealGame.game[i][j].piece == ROOK) && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == BISHOP){
							numberof grey = Threats(bestMove.x1, bestMove.y1);
							if (!(grey.pawn + grey.powpts)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}

			//BISHOP threathned part
		{
			// bishop threatened, kill knight to escape
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							move_coords bestMove = Attack(i, j, 1);
							numberof grey = Threats(bestMove.x2, bestMove.y2);
							if (bestMove.final == KNIGHT && !(grey.power + grey.pawn)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
							bestMove = Attack(i, j, 2);
							grey = Threats(bestMove.x2, bestMove.y2);
							if (bestMove.final == KNIGHT  && !(grey.power + grey.pawn)){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}

			// BISHOP threatened, kill pawn to escape
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							for (int m = 1; m <= 8; m++){
								for (int n = 1; n <= 8; n++){
									numberof threat2 = Threats(m, n);
									if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}

			//kill threatning piece with lower piece or equal piece
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].colour == RealGame.turn&&RealGame.game[i][j].piece == BISHOP){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								if (RealGame.Validity(i, j, m, n) && Rank(RealGame.game[i][j].piece, RealGame.game[m][n].piece)){
									Backup();
									int flug = 0;
									//MAke Virtual Move
									RealGame.game[m][n].piece = RealGame.game[i][j].piece;
									RealGame.game[m][n].colour = RealGame.game[i][j].colour;
									RealGame.game[i][j].piece = EMPTY;
									for (int k = 1; k <= 8; k++){
										for (int j = 0; j <= 8; j++){
											if (RealGame.game[i][j].piece == BISHOP&&RealGame.game[i][j].colour == RealGame.turn){
												numberof gum = Threats(i, j);
												if (gum.power + gum.pawn == 0){
													flug = 1; break;
												}
											}
										}
										if (flug)break;
									}
									Restore();
									if (flug){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}

			// BISHOP threatened, move to a place where threats is zero
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							for (int m = 1; m <= 8; m++){
								for (int n = 1; n <= 8; n++){
									if (RealGame.Validity(i, j, m, n)){
										threat = Threats(m, n);
										if (!(threat.power + threat.pawn)){
											MakeTheMove(i, j, m, n);

											return;
										}
									}
								}
							}
						}
					}
				}
			}
			//kill BISHOP threathning piece with another piece to stop threathning
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].colour == RealGame.turn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								if (RealGame.Validity(i, j, m, n)){
									Backup();
									int flug = 0;
									//make virtual move
									RealGame.game[m][n].piece = RealGame.game[i][j].piece;
									RealGame.game[m][n].colour = RealGame.game[i][j].colour;
									RealGame.game[i][j].piece = EMPTY;
									for (int k = 1; k <= 8; k++){
										for (int j = 0; j <= 8; j++){
											if (RealGame.game[i][j].piece == BISHOP&&RealGame.game[i][j].colour == RealGame.turn){
												numberof gum = Threats(i, j);
												if (gum.power + gum.pawn == 0){
													flug = 1; break;
												}
											}
										}
										if (flug)break;
									}
									Restore();
									if (flug){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}
			// If BISHOP is threatened and cannot escape, then kill knight
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							move_coords bestMove = Attack(i, j, 1);
							if (bestMove.final == KNIGHT){
								MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
								return;
							}
						}
					}
				}
			}
			// If BISHOP is threatened and cannot escape, then kill pawn
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
						numberof threat = Threats(i, j);
						if (threat.power + threat.pawn){
							for (int m = 1; m <= 8; m++){
								for (int n = 1; n <= 8; n++){
									numberof threat2 = Threats(m, n);
									if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}

		}
		}
}
void AI_2::AI_KNIGHT(){// Lower piece kills knight
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if ((RealGame.game[i][j].piece == PAWN || RealGame.game[i][j].piece == KING || RealGame.game[i][j].piece == BISHOP) && RealGame.game[i][j].colour == RealGame.turn){
				move_coords bestMove = Attack(i, j, 1);
				if (bestMove.final == KNIGHT){
					MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
					return;
				}
			}
		}
	}
	// knight kills knight
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
				move_coords bestMove = Attack(i, j, 1);
				if (bestMove.final == KNIGHT){
					MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
					return;
				}
			}
		}
	}
	// Queen or bishop or rook kills knight which is not protected
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if ((RealGame.game[i][j].piece == QUEEN || RealGame.game[i][j].piece == BISHOP || RealGame.game[i][j].piece == ROOK) && RealGame.game[i][j].colour == RealGame.turn){
				move_coords bestMove = Attack(i, j, 1);
				if (bestMove.final == KNIGHT){
					numberof grey = Threats(bestMove.x1, bestMove.y1);
					if (!(grey.pawn + grey.powpts)){
						MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
						return;
					}
				}
			}
		}
	}

	//KNIGHT threatned part
	{
		// knight threatened, kill bishop to escape
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						numberof grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == BISHOP && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
						bestMove = Attack(i, j, 2);
						grey = Threats(bestMove.x2, bestMove.y2);
						if (bestMove.final == BISHOP  && !(grey.power + grey.pawn)){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}

		// knight threatened, kill pawn to escape
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}

		//kill threatning piece with lower piece or equal piece
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].colour == RealGame.turn&&RealGame.game[i][j].piece == KNIGHT){
					for (int m = 1; m <= 8; m++){
						for (int n = 1; n <= 8; n++){
							if (RealGame.Validity(i, j, m, n) && Rank(RealGame.game[i][j].piece, RealGame.game[m][n].piece)){
								Backup();
								int flug = 0;
								//MAke Virtual Move
								RealGame.game[m][n].piece = RealGame.game[i][j].piece;
								RealGame.game[m][n].colour = RealGame.game[i][j].colour;
								RealGame.game[i][j].piece = EMPTY;
								for (int k = 1; k <= 8; k++){
									for (int j = 0; j <= 8; j++){
										if (RealGame.game[i][j].piece == KNIGHT &&RealGame.game[i][j].colour == RealGame.turn){
											numberof gum = Threats(i, j);
											if (gum.power + gum.pawn == 0){
												flug = 1; break;
											}
										}
									}
									if (flug)break;
								}
								Restore();
								if (flug){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}

		// KNIGHT threatened, move to a place where threats is zero
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								if (RealGame.Validity(i, j, m, n)){
									threat = Threats(m, n);
									if (!(threat.power + threat.pawn)){
										MakeTheMove(i, j, m, n);
										return;
									}
								}
							}
						}
					}
				}
			}
		}
		//kill KNIGHT threathning piece with another piece to stop threathning
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].colour == RealGame.turn){
					for (int m = 1; m <= 8; m++){
						for (int n = 1; n <= 8; n++){
							if (RealGame.Validity(i, j, m, n)){
								Backup();
								int flug = 0;
								//make virtual move
								RealGame.game[m][n].piece = RealGame.game[i][j].piece;
								RealGame.game[m][n].colour = RealGame.game[i][j].colour;
								RealGame.game[i][j].piece = EMPTY;
								for (int k = 1; k <= 8; k++){
									for (int j = 0; j <= 8; j++){
										if (RealGame.game[i][j].piece == KNIGHT &&RealGame.game[i][j].colour == RealGame.turn){
											numberof gum = Threats(i, j);
											if (gum.power + gum.pawn == 0){
												flug = 1; break;
											}
										}
									}
									if (flug)break;
								}
								Restore();
								if (flug){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}
		// If KNIGHT is threatened and cannot escape, then kill bishop
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == BISHOP){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}
		// If KNIGHT is threatened and cannot escape, then kill pawn
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
					numberof threat = Threats(i, j);
					if (threat.power + threat.pawn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}
		}

	}
}
void AI_2::AI_RANDOM(){//RANDOM part
	// Do a random move
	//if (RandomMove(KING)){}
	if (RandomMove(PAWN)){
		return;
	}
	else if (RandomMove(KNIGHT)){
		return;
	}
	else if (RandomMove(BISHOP)){
		return;
	}
	else if (RandomMove(ROOK)){
		return;
	}
	else if (RandomMovei(PAWN)){
		return;
	}
	else if (RandomMovei(KNIGHT)){
		return;
	}
	else if (RandomMovei(BISHOP)){
		return;
	}
	else if (RandomMovei(ROOK)){
		return;
	}
	else if (RandomMove(EMPTY)){
		return;
	}
}
void AI_2::AI_PAWN(){	//killing pawn part
		{
			// pawn kills pawn
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == PAWN && RealGame.game[i][j].colour == RealGame.turn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && (threat2.power + threat2.pawn == 0) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}

			// other pieces kill pawn which is not protected
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if ((RealGame.game[i][j].piece == QUEEN || RealGame.game[i][j].piece == BISHOP || RealGame.game[i][j].piece == ROOK || RealGame.game[i][j].piece == KNIGHT) && RealGame.game[i][j].colour == RealGame.turn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && !(threat2.power + threat2.pawn) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}

			// pawn kills pawn which is protected
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == PAWN && RealGame.game[i][j].colour == RealGame.turn){
						for (int m = 1; m <= 8; m++){
							for (int n = 1; n <= 8; n++){
								numberof threat2 = Threats(m, n);
								if (RealGame.game[m][n].piece == PAWN && !(threat2.power + threat2.pawn) && RealGame.Validity(i, j, m, n)){
									MakeTheMove(i, j, m, n);
									return;
								}
							}
						}
					}
				}
			}

			// king kills pawn
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (RealGame.game[i][j].piece == KING && RealGame.game[i][j].colour == RealGame.turn){
						move_coords bestMove = Attack(i, j, 1);
						if (bestMove.final == PAWN){
							MakeTheMove(bestMove.x1, bestMove.y1, bestMove.x2, bestMove.y2);
							return;
						}
					}
				}
			}
		}
}
void AI_2::moveAI(){
	if (moves < 1){
		Move_1();
		return;
	}
	AI_CHECKMATE();
	if (Player1.Colour != RealGame.turn) return;
	AI_QUEEN();
	if (Player1.Colour != RealGame.turn) return;
	AI_ROOK();
	if (Player1.Colour != RealGame.turn) return;
	AI_BISHOP();
	if (Player1.Colour != RealGame.turn) return;
	AI_KNIGHT();
	if (Player1.Colour != RealGame.turn) return;
	AI_PAWN();
	if (Player1.Colour != RealGame.turn) return;
	AI_RANDOM();
}
void AI_2::Move_1(){
	srand(time(NULL));
	if (RealGame.turn == LIGHT){
		int choice = (rand() % 3) + 1;
		switch (choice){
		case 1: {
			MakeTheMove(5, 7, 5, 5);
			return;
		}
		case 2: {
			MakeTheMove(4, 7, 4, 5);
			return;
		}
		case 3: {
			MakeTheMove(7, 8, 6, 6);
			return;
		}
		}
	}
	if ((RealGame.turn == DARK && RealGame.game[4][5].piece == PAWN)){
		int choice = (rand() % 3) + 1;
		switch (choice){
		case 1: {
			MakeTheMove(6, 2, 6, 4);
			return;
		}
		case 2: {
			MakeTheMove(4, 2, 4, 4);
			return;
		}
		case 3: {
			MakeTheMove(4, 2, 4, 3);
			return;
		}
		}
	}
	else if (RealGame.turn == DARK && RealGame.game[5][5].piece == PAWN){
		int choice = (rand() % 2) + 1;
		switch (choice){
		case 1: {
			MakeTheMove(2, 1, 3, 3);
			return;
		}
		case 2: {
			MakeTheMove(5, 2, 5, 4);
			return;
		}
		}
	}
	else if (RealGame.turn == DARK && RealGame.game[3][5].piece == EMPTY){
		MakeTheMove(4, 2, 4, 4);
		return;
	}
	else {
		MakeTheMove(5, 2, 5, 4);
		return;
	}
}

void AI_3::Move_1(){
	if (moves == 0){
		RealGame.Control(5, 2, 1);
		wxMilliSleep(500);
		RealGame.Control(5, 4, 1);
	}
	else if (moves == 0.5 && RealGame.game[6][3].piece == KNIGHT){
		RealGame.Control(4, 7, 1);
		wxMilliSleep(500);
		RealGame.Control(4, 5, 1);
	}

	else if (moves == 0.5 && (RealGame.game[4][4].piece == PAWN || RealGame.game[6][4].piece == PAWN)){
		RealGame.Control(7, 8, 1);
		wxMilliSleep(500);
		RealGame.Control(6, 6, 1);
	}
	else if (moves == 0.5){
		RealGame.Control(5, 7, 1);
		wxMilliSleep(500);
		RealGame.Control(5, 5, 1);
	}
}
void AI_3::Move_2(){
	if (moves == 1 && RealGame.game[5][4].piece == PAWN){
		RealGame.Control(4, 5, 1);
		wxMilliSleep(500);
		RealGame.Control(5, 4, 1);
	}
	else if (moves == 1 && RealGame.game[3][4].piece == PAWN){
		RealGame.Control(4, 5, 1);
		wxMilliSleep(500);
		RealGame.Control(3, 4, 1);
	}
	else if (moves == 1 && RealGame.game[3][3].piece == KNIGHT){
		RealGame.Control(4, 5, 1);
		wxMilliSleep(500);
		RealGame.Control(4, 4, 1);
	}
	else if (moves == 1 && RealGame.game[4][4].piece == PAWN){
		RealGame.Control(2, 8, 1);
		wxMilliSleep(500);
		RealGame.Control(3, 6, 1);
	}
	else {
		RealGame.Control(8, 7, 1);
		wxMilliSleep(500);
		RealGame.Control(6, 6, 1);
	}

}
void AI_3::moveAI(){
	move_coords moveToMake;
	// First looks for checkmate in one move:
	moveToMake = Checkmate(RealGame.turn, 1);
	if (moveToMake.x1 != 0) {
		// Make the move
	}
	// Stalemate
	int points = 0;	// To decide whether to do stalemate or not
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (RealGame.game[i][j].piece == PAWN && RealGame.game[i][j].colour == RealGame.turn){
				points++;
			}
			if (RealGame.game[i][j].piece == PAWN && RealGame.game[i][j].colour != RealGame.turn){
				points--;
			}
			if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour == RealGame.turn){
				points += 3;
			}
			if (RealGame.game[i][j].piece == KNIGHT && RealGame.game[i][j].colour != RealGame.turn){
				points -= 3;
			}
			if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour == RealGame.turn){
				points += 3;
			}
			if (RealGame.game[i][j].piece == BISHOP && RealGame.game[i][j].colour != RealGame.turn){
				points -= 3;
			}
			if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour == RealGame.turn){
				points += 8;
			}
			if (RealGame.game[i][j].piece == QUEEN && RealGame.game[i][j].colour != RealGame.turn){
				points -= 8;
			}
		}
	}
	moveToMake = Checkmate(RealGame.turn, 2);
	if (points < 10 && moveToMake.x1 != 0){
		//make move
	}



}

/*Game Functions*/
void Game::Move(int x1, int y1, int x2, int y2){
	// For internet Two Player
	// To register the move
	if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && RealGame.turn == Player1.Colour) {
		argListForMoveRegister.UserMaking = Player1.Name;
		argListForMoveRegister.UserReceiving = Player2.Name;
		argListForMoveRegister.x1 = highlighted_x;
		argListForMoveRegister.y1 = highlighted_y;
		argListForMoveRegister.x2 = x2;
		argListForMoveRegister.y2 = y2;
		_beginthread(Account::MoveRegister, 0, NULL);
	}
	if (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && RealGame.turn == Player2.Colour) {
		argListForMoveRegister.UserMaking = Player2.Name;
		argListForMoveRegister.UserReceiving = Player1.Name;
		argListForMoveRegister.x1 = highlighted_x;
		argListForMoveRegister.y1 = highlighted_y;
		argListForMoveRegister.x2 = x2;
		argListForMoveRegister.y2 = y2;
		_beginthread(Account::MoveRegister, 0, NULL);
	}
	/*Stale 50*///declare stale50 as global variables
	if (game[x1][y1].piece == PAWN || game[x2][y2].piece != EMPTY){	//takes care of castling and other piece moves
		if (game[x1][y1].colour == LIGHT){
			stale50_light = 0;
		}
		else
			stale50_dark = 0;
	}
	int x_king, y_king;
	if (game[x1][y1].piece != PAWN && game[x2][y2].piece == EMPTY && game[x1][y1].colour == LIGHT){
		stale50_light++;
	}
	if (game[x1][y1].piece != PAWN && game[x2][y2].piece == EMPTY && game[x1][y1].colour == DARK){
		stale50_dark++;
	}

	/*Castling*/
	if (x1 == 1 && y1 == 8 && game[x1][y1].piece == ROOK && game[x1][y1].colour == LIGHT){
		castle_white_queen = false;
	}
	else if (x1 == 8 && y1 == 8 && game[x1][y1].piece == ROOK && game[x1][y1].colour == LIGHT){
		castle_white_king = false;
	}
	else if (x1 == 1 && y1 == 1 && game[x1][y1].piece == ROOK && game[x1][y1].colour == LIGHT){
		castle_black_queen = false;
	}
	else if (x1 == 8 && y1 == 1 && game[x1][y1].piece == ROOK && game[x1][y1].colour == LIGHT){
		castle_black_king = false;
	}
	if (game[x1][y1].piece == KING && game[x1][y1].colour == LIGHT){
		castle_white_king = castle_white_queen = false;
	}
	else if (game[x1][y1].piece == KING && game[x1][y1].colour == LIGHT){
		castle_black_king = castle_black_queen = false;
	}

	/*En Passant*/
	EP_x = EP_y = 0;
	if (game[x1][y1].piece == PAWN && abs(y1 - y2) == 2){
		EP_x = x2;
		EP_y = y2;
	}

	/*Killed Pieces*/
	if (game[x2][y2].piece != EMPTY){
		if (game[x2][y2].colour == LIGHT){
			for (int i = 0; i <= 15; i++){
				if (killed_white[i] == EMPTY){
					killed_white[i] = game[x2][y2].piece;
					break;
				}
			}
		}
		else {
			for (int i = 0; i <= 15; i++){
				if (killed_black[i] == EMPTY){
					killed_black[i] = game[x2][y2].piece;
					break;
				}
			}
		}
	}

	/*Move*/
	game[x2][y2].piece = game[x1][y1].piece;
	game[x2][y2].colour = game[x1][y1].colour;
	game[x1][y1].piece = EMPTY;
	//game[x1][y1].Draw(MODE_DRAW_NORMAL);
	//game[x2][y2].Draw(MODE_DRAW_NORMAL);
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			RealGame.game[i][j].Draw(MODE_DRAW_NORMAL);
		}
	}

	///*Update the king position*/
	//if (game[x2][y2].piece == KING){
	//	if (turn == LIGHT){
	//		king_white_x = x2;
	//		king_white_y = y2;
	//	}
	//	else {
	//		king_black_x = x2;
	//		king_black_y = y2;
	//	}
	//}

	/*Pawn Promotion*/
	if (game[x2][y2].piece == PAWN && (y2 == 1 || y2 == 8)){
		promotionPane->promote_x = x2;
		promotionPane->promote_y = y2;
		promotionPane->promotion_state = true;
		PawnPromotion();	// Draws the pane
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].Draw(MODE_DRAW_HIGHLIGHT);
		if (RealGame.turn == LIGHT){
			RealGame.turn = DARK;
		}
		else {
			RealGame.turn = LIGHT;
		}
		return;	//TODO: Check for endgame, check etc. in pawn promotion function
	}

	// For internet Two Player
	// To register the move
	if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && RealGame.turn == Player1.Colour) {
		_beginthread(Account::MoveRegister, 0, NULL);
	}
	if (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && RealGame.turn == Player2.Colour) {
		_beginthread(Account::MoveRegister, 0, NULL);
	}

	/*Rotation and turn change - Skipped during castling*/
	if (castle == false){
		if (RealGame.turn == LIGHT){
			RealGame.turn = DARK;
		}
		else {
			RealGame.turn = LIGHT;
		}

		// King under check
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (game[i][j].piece == KING && game[i][j].colour == turn){
					xking = i;
					yking = j;
					break;
				}
			}
		}
		isCheck = Check(xking, yking);	// Check of king

		moves += 0.5;
		wxMilliSleep(400);
		// For turn change board lines
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				game[i][j].Draw(MODE_DRAW_NORMAL);
			}
		}

		if (isCheck == true){
			for (int i = 1; i <= 8; i++){
				for (int j = 1; j <= 8; j++){
					if (i == xking && j == yking){
						game[i][j].Draw(MODE_DRAW_CHECK);
						break;
					}
				}
			}
		}

		SaveObj.Save();
		RealGame.UpdateLogWindow();
	}

	/*killed*/
	killedPane->Killed();

	/*Checkmate Check*/
	if (Checkmate(RealGame.turn, 1) == 1){
		GameEnded = true;
		if (Player1.Colour == turn){	//The other player won because the turn just changed.
			EndGame(MODE_ENDGAME_CHECKMATE_2);
		}
		else {
			EndGame(MODE_ENDGAME_CHECKMATE_1);
		}
		return;
	}
	/*Stalemate and Draw Check*/
	else if (Checkmate(RealGame.turn, 1) == MODE_ENDGAME_STALEMATE){
		GameEnded = true;
		EndGame(MODE_ENDGAME_STALEMATE);
		return;
	}
	else if (Checkmate(RealGame.turn, 1) == MODE_ENDGAME_DRAW){
		GameEnded = true;
		EndGame(MODE_ENDGAME_DRAW);
		return;
	}
	else if ((stale50_light == 25 && stale50_light + stale50_dark >= 50) || (stale50_dark == 25 && stale50_light + stale50_dark >= 50)){
		GameEnded = true;
		EndGame(MODE_ENDGAME_DRAW);
		return;
	}

	/*3fold draw*/

	int Threefold = 0;
	Square TempArray[9][9];
	ifstream file("ThreefoldDraw.dat", ios::in | ios::binary);
	while (!file.eof()){
		file.read((char*)&TempArray, sizeof(TempArray));
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if ((RealGame.game[i][j].piece != TempArray[i][j].piece) || ((RealGame.game[i][j].colour != TempArray[i][j].colour) && (RealGame.game[i][j].piece != EMPTY && TempArray[i][j].piece != EMPTY))){
					break;
				}
			}
			if (i == 8){
				Threefold++;
			}
		}
	}
	file.close();

	if (Threefold >= 3){
		//Show the button for claiming a draw
	}

	if (sessionState == ONE_PLAYER && Player1.Colour == RealGame.turn && castle == false && promotionPane->promotion_state == false){
		CPWACN->moveAI();	// Make the AI's move
	}
}
int Game::Validity(int x1, int y1, int x2, int y2){
	// Boundary Check
	if (x1 < 1 || x1 > 8 || y1 < 1 || y1 >8 || x2 < 1 || x2 > 8 || y2 < 1 || y2 > 8){
		return 0;
	}
	/*Can't kill your own piece*/
	if ((game[x1][y1].colour == game[x2][y2].colour) && game[x2][y2].piece != EMPTY){
		return 0;
	}

	/*To check if king is under check after move is made*/
	piece_colour temp_colour; piece_code temp_piece;
	temp_colour = game[x2][y2].colour;
	temp_piece = game[x2][y2].piece;

	game[x2][y2].colour = game[x1][y1].colour;
	game[x2][y2].piece = game[x1][y1].piece;
	game[x1][y1].piece = EMPTY;

	int x_king, y_king;
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (game[i][j].piece == KING && game[i][j].colour == game[x1][y1].colour){
				x_king = i;
				y_king = j;
				break;
			}
		}
	}

	if (Check(x_king, y_king) == true){
		game[x1][y1].colour = game[x2][y2].colour;
		game[x1][y1].piece = game[x2][y2].piece;
		game[x2][y2].colour = temp_colour;
		game[x2][y2].piece = temp_piece;
		return 0;
	}

	game[x1][y1].colour = game[x2][y2].colour;
	game[x1][y1].piece = game[x2][y2].piece;
	game[x2][y2].colour = temp_colour;
	game[x2][y2].piece = temp_piece;

	/*Check if the move is valid for the piece*/
	switch (game[x1][y1].piece){
	case PAWN: {
		/*Backward move not allowed*/
		if (game[x1][y1].colour == LIGHT){
			if (y2 > y1){
				return 0;
			}
		}
		if (game[x1][y1].colour == DARK){
			if (y1 > y2){
				return 0;
			}
		}
		/*Move forward 1 step*/
		if (x1 == x2 && abs(y2 - y1) == 1){
			if (game[x2][y2].piece == EMPTY){
				return 1;
			}
			else {
				return 0;
			}
		}
		/*Move forward 2 steps*/
		else if ((y1 == 2 || y1 == 7) && (x1 == x2) && abs(y2 - y1) == 2){
			if (y1 == 7){
				if (game[x1][6].piece == EMPTY && game[x1][5].piece == EMPTY){
					return 1;
				}
				else return 0;
			}
			else if (y1 == 2){
				if (game[x1][3].piece == EMPTY && game[x1][4].piece == EMPTY){
					return 1;
				}
				else return 0;
			}
		}
		/*Kill*/
		else if ((abs(x2 - x1) == 1 && abs(y2 - y1) == 1) && game[x2][y2].piece != EMPTY){
			return 1;
		}
		/*En Passant*/
		else if ((abs(x2 - x1) == 1 && abs(y2 - y1) == 1) && game[x2][y2].piece == EMPTY && x2 == EP_x && y1 == EP_y){
			return 1;
		}
		else {
			return 0;
		}
		break;
	}
	case KNIGHT: {
		/*Diagonal distance checking*/
		if (pow((x2 - x1), 2) + pow((y2 - y1), 2) == 5){
			return 1;
		}
		break;
	}
	case BISHOP: {
		if ((x1 + y1 == x2 + y2) || (x1 - y1 == x2 - y2)){
			if (x1 > x2 && y1 > y2){
				for (int i = x1 - 1, j = y1 - 1; i > x2 && j > y2; i--, j--){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
				return 1;
			}
			else if (x1 < x2 && y1 < y2){
				for (int i = x1 + 1, j = y1 + 1; i < x2 && j < y2; i++, j++){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
				return 1;
			}
			else if (x1 > x2 && y1 < y2){
				for (int i = x1 - 1, j = y1 + 1; i > x2 && j < y2; i--, j++){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
				return 1;
			}
			else if (x1 < x2 && y1 > y2){
				for (int i = x1 + 1, j = y1 - 1; i < x2 && j > y2; i++, j--){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
			}
			return 1;
		}
		break;
	}
	case ROOK: {
		if (x1 != x2 && y1 != y2){
			return 0;
		}
		if (x1 > x2 && y1 == y2){
			for (int i = x1 - 1; i > x2; i--){
				if (game[i][y1].piece != EMPTY){
					return 0;
				}
			}
		}
		if (x2 > x1 && y1 == y2){
			for (int i = x1 + 1; i < x2; i++){
				if (game[i][y1].piece != EMPTY){
					return 0;
				}
			}
		}
		if (y1 > y2 && x1 == x2){
			for (int i = y1 - 1; i > y2; i--){
				if (game[x1][i].piece != EMPTY){
					return 0;
				}
			}
		}
		if (y2 > y1 && x1 == x2){
			for (int i = y1 + 1; i < y2; i++){
				if (game[x1][i].piece != EMPTY){
					return 0;
				}
			}
		}
		return 1;
		break;
	}
	case QUEEN: {
		/*Diagonal*/
		if ((x1 + y1 == x2 + y2) || (x1 - y1 == x2 - y2)){
			if (x1 > x2 && y1 > y2){
				for (int i = x1 - 1, j = y1 - 1; i > x2 && j > y2; i--, j--){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
				return 1;
			}
			else if (x1 < x2 && y1 < y2){
				for (int i = x1 + 1, j = y1 + 1; i < x2 && j < y2; i++, j++){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
				return 1;
			}
			else if (x1 > x2 && y1 < y2){
				for (int i = x1 - 1, j = y1 + 1; i > x2 && j < y2; i--, j++){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
				return 1;
			}
			else if (x1 < x2 && y1 > y2){
				for (int i = x1 + 1, j = y1 - 1; i < x2 && j > y2; i++, j--){
					if (game[i][j].piece != EMPTY){
						return 0;
					}
				}
			}
			return 1;
		}
		/*Horizontal and Vertical*/
		if (x1 > x2 && y1 == y2){
			for (int i = x1 - 1; i > x2; i--){
				if (game[i][y1].piece != EMPTY){
					return 0;
				}
			}
			return 1;
		}
		if (x2 > x1 && y1 == y2){
			for (int i = x1 + 1; i < x2; i++){
				if (game[i][y1].piece != EMPTY){
					return 0;
				}
			}
			return 1;
		}
		if (y1 > y2 && x1 == x2){
			for (int i = y1 - 1; i > y2; i--){
				if (game[x1][i].piece != EMPTY){
					return 0;
				}
			}
			return 1;
		}
		if (y2 > y1 && x1 == x2){
			for (int i = y1 + 1; i < y2; i++){
				if (game[x1][i].piece != EMPTY){
					return 0;
				}
			}
			return 1;
		}
		break;
	}
	case KING: {
		/*Castling*/
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (game[i][j].piece == KING && game[i][j].colour == game[x1][y1].colour){
					x_king = i;
					y_king = j;
				}
			}
		}

		if (abs(x1 - x2) == 2){
			if (Check(x_king, y_king) == false && Check((x_king + x2) / 2, y_king) == false && game[x1][y1].piece == KING){
				if (game[x1][y1].colour == LIGHT){	//White
					if (x2 == 3 && y2 == 8 && castle_white_queen == true){
						if (game[2][8].piece == EMPTY && game[3][8].piece == EMPTY && game[4][8].piece == EMPTY && game[1][8].piece == ROOK){
							return 1;
						}
					}
					else if (x2 == 7 && y2 == 8 && castle_white_king == true && game[8][8].piece == ROOK){
						if (game[6][8].piece == EMPTY && game[7][8].piece == EMPTY){
							return 1;
						}
					}
				}
				else if (game[x1][y1].colour == DARK){	//Black
					if (x2 == 3 && y2 == 1 && castle_black_queen == true){
						if (game[2][1].piece == EMPTY && game[3][1].piece == EMPTY && game[4][1].piece == EMPTY && game[1][1].piece == ROOK){
							return 1;
						}
					}
					else if (x2 == 7 && y2 == 1 && castle_black_king == true){
						if (game[6][1].piece == EMPTY && game[7][1].piece == EMPTY && game[8][1].piece == ROOK){
							return 1;
						}
					}
				}
			}
		}
		/*King normal move*/
		if (x2 - x1 > 1 || x1 - x2 > 1 || y2 - y1 > 1 || y1 - y2 > 1){
			return 0;
		}
		return 1;
		break;
	}
	}
	return 0;
}
bool Game::Check(int x, int y){
	bool colour = game[x][y].colour;
	/*Pawn*/
	if (colour == true){
		if ((game[x - 1][y - 1].piece == PAWN && game[x - 1][y - 1].colour == (!colour) && y > 1 && x > 1) ||
			(game[x + 1][y - 1].piece == PAWN && game[x + 1][y - 1].colour == (!colour) && x < 8 && y > 1))
		{
			return true;
		}
	}
	else {
		if ((game[x - 1][y + 1].piece == PAWN && game[x - 1][y + 1].colour == (!colour) && x > 1 && y < 8) ||
			(game[x + 1][y + 1].piece == PAWN && game[x + 1][y + 1].colour == (!colour) && x < 8 && y < 8))
		{
			return true;
		}
	}
	/*Knight*/
	if ((game[x + 1][y + 2].piece == KNIGHT && game[x + 1][y + 2].colour == (!colour)) && x + 1 <= 8 && y + 2 <= 8 ||
		(game[x + 1][y - 2].piece == KNIGHT && game[x + 1][y - 2].colour == (!colour)) && x + 1 <= 8 && y - 2 >= 1 ||
		(game[x - 1][y + 2].piece == KNIGHT && game[x - 1][y + 2].colour == (!colour)) && x - 1 >= 1 && y + 2 <= 8 ||
		(game[x - 1][y - 2].piece == KNIGHT && game[x - 1][y - 2].colour == (!colour)) && x - 1 >= 1 && y - 2 >= 1 ||
		(game[x + 2][y + 1].piece == KNIGHT && game[x + 2][y + 1].colour == (!colour)) && x + 2 <= 8 && y + 1 <= 8 ||
		(game[x + 2][y - 1].piece == KNIGHT && game[x + 2][y - 1].colour == (!colour)) && x + 2 <= 8 && y - 1 >= 1 ||
		(game[x - 2][y + 1].piece == KNIGHT && game[x - 2][y + 1].colour == (!colour)) && x - 2 >= 1 && y + 1 <= 8 ||
		(game[x - 2][y - 1].piece == KNIGHT && game[x - 2][y - 1].colour == (!colour)) && x - 2 >= 1 && y - 1 >= 1)
	{
		return true;
	}
	/*Diagonal - For bishop and queen*/
	for (int i = x - 1, j = y - 1; i >= 1 && j >= 1; i--, j--){
		if ((game[i][j].piece == BISHOP || game[i][j].piece == QUEEN) && game[i][j].colour == (!colour)){
			return true;
		}
		if (game[i][j].piece != EMPTY){
			break;
		}
	}
	for (int i = x - 1, j = y + 1; i >= 1 && j <= 8; i--, j++){
		if ((game[i][j].piece == BISHOP || game[i][j].piece == QUEEN) && game[i][j].colour == (!colour)){
			return true;
		}
		if (game[i][j].piece != EMPTY){
			break;
		}
	}
	for (int i = x + 1, j = y - 1; i <= 8 && j >= 1; i++, j--){
		if ((game[i][j].piece == BISHOP || game[i][j].piece == QUEEN) && game[i][j].colour == (!colour)){
			return true;
		}
		if (game[i][j].piece != EMPTY){
			break;
		}
	}
	for (int i = x + 1, j = y + 1; i <= 8 && j <= 8; i++, j++){
		if ((game[i][j].piece == BISHOP || game[i][j].piece == QUEEN) && game[i][j].colour == (!colour)){
			return true;
		}
		if (game[i][j].piece != EMPTY){
			break;
		}
	}
	/*Straight - For rook and queen*/
	for (int i = x + 1; i <= 8; i++){
		if ((game[i][y].piece == ROOK || game[i][y].piece == QUEEN) && game[i][y].colour == (!colour)){
			return true;
		}
		if (game[i][y].piece != EMPTY){
			break;
		}
	}
	for (int i = x - 1; i >= 1; i--){
		if ((game[i][y].piece == ROOK || game[i][y].piece == QUEEN) && game[i][y].colour == (!colour)){
			return true;
		}
		if (game[i][y].piece != EMPTY){
			break;
		}
	}
	for (int j = y + 1; j <= 8; j++){
		if ((game[x][j].piece == ROOK || game[x][j].piece == QUEEN) && game[x][j].colour == (!colour)){
			return true;
		}
		if (game[x][j].piece != EMPTY){
			break;
		}
	}
	for (int j = y - 1; j >= 1; j--){
		if ((game[x][j].piece == ROOK || game[x][j].piece == QUEEN) && game[x][j].colour == (!colour)){
			return true;
		}
		if (game[x][j].piece != EMPTY){
			break;
		}
	}
	/*King*/
	for (int i = x - 1; i <= x + 1; i++){
		for (int j = y - 1; j <= y + 1; j++){
			if (game[i][j].piece == KING && game[i][j].colour == (!colour)){
				return true;
			}
		}
	}
	return false;
}
int Game::Checkmate(bool colour, int mode){	// mode 1 for normal, mode 2 for 50 moves draw; colour of playing player
	int status = 0; //0

	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			if (game[i][j].colour == colour){
				for (int m = 1; m <= 8; m++){
					for (int n = 1; n <= 8; n++){
						if (Validity(i, j, m, n) == true){
							status = 1;
							break;
						}
					}
					if (status == 1)break;
				}
			}
			if (status == 1)break;
		}
		if (status == 1)break;
	}

	/*Special cases of stalemate*/
	int pieces_left = 0, knight_left = 0, bishop_left = 0;
	for (int i = 0; i <= 8; i++){
		for (int j = 0; j <= 8; j++){
			if (game[i][j].piece != EMPTY)pieces_left++;
			if (game[i][j].piece == KNIGHT)knight_left++;
			if (game[i][j].piece == BISHOP)bishop_left++;
		}
	}
	if (pieces_left == 2 || (pieces_left == 3 && (knight_left == 1 || bishop_left == 1))){
		return MODE_ENDGAME_DRAW;
	}

	if (mode == 1){
		int x_king, y_king;
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (game[i][j].piece == KING && game[i][j].colour == colour){
					x_king = i;
					y_king = j;
					break;
				}
			}
		}

		if (status == 0 && Check(x_king, y_king) == true){
			return 1;
		}
		if (status == 0 && Check(x_king, y_king) == false){
			return MODE_ENDGAME_STALEMATE;
		}
		return 0;
	}

	else if (mode == 2){
		if (status == 0){
			return MODE_ENDGAME_DRAW;
		}
	}

}
void Game::Control(int xClick, int yClick, int mode){	//mode 1 for click (actual game), mode 2 for replay, mode 3 for load, mode 4 for synth move
	SaveObj.Special(0);

	if ((sessionState == TWO_PLAYER_LOCAL && RealGame.turn == DARK && board_rotation == true) || (sessionState == ONE_PLAYER && Player1.Colour == LIGHT && RealGame.turn == DARK) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && Player1.Colour == DARK && mode == 1) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && Player2.Colour == DARK && mode == 1)){
		xClick = 9 - xClick;	yClick = 9 - yClick;	// Click adjustments
	}

	/*Touch Piece*/
	bool flag = 0;
	if (touch_piece == true && highlighted_x == 0 && highlighted_y == 0){
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				if (RealGame.Validity(xClick, yClick, i, j) == true){
					flag = 1;
					break;
				}
			}
			if (flag == 1) break;
		}
		if (flag == 0) return;
	}

	// De-highlights if clicked on the same square which was highlighted
	if (xClick == highlighted_x && yClick == highlighted_y && touch_piece == false){
		if (!soundMute){
			PlaySound(TEXT("Sounds/Click.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}
		//RealGame.game[xClick][yClick].Draw(MODE_DRAW_NORMAL);
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				RealGame.game[i][j].Draw(MODE_DRAW_NORMAL);
				if (isCheck == true && i == xking && j == yking){
					RealGame.game[i][j].Draw(MODE_DRAW_CHECK);
				}
			}
		}

	}

	// See if a piece is highligted and make a move if it is valid
	else if (highlight && RealGame.Validity(highlighted_x, highlighted_y, xClick, yClick)){

		/*En Passant*/
		if (RealGame.game[xClick][yClick].piece == EMPTY){
			if ((abs(xClick - highlighted_x) == 1 && abs(yClick - highlighted_y) == 1) && EP_x != 0 && EP_y != 0 && RealGame.game[highlighted_x][highlighted_y].piece == PAWN){
				int temp_hx = highlighted_x, temp_hy = highlighted_y;	// For the actual pawn move to be possible, since drawing the square resets highlight

				//Killed piece
				if (RealGame.game[EP_x][EP_y].piece != EMPTY){
					if (RealGame.game[EP_x][EP_y].colour == LIGHT){
						for (int i = 0; i <= 15; i++){
							if (killed_white[i] == EMPTY){
								killed_white[i] = RealGame.game[EP_x][EP_y].piece;
								break;
							}
						}
					}
					else {
						for (int i = 0; i <= 15; i++){
							if (killed_black[i] == EMPTY){
								killed_black[i] = RealGame.game[EP_x][EP_y].piece;
								break;
							}
						}
					}
				}

				// Save
				SaveObj.Special(8);
				SaveObj.Piece2(RealGame.game[EP_x][EP_y].piece);

				RealGame.game[EP_x][EP_y].piece = EMPTY;
				RealGame.game[EP_x][EP_y].Draw(MODE_DRAW_NORMAL);

				// Restore the values
				highlighted_x = temp_hx; highlighted_y = temp_hy;
			}
		}

		/*The actual move*/
		// Save only during actual game, not replay
		if (mode == 1 || mode == 4){
			SaveObj.Initial(highlighted_x, highlighted_y);
			SaveObj.Killed(xClick, yClick);
			SaveObj.Piece1(RealGame.game[highlighted_x][highlighted_y].piece);
			if (SaveObj.GetSpecial() != 8){	//Skip if En Passant was done
				SaveObj.Piece2(RealGame.game[xClick][yClick].piece);
			}
		}
		// Set CASTLE to true if it is a castling, so that turn dosen't change (in move) and extra move can be made.
		if (abs(xClick - highlighted_x) == 2 && RealGame.game[highlighted_x][highlighted_y].piece == KING){
			RealGame.castle = true;
		}

		if (mode == 1 || mode == 2 || mode == 3 || mode == 4){
			// THE move
			if (!soundMute){
				PlaySound(TEXT("Sounds/Click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			RealGame.Move(highlighted_x, highlighted_y, xClick, yClick);

			// Move Check
			if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1){
				//argsForMoveCheck param;
				argListForMoveCheck.UserChecking = Player1.Name;
				argListForMoveCheck.UserMaking = Player2.Name;
				//_beginthread(Account::MoveCheck, 0, NULL);
			}
			if (sessionState == TWO_PLAYER_INTERNET_PLAYER_2){
				//argsForMoveCheck param;
				argListForMoveCheck.UserChecking = Player2.Name;
				argListForMoveCheck.UserMaking = Player1.Name;
				//_beginthread(Account::MoveCheck, 0, NULL);
			}
		}

		/*Castling*/
		if (xClick == 3 && yClick == 8 && RealGame.castle == true){
			RealGame.castle = false;
			SaveObj.Special(7);
			RealGame.Move(1, 8, 4, 8);
		}
		else if (xClick == 7 && yClick == 8 && RealGame.castle == true){
			RealGame.castle = false;
			SaveObj.Special(7);
			RealGame.Move(8, 8, 6, 8);
		}
		else if (xClick == 3 && yClick == 1 && RealGame.castle == true){
			RealGame.castle = false;
			SaveObj.Special(7);
			RealGame.Move(1, 1, 4, 1);
		}
		else if (xClick == 7 && yClick == 1 && RealGame.castle == true){
			RealGame.castle = false;
			SaveObj.Special(7);
			RealGame.Move(8, 1, 6, 1);
		}
	}

	// Highlight square, white's turn
	else if (RealGame.turn == LIGHT){
		if (RealGame.game[xClick][yClick].colour == LIGHT && RealGame.game[xClick][yClick].piece != 0){
			// Touch piece return, can't change the highlight
			if (touch_piece == true && highlighted_x != 0 && highlighted_y != 0){
				return;
			}

			if (highlight){
				//RealGame.game[highlighted_x][highlighted_y].Draw(MODE_DRAW_NORMAL);	// Dehighlight if any other piece is highlighted
				for (int i = 1; i <= 8; i++){
					for (int j = 1; j <= 8; j++){
						RealGame.game[i][j].Draw(MODE_DRAW_NORMAL);
						if (isCheck == true && i == xking && j == yking){
							RealGame.game[i][j].Draw(MODE_DRAW_CHECK);
						}
					}
				}
			}

			if (!soundMute){
				PlaySound(TEXT("Sounds/Click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			RealGame.game[xClick][yClick].Draw(MODE_DRAW_HIGHLIGHT);

			if (beginnerMode){
				for (int i = 1; i <= 8; i++){
					for (int j = 1; j <= 8; j++){
						if (RealGame.Validity(xClick, yClick, i, j)){
							RealGame.game[i][j].Draw(MODE_DRAW_BEGINNER);
						}
						if (isCheck == true && i == xking && j == yking){
							RealGame.game[i][j].Draw(MODE_DRAW_CHECK);
						}
					}
				}
			}
		}
	}
	// Highlight square, black's turn
	else if (RealGame.turn == DARK){
		if (RealGame.game[xClick][yClick].colour == DARK && RealGame.game[xClick][yClick].piece != 0){
			// Touch piece return, can't change the highlight
			if (touch_piece == true && highlighted_x != 0 && highlighted_y != 0){
				return;
			}

			if (highlight){
				//RealGame.game[highlighted_x][highlighted_y].Draw(MODE_DRAW_NORMAL);	// Dehighlight if any other piece is highlighted
				for (int i = 1; i <= 8; i++){
					for (int j = 1; j <= 8; j++){
						RealGame.game[i][j].Draw(MODE_DRAW_NORMAL);
						if (isCheck == true && i == xking && j == yking){
							RealGame.game[i][j].Draw(MODE_DRAW_CHECK);
						}
					}
				}
			}
			if (!soundMute){
				PlaySound(TEXT("Sounds/Click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			RealGame.game[xClick][yClick].Draw(MODE_DRAW_HIGHLIGHT);

			if (beginnerMode){
				for (int i = 1; i <= 8; i++){
					for (int j = 1; j <= 8; j++){
						if (RealGame.Validity(xClick, yClick, i, j)){
							RealGame.game[i][j].Draw(MODE_DRAW_BEGINNER);
						}
						if (isCheck == true && i == xking && j == yking){
							RealGame.game[i][j].Draw(MODE_DRAW_CHECK);
						}
					}
				}
			}
		}
	}
}
void Game::PawnPromotion(){
	wxClientDC dc(promotionPane);
	promotionPane->Units();
	if (game[promotionPane->promote_x][promotionPane->promote_y].colour == LIGHT){
		promotionPane->promote_colour = LIGHT;
	}
	else {
		promotionPane->promote_colour = DARK;
	}
	dc.SetBackground(wxColor(40, 40, 40));
	dc.Clear();

	promotionPane->piece_number = 1;
	promotionPane->promote_piece = QUEEN;
	game[0][0].Draw(MODE_DRAW_PROMOTION);
	promotionPane->piece_number = 2;
	promotionPane->promote_piece = BISHOP;
	game[0][0].Draw(MODE_DRAW_PROMOTION);
	promotionPane->piece_number = 3;
	promotionPane->promote_piece = ROOK;
	game[0][0].Draw(MODE_DRAW_PROMOTION);
	promotionPane->piece_number = 4;
	promotionPane->promote_piece = KNIGHT;
	game[0][0].Draw(MODE_DRAW_PROMOTION);
}
void Game::PawnPromotion(int select){
	wxClientDC dc(promotionPane);
	dc.SetBackground(wxColor(40, 40, 40));
	dc.Clear();
	if (select == 1){	// QUEEN
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].piece = QUEEN;
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].Draw(MODE_DRAW_NORMAL);
		promotionPane->promotion_state = false;
		SaveObj.Special(5);
	}
	if (select == 2){	// BISHOP
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].piece = BISHOP;
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].Draw(MODE_DRAW_NORMAL);
		promotionPane->promotion_state = false;
		SaveObj.Special(3);
	}
	if (select == 3){	// ROOK
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].piece = ROOK;
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].Draw(MODE_DRAW_NORMAL);
		promotionPane->promotion_state = false;
		SaveObj.Special(4);
	}
	if (select == 4){	// KNIGHT
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].piece = KNIGHT;
		RealGame.game[promotionPane->promote_x][promotionPane->promote_y].Draw(MODE_DRAW_NORMAL);
		promotionPane->promotion_state = false;
		SaveObj.Special(2);
	}
	SaveObj.Save();
	RealGame.UpdateLogWindow();

	if ((sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && RealGame.turn == Player2.Colour) || (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 &&  RealGame.turn == Player1.Colour)){
		PromoteRegisterParameter = select;
		_beginthread(Account::PromoteRegister, 0, NULL);
	}

	while (isPromoteRegisterRunning){
		wxMilliSleep(500);
	}
	if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1 && RealGame.turn == Player2.Colour) {
		_beginthread(Account::MoveRegister, 0, NULL);
	}
	if (sessionState == TWO_PLAYER_INTERNET_PLAYER_2 && RealGame.turn == Player1.Colour) {
		_beginthread(Account::MoveRegister, 0, NULL);
	}



	moves += 0.5;

	if (sessionState == ONE_PLAYER && Player1.Colour == turn){
		if (Player1.Colour == DARK){	//Change turn to AI
			RealGame.turn = DARK;
		}
		else {
			RealGame.turn = LIGHT;
		}
		// Refresh board
		for (int i = 1; i <= 8; i++){
			for (int j = 1; j <= 8; j++){
				game[i][j].Draw(MODE_DRAW_NORMAL);
			}
		}
		CPWACN->moveAI();
	}

	// Refresh board if it does not enter if condition
	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			game[i][j].Draw(MODE_DRAW_NORMAL);
		}
	}
}
void Game::Replay(void*){
	//Encryption("Log.txt");	// Decrypt
	ifstream file;
	int x1, y1, x2, y2;
	char buffer;
	file.open("Log.txt");
	for (int i = 0;; i++) {
		file.seekg(i * 6);
		file.get(buffer);
		x1 = buffer;
		file.get(buffer);
		y1 = buffer;
		file.get(buffer);
		x2 = buffer;
		file.get(buffer);
		y2 = buffer;
		file.get(buffer);	//For newline character

		for (int i = 97; i <= 104; i++){
			if (x1 == i){
				x1 -= 96;
				break;
			}
		}
		for (int i = 49; i <= 56; i++){
			if (y1 == i){
				y1 -= 48;
				break;
			}
		}
		y1 = 9 - y1;
		for (int i = 97; i <= 104; i++){
			if (x2 == i){
				x2 -= 96;
				break;
			}
		}
		for (int i = 49; i <= 56; i++){
			if (y2 == i){
				y2 -= 48;
				break;
			}
		}
		y2 = 9 - y2;

		Control(x1, y1, 2);
		wxMilliSleep(200);
		Control(x2, y2, 2);
		wxMilliSleep(200);

		if (file.eof())break;
	}
	file.close();
	//Encryption("Log.txt");	// Encrypt
}
void Game::Save(){
	ofstream file;
	file.open("Other.dat", ios::out | ios::binary);
	file.write((char*)&killed_black, sizeof(killed_black));
	file.write((char*)&killed_white, sizeof(killed_white));
	file.write((char*)&EP_x, sizeof(EP_x));
	file.write((char*)&EP_y, sizeof(EP_y));
	file.close();

	file.open("Main.dat", ios::out | ios::binary);
	file.write((char*)&RealGame, sizeof(RealGame));
	file.close();
}
void Game::Load(){
	ifstream file;
	file.open("Other.dat", ios::out | ios::binary);
	file.read((char*)&killed_black, sizeof(killed_black));
	file.read((char*)&killed_white, sizeof(killed_white));
	file.read((char*)&EP_x, sizeof(EP_x));
	file.read((char*)&EP_y, sizeof(EP_y));
	file.close();

	file.open("Main.dat", ios::out | ios::binary);
	file.read((char*)&RealGame, sizeof(RealGame));
	file.close();

	for (int i = 1; i <= 8; i++){
		for (int j = 1; j <= 8; j++){
			RealGame.game[i][j].Draw(MODE_DRAW_NORMAL);
		}
	}
	drawPane->Reload();
}
void Game::Initialize(){
	// Reset game options
	board_rotation = true;
	touch_piece = false;
	beginnerMode = false;
	GameEnded = false;

	// Reset other things
	EP_x = 0;
	EP_y = 0;
	stale50_light = 0;
	stale50_dark = 0;
	highlighted_x = 0;
	highlighted_y = 0;
	xking = yking = 0;

	for (int i = 0; i < 16; i++){
		killed_black[i] = EMPTY;
		killed_white[i] = EMPTY;
	}

	isCheck = false;

	// Reset all castling parameters
	RealGame.turn = LIGHT;
	RealGame.castle_white_queen = true;
	RealGame.castle_white_king = true;
	RealGame.castle_black_queen = true;
	RealGame.castle_black_king = true;
	RealGame.castle = false;
	RealGame.king_white_x = 5;
	RealGame.king_white_y = 8;
	RealGame.king_black_x = 5;
	RealGame.king_black_y = 1;

	// Reset all the pieces
	for (i = 1; i <= 8; i++){
		for (j = 1; j <= 2; j++){
			RealGame.game[i][j].x = i; RealGame.game[i][j].y = j;
			RealGame.game[i][j].colour = DARK;
		}
		for (j = 3; j <= 6; j++) {
			RealGame.game[i][j].x = i; RealGame.game[i][j].y = j;
		}
		for (j = 7; j <= 8; j++){
			RealGame.game[i][j].x = i; RealGame.game[i][j].y = j;
			RealGame.game[i][j].colour = LIGHT;
		}
	}
	RealGame.game[1][1].piece = RealGame.game[1][8].piece = ROOK;
	RealGame.game[2][1].piece = RealGame.game[2][8].piece = KNIGHT;
	RealGame.game[3][1].piece = RealGame.game[3][8].piece = BISHOP;
	RealGame.game[4][1].piece = RealGame.game[4][8].piece = QUEEN;
	RealGame.game[5][1].piece = RealGame.game[5][8].piece = KING;
	RealGame.game[6][1].piece = RealGame.game[6][8].piece = BISHOP;
	RealGame.game[7][1].piece = RealGame.game[7][8].piece = KNIGHT;
	RealGame.game[8][1].piece = RealGame.game[8][8].piece = ROOK;

	for (int i = 1; i <= 8; i++){
		RealGame.game[i][2].piece = RealGame.game[i][7].piece = PAWN;
	}
	for (int i = 1; i <= 8; i++){
		for (int j = 3; j <= 6; j++){
			RealGame.game[i][j].piece = EMPTY;
		}
	}
}
void Game::EndGame(int mode){
	// Give the message:
	UIObj.InfoBar->Clear();
	if (mode == MODE_ENDGAME_CHECKMATE_1){
		wxMessageBox("Checkmate!", Player1.Name + " wins!");
		UIObj.InfoBar->AppendText("0 - 1");	//Since Player 2 box is first
	}
	else if (mode == MODE_ENDGAME_CHECKMATE_2){
		wxMessageBox("Checkmate!", Player2.Name + " wins!");
		UIObj.InfoBar->AppendText("1 - 0");	//Since Player 1 box is second
	}
	else if (mode == MODE_ENDGAME_STALEMATE){
		wxMessageBox("Stalemate.", "Game Over");
		UIObj.InfoBar->AppendText("0.5 - 0.5");
	}
	else if (mode == MODE_ENDGAME_DRAW){
		wxMessageBox("It's a Draw.", "Game Over");
		UIObj.InfoBar->AppendText("0.5 - 0.5");
	}

	if (internetConnected == false || serverConnected == false){
		drawPane->Reload();
		sessionState = NONE;
		return;
	}

	wxString gameName;
	// Save log files on the server
	if (sessionState == ONE_PLAYER) {
		if (Player1.Name == "CPWACN - Chess Player Without A Cool Name") {
			if (gameName == ""){
				gameName = "Game-";
				int x = rand() % 10000;
				gameName << x;
			}
			// Upload the log file
			argListForUploadFile.userName = Player1.Name;
			argListForUploadFile.fileName = gameName;
			argListForUploadFile.mode = COMPLETED;
			_beginthread(Account::UploadGame, 0, (void*)&argListForUploadFile);
			// Update stats
			argListForStatUpdate.username = Player1.Name;
			if (mode == MODE_ENDGAME_CHECKMATE_1){
				argListForStatUpdate.mode = MODE_STAT_UPDATE_WIN;
			}
			else if (mode == MODE_ENDGAME_CHECKMATE_2){
				argListForStatUpdate.mode = MODE_STAT_UPDATE_LOSE;
			}
			else {
				argListForStatUpdate.mode = MODE_STAT_UPDATE_DRAW;
			}
			_beginthread(Account::StatUpdate, 0, (void*)&argListForStatUpdate);
		}
		if (Player2.Name != "GUEST") {
			gameName = wxGetTextFromUser("Enter a name for this game:");
			if (gameName == ""){
				gameName = "Game-";
				int x = rand() % 10000;
				gameName << x;
			}
			// Upload the log file
			argListForUploadFile.userName = Player2.Name;
			argListForUploadFile.fileName = gameName;
			argListForUploadFile.mode = COMPLETED;
			_beginthread(Account::UploadGame, 0, (void*)&argListForUploadFile);
		}
		// Update stats
		argListForStatUpdate.username = Player2.Name;
		if (mode == MODE_ENDGAME_CHECKMATE_2){
			argListForStatUpdate.mode = MODE_STAT_UPDATE_WIN;
		}
		else if (mode == MODE_ENDGAME_CHECKMATE_1){
			argListForStatUpdate.mode = MODE_STAT_UPDATE_LOSE;
		}
		else {
			argListForStatUpdate.mode = MODE_STAT_UPDATE_DRAW;
		}
		_beginthread(Account::StatUpdate, 0, (void*)&argListForStatUpdate);
	}
	else if (sessionState == TWO_PLAYER_LOCAL) {
		if (Player1.Name != "GUEST") {
			gameName = wxGetTextFromUser(Player1.Name + " - Enter a name for this game:");
			if (gameName == ""){
				gameName = "Game-";
				int x = rand() % 10000;
				gameName << x;
			}
			// Upload the log file
			argListForUploadFile.userName = Player1.Name;
			argListForUploadFile.fileName = gameName;
			argListForUploadFile.mode = COMPLETED;
			_beginthread(Account::UploadGame, 0, (void*)&argListForUploadFile);
			// Update stats
			argListForStatUpdate.username = Player1.Name;
			if (mode == MODE_ENDGAME_CHECKMATE_1){
				argListForStatUpdate.mode = MODE_STAT_UPDATE_WIN;
			}
			else if (mode == MODE_ENDGAME_CHECKMATE_2){
				argListForStatUpdate.mode = MODE_STAT_UPDATE_LOSE;
			}
			else {
				argListForStatUpdate.mode = MODE_STAT_UPDATE_DRAW;
			}
			_beginthread(Account::StatUpdate, 0, (void*)&argListForStatUpdate);
		}
		if (Player2.Name != "GUEST") {
			gameName = wxGetTextFromUser(Player2.Name + " - Enter a name for this game:");
			if (gameName == ""){
				gameName = "Game-";
				int x = rand() % 10000;
				gameName << x;
			}
			// Upload the log file
			argListForUploadFile.userName = Player2.Name;
			argListForUploadFile.fileName = gameName;
			argListForUploadFile.mode = COMPLETED;
			_beginthread(Account::UploadGame, 0, (void*)&argListForUploadFile);
			// Update stats
			argListForStatUpdate.username = Player2.Name;
			if (mode == MODE_ENDGAME_CHECKMATE_2){
				argListForStatUpdate.mode = MODE_STAT_UPDATE_WIN;
			}
			else if (mode == MODE_ENDGAME_CHECKMATE_1){
				argListForStatUpdate.mode = MODE_STAT_UPDATE_LOSE;
			}
			else {
				argListForStatUpdate.mode = MODE_STAT_UPDATE_DRAW;
			}
			_beginthread(Account::StatUpdate, 0, (void*)&argListForStatUpdate);
		}
	}
	else if (sessionState == TWO_PLAYER_INTERNET_PLAYER_1) {
		gameName = wxGetTextFromUser(Player1.Name + " - Enter a name for this game:");
		if (gameName == ""){
			gameName = "Game-";
			int x = rand() % 10000;
			gameName << x;
		}
		// Upload the log file
		argListForUploadFile.userName = Player1.Name;
		argListForUploadFile.fileName = gameName;
		argListForUploadFile.mode = COMPLETED;
		_beginthread(Account::UploadGame, 0, (void*)&argListForUploadFile);
		// Update stats
		argListForStatUpdate.username = Player1.Name;
		if (mode == MODE_ENDGAME_CHECKMATE_1){
			argListForStatUpdate.mode = MODE_STAT_UPDATE_WIN;
		}
		else if (mode == MODE_ENDGAME_CHECKMATE_2){
			argListForStatUpdate.mode = MODE_STAT_UPDATE_LOSE;
		}
		else {
			argListForStatUpdate.mode = MODE_STAT_UPDATE_DRAW;
		}
		_beginthread(Account::StatUpdate, 0, (void*)&argListForStatUpdate);

	}
	else if (sessionState == TWO_PLAYER_INTERNET_PLAYER_2){
		gameName = wxGetTextFromUser(Player2.Name + " - Enter a name for this game:");
		if (gameName == ""){
			gameName = "Game-";
			int x = rand() % 10000;
			gameName << x;
		}
		// Upload the log file
		argListForUploadFile.userName = Player2.Name;
		argListForUploadFile.fileName = gameName;
		argListForUploadFile.mode = COMPLETED;
		_beginthread(Account::UploadGame, 0, (void*)&argListForUploadFile);
		// Update stats
		argListForStatUpdate.username = Player2.Name;
		if (mode == MODE_ENDGAME_CHECKMATE_2){
			argListForStatUpdate.mode = MODE_STAT_UPDATE_WIN;
		}
		else if (mode == MODE_ENDGAME_CHECKMATE_1){
			argListForStatUpdate.mode = MODE_STAT_UPDATE_LOSE;
		}
		else {
			argListForStatUpdate.mode = MODE_STAT_UPDATE_DRAW;
		}
		_beginthread(Account::StatUpdate, 0, (void*)&argListForStatUpdate);
	}

	drawPane->Reload();
	sessionState = NONE;
}
void Game::Shutdown(){
	if (!isExit){
		return;
	}
	isExit = false;	// Because Shutdown was being called more than once due to destructors or something.
	_beginthread(ExitSound, 0, NULL);
	wxBeginBusyCursor();

	Encryption("LoggedInPlayersTemp.txt");	// Decrypt
	// To logout all temporary logged in players
	ifstream file("LoggedInPlayersTemp.txt", ios::in);
	char temp[100];
	while (!file.eof()){
		file.getline(temp, 100, '~');
		//Logout
		wxString Username = temp;
		if (Username == ""){
			break;
		}
		wxString Info = "uname=";
		Info.Append(Username);

		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();

		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/logout/logout_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(Info));

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);

		char * ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);
		delete[] ptr;
	}
	file.close();
	Encryption("LoggedInPlayersTemp.txt");	// Encrypt

	Encryption("LoggedInPlayers.txt");	// Decrypt
	file.open("LoggedInPlayers.txt", ios::in);
	while (!file.eof()){
		file.getline(temp, 100, '~');
		//Logout
		wxString Username = temp;
		if (Username == ""){
			break;
		}
		wxString Info = "uname=";
		Info.Append(Username);

		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();

		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/logout/logout_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(Info));

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);

		char * ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);
		delete[] ptr;
	}
	file.close();
	Encryption("LoggedInPlayers.txt");	// Encrypt

	wxEndBusyCursor();
	while (isExitSoundPlaying){
		wxMilliSleep(1);
	}
	wxExit();
}
void Game::QuitGame(){
	if (sessionState == ONE_PLAYER){
		delete CPWACN;
		CPWACN = NULL;
	}
	windowStart->Show(true);

	delete drawPane;
	drawPane = NULL;
	delete promotionPane;
	promotionPane = NULL;
	if (LogWindow) delete LogWindow;
	LogWindow = NULL;
	delete StatusBarGame;

	isExit = false;
	windowGame->Show(false);
	delete windowGame;
	isExit = true;
	windowGame = NULL;

	sessionState = NONE;
}

void Game::StartOnePlayer(){
	int Colour = UIObj.MyColour->GetSelection();
	if (Colour == 0){	//Player 1 is always AI
		Player1.Colour = DARK;
		Player2.Colour = LIGHT;
	}
	else{
		Player1.Colour = LIGHT;
		Player2.Colour = DARK;
	}
	Player1.Name = "CPWACN - Chess Player Without A Cool Name";
	Player2.Name = UIObj.NewOnePlayerUsers->GetStringSelection();

	int AILevel = UIObj.AILevel->GetSelection();
	if (AILevel == 0){
		CPWACN = new AI_1;
	}
	else if (AILevel = 1){
		CPWACN = new AI_2;
	}
}
void Game::StartTwoPlayerLocal(){
	int Colour1 = UIObj.Player1Colour->GetSelection();
	Player1.Colour = Colour1 == 0 ? LIGHT : DARK;
	int Colour2 = UIObj.Player1Colour->GetSelection();
	Player2.Colour = Colour2 == 0 ? LIGHT : DARK;

	Player1.Name = UIObj.NewTwoPlayerUser1->GetStringSelection();
	Player2.Name = UIObj.NewTwoPlayerUser2->GetStringSelection();
}

void Game::UpdateLogWindow(){
	//Encryption("Log.txt");	// Decrypt
	ifstream file("Log.txt");
	char temp[10];
	file.seekg(-9, ios::end);
	file.getline(temp, 10);
	LogWindow->AppendText("\n");
	LogWindow->AppendText(temp[0]);
	LogWindow->AppendText(temp[1]);
	LogWindow->AppendText(temp[2]);
	LogWindow->AppendText(temp[3]);
	file.close();
	//Encryption("Log.txt");	// Encrypt
}

/*SaveMoves Functions*/
void SaveMoves::Save(){
	//Encryption("Log.txt");	// Decrypt
	fstream file("Log.txt", ios::app);
	for (int i = 1; i <= 8; i++){
		if (x1 == i){
			file << (char)(96 + i);	//97 is 'a'
			break;
		}
	}
	file << 9 - y1;
	for (int i = 1; i <= 8; i++){
		if (x2 == i){
			file << (char)(96 + i);	//97 is 'a'
			break;
		}
	}
	file << 9 - y2;
	file << piece_moved << piece_killed << special_code;
	file << "\n";
	file.close();
	//Encryption("Log.txt");	// Encrypt

	ofstream ofile("ThreefoldDraw.dat", ios::out | ios::app | ios::binary);
	ofile.write((char*)&RealGame.game, sizeof(RealGame.game));
	ofile.close();
}

/*Account Functions*/
void Account::InternetCheck(void*){
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.example.com/");
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	curl_easy_cleanup(myHandle);

	if (!strcmp(ptr, "")){	// If there is no response from website
		internetConnected = false;
	}
	else {
		internetConnected = true;
	}
	delete ptr;
	initInternetCheck = true;

	// Set in status bar
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarStart->SetStatusText(status, 1);

	if (windowNewGame || windowNewGameOnePlayer || windowNewGameTwoPlayer || windowNewGameTwoPlayerLocal || windowNewGameTwoPlayerOnline || windowLoad || windowAccount || windowLogin || windowLogout || windowCreateAccount){
		StatusBarUI->SetStatusText(status, 1);
	}
	if (windowGame){
		StatusBarGame->SetStatusText(status, 1);
	}
	wxMilliSleep(6000);
	_beginthread(Account::InternetCheck, 0, NULL);
}
void Account::ServerCheck(void*){
	wxMilliSleep(50);
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/test.txt");
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	curl_easy_cleanup(myHandle);

	if (!strcmp(ptr, "YES")){
		serverConnected = true;
	}
	else {
		serverConnected = false;
	}
	delete ptr;
	initServerCheck = true;

	// Set in status bar
	wxString status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarStart->SetStatusText(status, 2);

	if (windowNewGame || windowNewGameOnePlayer || windowNewGameTwoPlayer || windowNewGameTwoPlayerLocal || windowNewGameTwoPlayerOnline || windowLoad || windowAccount || windowLogin || windowLogout || windowCreateAccount){
		StatusBarUI->SetStatusText(status, 2);
	}
	if (windowGame){
		StatusBarGame->SetStatusText(status, 2);
	}

	wxMilliSleep(2500);
	_beginthread(Account::ServerCheck, 0, NULL);
}
void Account::Login(){
	// Get the required details and add create string for cURL code
	wxString Username = UIObj.Login_Username_field->GetLineText(0);
	Username.MakeUpper();
	wxString Password = UIObj.Login_Password_field->GetLineText(0);
	wxString Info = "uname=";
	Info.Append(Username);
	Info.Append("&passwd=");
	Info.Append(Password);

	// Empty fields check
	if (Username == "" || Password == ""){
		wxMessageBox("You can't leave any of the fields blank!", "Chess | Login Alert");
		return;
	}

	// Check if user is already logged in

	// Decrypt
	Encryption("LoggedInPlayers.txt");
	Encryption("LoggedInPlayersTemp.txt");

	ifstream ifile;
	char name[100];
	wxString name_str;
	ifile.open("LoggedInPlayers.txt", ios::in);
	while (!ifile.eof()){
		ifile.getline(name, 100, '~');
		name_str = name;
		name_str.MakeUpper();
		if (name_str == Username){
			wxMessageBox("Already logged in!", "Chess | Login Alert");
			// Encrypt
			Encryption("LoggedInPlayers.txt");
			Encryption("LoggedInPlayersTemp.txt");
			return;
		}
	}
	ifile.close();

	ifile.open("LoggedInPlayersTemp.txt", ios::in);
	while (!ifile.eof()){
		ifile.getline(name, 100, '~');
		name_str = name;
		name_str.MakeUpper();
		if (name_str == Username){
			wxMessageBox("Already logged in!", "Chess | Login Alert");
			// Encrypt
			Encryption("LoggedInPlayers.txt");
			Encryption("LoggedInPlayersTemp.txt");
			return;
		}
	}
	ifile.close();

	// Encrypt
	Encryption("LoggedInPlayers.txt");
	Encryption("LoggedInPlayersTemp.txt");

	wxBeginBusyCursor();

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/login/login_process.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(Info));

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	curl_easy_cleanup(myHandle);

	wxEndBusyCursor();

	// Login Alert
	if (!strcmp(ptr, "9")){
		wxMessageBox("Logged in successfully!", "Chess | Login Alert");
	}
	else if (!strcmp(ptr, "5")){
		wxMessageBox("Server Error: Could not connect to database!", "Chess | Login Alert");
		return;
	}
	else if (!strcmp(ptr, "6")){
		wxMessageBox("Server Error: SQL error!", "Chess | Login Alert");
		return;
	}
	else if (!strcmp(ptr, "7")){
		wxMessageBox("Wrong username!", "Chess | Login Alert");
		return;
	}
	else if (!strcmp(ptr, "8")){
		wxMessageBox("Wrong password!", "Chess | Login Alert");
		return;
	}

	delete ptr;

	// Write username to file

	// Decrypt
	Encryption("LoggedInPlayers.txt");
	Encryption("LoggedInPlayersTemp.txt");
	ofstream ofile;
	if (UIObj.KeepLoggedIn->IsChecked()){
		ofile.open("LoggedInPlayers.txt", ios::out | ios::app);
	}
	else {
		ofile.open("LoggedInPlayersTemp.txt", ios::out | ios::app);
	}
	ofile << Username << "~";
	ofile.close();
	// Encrypt
	Encryption("LoggedInPlayers.txt");
	Encryption("LoggedInPlayersTemp.txt");

	UIObj.Login_Username_field->Clear();
	UIObj.Login_Password_field->Clear();
}
void Account::LoginStartup(void*){
	wxBeginBusyCursor();
	wxMilliSleep(2000);

	while (initInternetCheck == false || initServerCheck == false){
		wxMilliSleep(100);
	}
	if (serverConnected == false){
		wxMessageBox("No internet or server connection!", "Chess | LoginStartup Alert");
		isLoginStartupWorking = false;
		wxEndBusyCursor();
		return;
	}
	// Login the saved users
	Encryption("LoggedInPlayers.txt");	// Decrypt
	ifstream file("LoggedInPlayers.txt", ios::in);
	char temp[100];
	while (!file.eof()){
		file.getline(temp, 100, '~');
		//Login
		wxString Username = temp;
		if (Username == ""){
			break;
		}
		wxString Info = "uname=";
		Info.Append(Username);

		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();

		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/loginNoPwd/login_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(Info));

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);

		char * ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);
		delete ptr;
	}
	file.close();
	Encryption("LoggedInPlayers.txt");	// Encrypt

	isLoginStartupWorking = false;
	wxEndBusyCursor();
}
void Account::Logout(){
	wxBeginBusyCursor();
	// Get the required details and add create string for cURL code
	wxString Username = UIObj.LogoutUsers->GetStringSelection();
	wxString POST = "uname=";
	POST.Append(Username);

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/logout/logout_process.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	curl_easy_cleanup(myHandle);

	wxEndBusyCursor();

	if (!strcmp(ptr, "9")){
		wxMessageBox("Logged out!", "Chess | Logout Alert");
	}
	else if (!strcmp(ptr, "5")){
		wxMessageBox("Server Error: Could not connect to database!", "Chess | Logout Alert");
		return;
	}
	else if (!strcmp(ptr, "6")){
		wxMessageBox("Server Error: SQL error!", "Chess | Logout Alert");
		return;
	}
	else if (!strcmp(ptr, "7")){
		wxMessageBox("Wrong username!", "Chess | Logout Alert");
		return;
	}

	delete ptr;

	// Delete username from file
	char temp[100];
	Encryption("LoggedInPlayers.txt");	// Decrypt
	ifstream ifile("LoggedInPlayers.txt", ios::in);
	ofstream ofile("TempFile.txt", ios::out);
	int count = 1;	//Gets actual no. of lines + 2
	while (!ifile.eof()){
		ifile.getline(temp, 100, '~');
		count++;
	}
	ifile.close();
	ifile.open("LoggedInPlayers.txt", ios::in);
	for (int i = 1; i <= count - 2; i++){	//For having actual no. of lines (No idea why it is like this)
		ifile.getline(temp, 100, '~');
		if (strcmp(temp, Username) != 0){
			ofile << temp << "~";
		}
	}
	ifile.close();
	ofile.close();
	remove("LoggedInPlayers.txt");
	rename("TempFile.txt", "LoggedInPlayers.txt");
	Encryption("LoggedInPlayers.txt");	// Encrypt

	// Delete username from file for temporary logins
	Encryption("LoggedInPlayersTemp.txt");	// Decrypt
	ifile.open("LoggedInPlayersTemp.txt", ios::in);
	ofile.open("TempFile.txt", ios::out);
	count = 1;	//Gets actual no. of lines + 2
	while (!ifile.eof()){
		ifile.getline(temp, 100, '~');
		count++;
	}
	ifile.close();
	ifile.open("LoggedInPlayersTemp.txt", ios::in);
	for (int i = 1; i <= count - 2; i++){	//Need to have actual no. of lines (No idea why it is like this)
		ifile.getline(temp, 100, '~');
		if (strcmp(temp, Username) != 0){
			ofile << temp << "~";
		}
	}
	ifile.close();
	ofile.close();
	remove("LoggedInPlayersTemp.txt");
	rename("TempFile.txt", "LoggedInPlayersTemp.txt");
	Encryption("LoggedInPlayersTemp.txt");	// Encrypt
}
void Account::CreateAccount(){
	// Get the required details and add create string for cURL code
	wxString Username = UIObj.CreateAccount_Username_field->GetLineText(0);
	Username.MakeUpper();
	wxString Password = UIObj.CreateAccount_Password_field->GetLineText(0);
	wxString ConfirmPassword = UIObj.CreateAccount_ConfirmPassword_field->GetLineText(0);
	wxString Email = UIObj.CreateAccount_Email_field->GetLineText(0);
	wxString POST = "uname=";
	POST.Append(Username);
	POST.Append("&passwd=");
	POST.Append(Password);
	POST.Append("&emai=");
	POST.Append(Email);

	// Empty fields check
	if (Username == "" || Password == "" || ConfirmPassword == "" || Email == ""){
		wxMessageBox("You can't leave any of the fields blank!", "Chess | Create Account Alert");
		return;
	}
	if (ConfirmPassword != Password){
		wxMessageBox("The passwords don't match!", "Chess | Create Account Alert");
		return;
	}

	wxBeginBusyCursor();

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/newUser/verify.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	curl_easy_cleanup(myHandle);

	wxEndBusyCursor();

	if (!strcmp(ptr, "9")){
		wxMessageBox("Account created successfully!", "Chess | Create Account Alert");
	}
	else if (!strcmp(ptr, "0")){
		wxMessageBox("Server Error: Could not open file!", "Chess | Create Account Alert");
		return;
	}
	else if (!strcmp(ptr, "5")){
		wxMessageBox("Server Error: Could not connect to database!", "Chess | Create Account Alert");
		return;
	}
	else if (!strcmp(ptr, "6")){
		wxMessageBox("Server Error: SQL error!", "Chess | Create Account Alert");
		return;
	}
	else if (!strcmp(ptr, "7")){
		wxMessageBox("Username already taken! Pick something else.", "Chess | Create Account Alert");
		return;
	}
	else if (!strcmp(ptr, "8")){
		wxMessageBox("Email ID already registered!", "Chess | Create Account Alert");
		return;
	}

	delete ptr;

	UIObj.CreateAccount_Username_field->Clear();
	UIObj.CreateAccount_Password_field->Clear();
	UIObj.CreateAccount_ConfirmPassword_field->Clear();
	UIObj.CreateAccount_Email_field->Clear();
}
void Account::SendRequest(){
	wxBeginBusyCursor();
	// Intimate the user that the request is being sent!
	// Get the required details and add create string for cURL code
	wxString Username1 = UIObj.NewTwoPlayerUserOnline->GetStringSelection();
	wxString Username2 = UIObj.OnlinePlayerToPlay->GetStringSelection();
	int Colour = UIObj.PlayerOnlineColour->GetSelection();
	wxString Color = Colour == 0 ? "LIGHT" : "DARK";
	wxString Message = UIObj.Message->GetLineText(0);
	wxString POST = "uname1=";
	POST.Append(Username1);
	POST.Append("&uname2=");
	POST.Append(Username2);
	POST.Append("&color=");
	POST.Append(Color);
	POST.Append("&txt=");
	POST.Append(Message);

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/sendRequest/request_process.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	curl_easy_cleanup(myHandle);
	delete ptr;

	wxEndBusyCursor();

	// Now a new screen saying "Waiting for other user..."? Too much work :P
	wxMessageBox("Request sent!\nNow waiting for other player to accept...", "Chess | SendRequest Alert");

	Player1.Name = Username1;
	Player2.Name = Username2;
	Player1.Colour = Colour == 0 ? LIGHT : DARK;
	Player2.Colour = Colour == 0 ? DARK : LIGHT;

	UIObj.StartTwoPlayerOnline();
	windowGame->Show(false);
	controlMenu->Enable(ID_TOUCH_PIECE, false);
	_beginthread(AcceptRequestCheck, 0, NULL);
}
void Account::AcceptRequestCheck(void*){
	acceptRequestCheckOn = true;
	bool flag = false;

	wxString Username1 = UIObj.NewTwoPlayerUserOnline->GetStringSelection();
	wxString Username2 = UIObj.OnlinePlayerToPlay->GetStringSelection();
	wxString POST = "uname1=" + Username1 + "&uname2=" + Username2;

	wxString reportLocation = "http://www.octpp.tk/users/" + Username1 + "/accepts/" + Username2 + ".txt";

	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	char* ptr;

	myHandle = curl_easy_init();
	for (int i = 0; i < 30; i++){
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(myHandle, CURLOPT_URL, wxStringToCharStar(reportLocation));
		result = curl_easy_perform(myHandle);

		// Convert CURLCode to char*
		ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		if (!strcmp(ptr, "YES")){
			wxMessageBox("Your request has been accepted by " + Username2 + "!", "Chess | Accept Request Check Alert");

			// Begin game
			sessionState = TWO_PLAYER_INTERNET_PLAYER_1;

			// Begin ping
			argListForPing.userName = Username1;
			argListForPing.otherUser = Username2;
			_beginthread(Ping, 0, (void*)&argListForPing);

			windowGame->Show(true);
			windowNewGameTwoPlayerOnline->Show(false);
			flag = true;

			if (Player1.Colour == DARK){
				argListForMoveCheck.UserChecking = Player1.Name;
				argListForMoveCheck.UserMaking = Player2.Name;
				_beginthread(Account::MoveCheck, 0, NULL);
			}
			break;
		}
		else if (!strcmp(ptr, "NO")){
			wxMessageBox("Your request has been turned down by " + Username2 + "  :(", "Chess | Accept Request Check Alert");

			isExit = false;
			delete windowGame;	// Delete the window created in SendRequest
			isExit = true;
			flag = true;
			break;
		}
		wxMilliSleep(100);
		delete ptr;
		readBuffer.clear();
	}

	if (!flag){
		// Delete the window created in SendRequest
		isExit = false;
		delete windowGame;
		isExit = true;
		wxMessageBox("Request timed out!", "Chess | Accept Request Check Alert");
	}

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/acceptRequestCheck/request_process.php");
	readBuffer.clear();
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	//TODO Error codes!!!!!!

	curl_easy_cleanup(myHandle);
}
void Account::CheckRequest(){
	wxString ColourData;
	wxBeginBusyCursor();

	// This function looks for a new request file.
	wxString Username = UIObj.NewTwoPlayerUserOnline->GetStringSelection();

	wxString POST = "uname=" + Username;

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	// Generating report file
	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/checkRequest/request_process.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_perform(myHandle);

	// Getting the report file
	wxString reportLocation = "http://www.octpp.tk/users/" + Username + "/requests/report.txt";
	const char* temp1 = (const char*)reportLocation.mb_str();
	char* location = (char*)temp1;

	curl_easy_setopt(myHandle, CURLOPT_URL, location);
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	wxEndBusyCursor();

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	if (!strcmp(ptr, "YES")){
		delete ptr;
		readBuffer.clear();

		// Get the request
		wxString reportLocation = "http://www.octpp.tk/users/" + Username + "/requests/request.txt";

		curl_easy_setopt(myHandle, CURLOPT_URL, wxStringToCharStar(reportLocation));

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);

		// Convert CURLCode to char*
		ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';
		ofstream ofile("request.txt");
		ofile.write(ptr, readBuffer.size());
		ofile.close();

		delete ptr;

		// Now we have the file. Read from the file. Display the contents.
		char fileData[100];
		wxString ExtendedText = "";
		wxString text = "You have a request from ";
		ifstream file("request.txt");
		file.getline(fileData, 100);
		wxString userOther = fileData;
		text += fileData;
		userOther.MakeUpper();
		Username.MakeUpper();
		ExtendedText += "Your colour is ";
		file.getline(fileData, 100);
		ColourData = fileData;
		if (!strcmp(fileData, "LIGHT")){
			ExtendedText += "DARK.";
		}
		else {
			ExtendedText += "LIGHT.";
		}
		file.getline(fileData, 100);

		ExtendedText += "\nMessage: ";
		ExtendedText += fileData;
		file.close();

		remove("request.txt");

		wxMessageDialog message(windowStart, text, "Chess | Confirm Request", wxOK | wxCANCEL | wxICON_NONE);
		message.SetOKCancelLabels("Accept", "Reject");
		message.SetExtendedMessage(ExtendedText);
		int returnValue = message.ShowModal();

		// Send form to acceptRequest
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/acceptRequest/request_process.php");
		POST.clear();
		POST += "uname1=" + Username + "&uname2=" + userOther + "&param=";
		if (returnValue == wxID_OK) { POST += "YES"; }
		else { POST += "NO"; }

		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_perform(myHandle);
		// Request file gets deleted online, BTW.
		curl_easy_cleanup(myHandle);

		// Remember to add error codes

		if (returnValue == wxID_CANCEL) {
			return;
		}
		else {
			sessionState = TWO_PLAYER_INTERNET_PLAYER_2;
			argListForPing.otherUser = userOther;
			argListForPing.userName = Username;
			// Start the Ping thread
			_beginthread(Ping, 0, (void*)&argListForPing);

			// Now move on to the game.
			Player1.Name = userOther;
			Player2.Name = Username;
			if (ColourData == "LIGHT"){	// Player 1 is light
				Player1.Colour = LIGHT;
				Player2.Colour = DARK;
			}
			else {
				Player1.Colour = DARK;
				Player2.Colour = LIGHT;
			}

			UIObj.StartTwoPlayerOnline();
			windowGame->Show(true);
			windowNewGameTwoPlayerOnline->Show(false);

			// Start checking for move by other player
			if (Player2.Colour == DARK){
				argListForMoveCheck.UserChecking = Player2.Name;
				argListForMoveCheck.UserMaking = Player1.Name;
				_beginthread(Account::MoveCheck, 0, NULL);
			}
		}
	}
	else {
		// No new request
		delete ptr;
		wxMessageBox("No new requests.");
	}
}
void Account::OnlineUsers(){
	wxBeginBusyCursor();

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/onlineUsers/index.php");
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_perform(myHandle);

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/users/onlineUsers.txt");
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	ofstream file("onlineUsers.txt", ios::out);
	file.write(ptr, readBuffer.size());
	file.close();

	curl_easy_cleanup(myHandle);

	wxEndBusyCursor();
}
void Account::Ping(void* arg) {
	// Retrieve the parameters.
	argsForPing* param = (argsForPing*)arg;
	wxString userToCheckPing = param->otherUser;
	wxString Username = param->userName;

	// Create the Post Fields
	wxString POST1 = "uname=" + Username + "&ping-digit=";
	POST1 << pingDigit;
	wxString reportLocation = "http://www.octpp.tk/users/" + userToCheckPing + "/playTemp/ping.txt";

	// Now other stuff we'll need.
	int error_count = 0;
	char* ptr = new char;

	// Set the flag to true
	isPingWorking = true;

	while ((sessionState == TWO_PLAYER_INTERNET_PLAYER_1 || sessionState == TWO_PLAYER_INTERNET_PLAYER_2) && error_count <= 7) {
		while (1) {
			delete ptr;

			// cURL code
			curl_global_init(CURL_GLOBAL_ALL);
			string readBuffer;
			CURL * myHandle;
			CURLcode result;
			myHandle = curl_easy_init();
			// Ping once
			curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/pingPut/ping_process.php");
			curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST1));
			curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
			result = curl_easy_perform(myHandle);

			// Convert cURL code to char*
			ptr = new char[readBuffer.size() + 1];
			copy(readBuffer.begin(), readBuffer.end(), ptr);
			ptr[readBuffer.size()] = '\0';

			// Error handling:
			if (!strcmp(ptr, "")) {
				error_count = 0;
				wxMilliSleep(3000);
				curl_easy_cleanup(myHandle);
				continue;
			}
			//TODO Check if Internet is still connected.

			else if (!strcmp(ptr, "0")) {
				wxMilliSleep(3000);
				curl_easy_cleanup(myHandle);
				continue;
			}
			else {
				pingDigit++;
				pingDigit %= 10;
				curl_easy_cleanup(myHandle);
				break;
			}
		}

		delete ptr;

		// Now check for other user's ping
		wxString URL = "http://www.octpp.tk/users/" + userToCheckPing + "/playTemp/ping.txt";

		// cURL code
		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();
		myHandle = curl_easy_init();
		curl_easy_setopt(myHandle, CURLOPT_URL, wxStringToCharStar(URL));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		readBuffer.clear();
		result = curl_easy_perform(myHandle);
		curl_easy_cleanup(myHandle);

		// Convert CURLCode to char*
		ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		wxString OtherPingDigit;
		OtherPingDigit << otherPingDigit;

		// Check what the report contained
		if (!strcmp(ptr, "")) {
			// Either server saturation, or user has logged out.
			error_count++;
			wxMilliSleep(1000);
		}
		else if (strcmp(ptr, wxStringToCharStar(OtherPingDigit))) {
			// User still online. Thank heavens.
			error_count = 0;
			otherPingDigit = atoi(ptr); //TODO Check if this is right!
		}
		else {
			// Other user MIGHT have pinged.
			error_count++;
			if (error_count == 5 || error_count == 6) {
				wxMessageBox("Looks like the other player has lost their connection. Click \"OK\" to try again.");
			}
			if (error_count > 6) {
				wxMessageBox("Connection lost. All your game are belong to us.");
				// Exit the Game
				RealGame.QuitGame();
			}
			wxMilliSleep(4000);
			delete ptr;
			continue;
		}
	}
}
void Account::MoveRegister(void*){
	wxString UserMaking = argListForMoveRegister.UserMaking;
	wxString UserReceiving = argListForMoveRegister.UserReceiving;
	wxString x1, y1, x2, y2;
	x1 << argListForMoveRegister.x1;
	y1 << argListForMoveRegister.y1;
	x2 << argListForMoveRegister.x2;
	y2 << argListForMoveRegister.y2;

	// Create the Post Fields
	wxString POST = "move-digit=";
	POST << moveDigit;
	POST += "&uname1=" + UserMaking + "&uname2=" + UserReceiving + "&x1=" + x1 + "&y1=" + y1 + "&x2=" + x2 + "&y2=" + y2;

	do {
		// cURL code
		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();

		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/moveRegister/move_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);

		// Convert CURLCode to char*
		char* ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);

		// Error codes
		if (!strcmp(ptr, "0")){
			wxMessageBox("Server Error: Could not open file for writing!", "Chess | MoveRegister Alert");
			delete ptr;
			wxMilliSleep(2000);
			continue;
		}
		else if (!strcmp(ptr, "")){
			delete ptr;
			wxMilliSleep(10000);
			continue;
		}
		else {
			delete ptr;
			break;
		}
	} while (true);

	// This updates the moveDigit.
	moveDigit++;
	moveDigit %= 10;

	_beginthread(Account::MoveCheck, 0, NULL);
}
void Account::MoveCheck(void*){
	wxString UserChecking = argListForMoveCheck.UserChecking, UserMaking = argListForMoveCheck.UserMaking;
	wxString URL = "http://www.octpp.tk/users/" + UserChecking + "/playTemp/move.txt";

	char *ptr = new char;
	while (1) {
		delete ptr;
		// cURL code
		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();

		curl_easy_setopt(myHandle, CURLOPT_URL, wxStringToCharStar(URL));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/4.0");
		curl_easy_setopt(myHandle, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(myHandle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(myHandle, CURLOPT_REFERER, "http://www.octpp.tk");
		curl_easy_setopt(myHandle, CURLOPT_CONNECTTIMEOUT, 5000);
		readBuffer.clear();
		result = curl_easy_perform(myHandle);
		curl_easy_cleanup(myHandle);

		// Convert CURLCode to char*
		ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		char x = otherMoveDigit + '0';

		// Error codes
		if (ptr[0] == '\0' || ptr[0] == '\n') {
			// Server Error: Saturation! Or somehthing else.
			wxMilliSleep(1000);
			continue;
		}
		else if (ptr[0] >= '0' && ptr[0] <= '9' && ptr[0] != x) {
			// Move has been made
			break;
		}
		else {
			// Move hasn't been made yet.
			wxMilliSleep(1000);
			continue;
		}
		wxMilliSleep(1000);
	}

	ofstream file("move.txt", ios::out);
	file.write(ptr, strlen(ptr));
	file.close();

	// Make the move by reading the file!
	ifstream ifile("move.txt", ios::in);
	int x1, x2, y1, y2;
	ifile >> otherMoveDigit >> x1 >> y1 >> x2 >> y2;
	ifile.close();

	RealGame.Control(x1, y1, 4);
	wxMilliSleep(400);
	RealGame.Control(x2, y2, 4);

	ofstream ofile("move.txt");
	ofile.close();

	if (promotionPane->promotion_state == true){
		_beginthread(Account::PromoteCheck, 0, NULL);
	}
}
void Account::PromoteRegister(void*){
	isPromoteRegisterRunning = true;
	wxString UserReceiving = argListForMoveRegister.UserReceiving;
	wxString Param;
	Param << PromoteRegisterParameter;

	wxString POST = "uname2=" + UserReceiving + "&param=" + Param;
	PromoteRegisterParameter = 0;

	while (true) {
		// cURL code
		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();

		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/paramRegister/move_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);
		curl_easy_cleanup(myHandle);

		// Convert CURLCode to char*
		char* ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		if (!strcmp(ptr, "")) {
			// Possible sevrer saturation
			delete ptr;
			wxMilliSleep(2000);
			continue;
		}
		else if (!strcmp(ptr, "0")) {
			// File couldn't be read on server
			delete ptr;
			wxMilliSleep(1000);
			continue;
		}
		else {
			// Success
			delete ptr;
			break;
		}
	}
	isPromoteRegisterRunning = false;
}
void Account::PromoteCheck(void*){
	wxString UserChecking = argListForMoveCheck.UserChecking;
	wxString Param;

	wxString reportLocation = "http://www.octpp.tk/users/" + UserChecking + "/playTemp/param.txt";
	wxString POST = "uname1=" + UserChecking;
	char* ptr = new char;
	while (true) {
		// cURL code
		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/paramUnregister/move_process.php");
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);
		curl_easy_cleanup(myHandle);
		// Convert CURLCode to char*
		ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		if (sessionState == NONE){
			delete ptr;
			return;
		}
		else if (!strcmp(ptr, "")) {
			// Server saturation
			wxMilliSleep(2000);
			delete ptr;
			continue;
		}
		else if (!strcmp(ptr, "0")){
			// File error on server
			wxMilliSleep(3000);
			delete ptr;
			continue;
		}
		else {
			delete ptr;
			break;
		}
	}
	wxString temp = ptr;
	long int param;
	temp.ToLong(&param, 10);

	RealGame.PawnPromotion(param);	// Do the pawn promotion
}
void Account::UpdateCheck(void*){
	wxMilliSleep(3000);
	if (internetConnected == false || serverConnected == false){
		return;
	}
	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/message.txt");
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	if (strcmp(ptr, "")){
		wxMessageBox(ptr, "Message from the developer");
	}

	delete ptr;

	readBuffer.clear();
	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/download/latestVersion.txt");
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	// Check and notify if there is an update
	if (strcmp(ptr, "202") > 0){
		wxMessageDialog message(windowStart, "A new version of OCTPP Chess 2015 is now available!\nDownload it now!", "Chess | New Version", wxOK | wxCANCEL | wxICON_NONE);
		message.SetOKCancelLabels("Download now!", "Later");
		int returnValue = message.ShowModal();

		if (returnValue == wxID_OK){
			ShellExecute(0, 0, L"http://www.octpp.tk/download", 0, 0, SW_SHOW);
			RealGame.Shutdown();
		}
	}
}
void Account::ForgotUsername(wxString &emai){
	wxString POST = "emai=" + emai;

	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/forgotUsername/forgotUsername_process.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	// Error Handling
	if (!strcmp(ptr, "5")) {
		wxMessageBox("Server Error: SQL error!", "Chess | Forgot Username Alert");
	}
	else if (!strcmp(ptr, "6")) {
		wxMessageBox("Server Error: SQL error!", "Chess | Forgot Username Alert");
	}
	else if (!strcmp(ptr, "7")) {
		wxMessageBox("Unregistered email ID!", "Chess | Forgot Username Alert");
	}
	else if (!strcmp(ptr, "9")) {
		wxMessageBox("Email sent!", "Chess | Forgot Username Alert");
	}
	curl_easy_cleanup(myHandle);
	delete ptr;
}
void Account::ForgotPassword(wxString &uname){
	wxString username = uname;
	username = username.MakeUpper();
	wxString POST = "uname=" + username;

	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/forgotPassword/forgotPassword_process.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	// Error Handling
	if (!strcmp(ptr, "5")) {
		wxMessageBox("Server Error: Could not connect to database!", "Chess | Forgot Password Alert");
	}
	else if (!strcmp(ptr, "6")) {
		wxMessageBox("Server Error: SQL error!", "Chess | Forgot Password Alert");
	}
	else if (!strcmp(ptr, "7")) {
		wxMessageBox("Wrong username!", "Chess | Forgot Password Alert");
	}
	else if (!strcmp(ptr, "9")) {
		wxMessageBox("Email sent!", "Chess | Forgot Password Alert");
	}
	curl_easy_cleanup(myHandle);
	delete ptr;
}
void Account::ChangePassword(wxString &newPwd){
	wxString CurrentUser = UIObj.AccountUsers->GetStringSelection();
	wxString POST = "uname=" + CurrentUser + "&newpwd=" + newPwd;

	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/changePassword/changePassword_form.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	// Error Handling
	if (!strcmp(ptr, "5")) {
		wxMessageBox("Server Error: Could not connect to database!", "Chess | Change Password Alert");
	}
	else if (!strcmp(ptr, "6")) {
		wxMessageBox("Server Error: SQL error!", "Chess | Change Password Alert");
	}
	else if (!strcmp(ptr, "7")) {
		wxMessageBox("Wrong username!", "Chess | Change Password Alert");
	}
	else if (!strcmp(ptr, "9")) {
		wxMessageBox("Password changed successfully!", "Chess | Change Password Alert");
	}
	curl_easy_cleanup(myHandle);
	delete ptr;
}
void Account::DeleteAccount(wxString &emai){
	wxString POST = "emai=" + emai;

	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/newUnregisterUser/verify.php");
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	// Error Handling
	if (!strcmp(ptr, "5")) {
		wxMessageBox("Server Error: Could not connect to database!", "Chess | Delete Account Alert");
	}
	else if (!strcmp(ptr, "6")) {
		wxMessageBox("Server Error: SQL error!", "Chess | Delete Account Alert");
	}
	else if (!strcmp(ptr, "7")) {
		wxMessageBox("Unregistered email ID!", "Chess | Delete Account Alert");
	}
	else if (!strcmp(ptr, "9")) {
		wxMessageBox("Account deleted.\nYES. Really.\n\nWe are sorry to see you go. Hope you enjoyed playing!", "Chess | Delete Account Alert");
	}
	curl_easy_cleanup(myHandle);
	delete ptr;
}
void Account::StatUpdate(void* arg){
	argsForStatUpdate* param = (argsForStatUpdate*)arg;

	// cURL code
	curl_global_init(CURL_GLOBAL_ALL);
	string readBuffer;
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	if (param->mode == MODE_STAT_UPDATE_WIN){
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/recordWin/record_process.php");
	}
	else if (param->mode == MODE_STAT_UPDATE_LOSE){
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/recordLoss/record_process.php");
	}
	else if (param->mode == MODE_STAT_UPDATE_DRAW){
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/recordDraw/record_process.php");
	}

	wxString POST = "uname=" + param->username;
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
	result = curl_easy_perform(myHandle);

	// Convert CURLCode to char*
	char* ptr = new char[readBuffer.size() + 1];
	copy(readBuffer.begin(), readBuffer.end(), ptr);
	ptr[readBuffer.size()] = '\0';

	//TODO Error codes

	curl_easy_cleanup(myHandle);
	delete ptr;
}
void Account::UploadGame(void* parameters) {
	// Upload game over here.
	argsForUploadFile *arg = (argsForUploadFile*)parameters;

	wxString fileName = arg->fileName;
	wxString userName = arg->userName;
	int mode = arg->mode;

	wxString POST = "uname=" + userName + "&filee=" + fileName + ".txt";

	if (mode & COMPLETED == COMPLETED) {
		// cURL code
		POST += "&mode=completedGames&contents=";
		// Grab contents of log file and post them too.
		//Encryption("Log.txt");	// Decrypt
		ifstream ifile("Log.txt", ios::in);
		while (!ifile.eof()){
			char ch;
			ifile.get(ch);
			POST << ch;
		}
		ifile.close();
		//Encryption("Log.txt");	// Encrypt

		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/UserAddFile/add_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);
		// Convert CURLCode to char*
		char* ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);
	}
	if (mode & SAVE == SAVE) {
		POST += "&mode=savedGames&contents=";
		// Grab contents of log file and POST it.
		//Encryption("Log.txt");	// Decrypt
		ifstream ifile("Log.txt", ios::in);
		while (!ifile.eof()){
			char ch;
			ifile.get(ch);
			POST << ch;
		}
		ifile.close();
		//Encryption("Log.txt");	// Encrypt

		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/UserAddFile/add_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);
		// Convert CURLCode to char*
		char* ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);
	}
	if (mode & DELETE_SAVE == DELETE_SAVE) {
		// Delete game from SavedGames folder
		curl_global_init(CURL_GLOBAL_ALL);
		string readBuffer;
		CURL * myHandle;
		CURLcode result;
		myHandle = curl_easy_init();
		curl_easy_setopt(myHandle, CURLOPT_URL, "http://www.octpp.tk/automata/UserDeleteFile/delete_process.php");
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, wxStringToCharStar(POST));
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
		result = curl_easy_perform(myHandle);
		// Convert CURLCode to char*
		char* ptr = new char[readBuffer.size() + 1];
		copy(readBuffer.begin(), readBuffer.end(), ptr);
		ptr[readBuffer.size()] = '\0';

		curl_easy_cleanup(myHandle);
	}
}

/*UI Functions*/
void UI::Splash(){
	windowSplash = new SplashScreen("Chess | SplashScreen");
	windowSplash->SetIcon(icon);
	wxImage Splash("Images/SplashScreen.jpg", wxBITMAP_TYPE_JPEG);
	wxButton *Button = new wxButton(windowSplash, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(Button, 1, wxEXPAND);

	windowSplash->Maximize(true);

	int w, h;
	windowSplash->GetSize(&w, &h);

	Splash = Splash.Scale(w, h, wxIMAGE_QUALITY_HIGH);
	Button->SetImageLabel(Splash);

	windowStart->Maximize(true);
	//windowSplash->ShowFullScreen(true, wxFULLSCREEN_NOBORDER);
	windowSplash->Show(false);
}
void UI::Startup(){
	windowStart = new Window_UI("Chess | Home");
	windowStart->SetIcon(icon);

	StatusBarStart = new wxStatusBar(windowStart);
	StatusBarStart->SetFieldsCount(3);
	StatusBarStart->SetStatusText("Welcome to OCTPP Chess 2015", 0);
	windowStart->SetStatusBar(StatusBarStart);

	wxBoxSizer* Start = new wxBoxSizer(wxHORIZONTAL);

	windowStart->SetBackgroundColour(wxColour(240, 240, 240));
	windowStart->ClearBackground();

	panelStart = new wxPanel(windowStart);

	NewGameButton = new wxButton(panelStart, ID_NEW, "");

	LoadGameButton = new wxButton(panelStart, ID_LOAD, "");

	MyAccountButton = new wxButton(panelStart, ID_ACCOUNT, "");

	LoginButton = new wxButton(panelStart, ID_LOGIN, "");

	LogoutButton = new wxButton(panelStart, ID_LOGOUT, "");

	NewAccountButton = new wxButton(panelStart, ID_CREATE_ACCOUNT, "");

	Start->Add(NewGameButton, 1, wxALIGN_CENTER | wxEXPAND);
	Start->Add(LoadGameButton, 1, wxALIGN_CENTER | wxEXPAND);
	Start->Add(MyAccountButton, 1, wxALIGN_CENTER | wxEXPAND);
	Start->Add(LoginButton, 1, wxALIGN_CENTER | wxEXPAND);
	Start->Add(LogoutButton, 1, wxALIGN_CENTER | wxEXPAND);
	Start->Add(NewAccountButton, 1, wxALIGN_CENTER | wxEXPAND);

	panelStart->SetSizer(Start);

	windowStart->Maximize(true);
	//windowStart->ShowFullScreen(true, wxFULLSCREEN_NOBORDER);
	windowStart->Show(true);
}
void UI::NewGame(){
	windowNewGame = new Window_UI("Chess | New Game");
	windowNewGame->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowNewGame);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | New Game", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowNewGame->SetStatusBar(StatusBarUI);

	windowNewGame->SetBackgroundColour(wxColour(249, 187, 41));
	windowNewGame->ClearBackground();

	panelNewGame = new wxPanel(windowNewGame);

	wxButton *OnePlayer = new wxButton(panelNewGame, ID_NEW_GAME_ONE_PLAYER, "1 Player");
	wxButton *TwoPlayer = new wxButton(panelNewGame, ID_NEW_GAME_TWO_PLAYER, "2 Player");
	wxButton *Home = new wxButton(panelNewGame, ID_HOME, "Home");
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	sizer->AddStretchSpacer(4);
	sizer->Add(OnePlayer, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(TwoPlayer, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelNewGame->SetSizer(sizer);

	windowNewGame->Maximize(true);
}
void UI::NewGameOnePlayer(){
	windowNewGameOnePlayer = new Window_UI("Chess | New Game | One Player");
	windowNewGameOnePlayer->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowNewGameOnePlayer);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | New Game One Player", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowNewGameOnePlayer->SetStatusBar(StatusBarUI);

	windowNewGameOnePlayer->SetBackgroundColour(wxColour(249, 187, 41));
	windowNewGameOnePlayer->ClearBackground();

	panelNewGameOnePlayer = new wxPanel(windowNewGameOnePlayer);

	Encryption("LoggedInPlayers.txt"); // Decrypt
	Encryption("LoggedInPlayersTemp.txt"); // Decrypt

	wxArrayString list;
	list.Add("GUEST");
	char temp[100];
	ifstream file("LoggedInPlayers.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	int count = list.GetCount();
	list.RemoveAt(count - 1);

	file.open("LoggedInPlayersTemp.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	count = list.GetCount();
	list.RemoveAt(count - 1);

	Encryption("LoggedInPlayers.txt"); // Encrypt
	Encryption("LoggedInPlayersTemp.txt"); // Encrypt

	wxStaticText *LoggedInUsers_label = new wxStaticText(panelNewGameOnePlayer, wxID_ANY, "Select User:");
	NewOnePlayerUsers = new wxChoice(panelNewGameOnePlayer, wxID_ANY, wxDefaultPosition, wxDefaultSize, list);
	NewOnePlayerUsers->SetSelection(0);
	wxString MyColour_choices[] = { "Light", "Dark" };
	MyColour = new wxRadioBox(panelNewGameOnePlayer, wxID_ANY, "My Colour", wxDefaultPosition, wxDefaultSize, 2, MyColour_choices);
	MyColour->SetBackgroundColour(wxColour(249, 187, 41));
	MyColour->SetForegroundColour(wxColour(0, 0, 0));

	wxString AILevel_choices[] = { "Piece of cake", "Easy", "Medium" };
	AILevel = new wxRadioBox(panelNewGameOnePlayer, wxID_ANY, "AI Level", wxDefaultPosition, wxDefaultSize, 2, AILevel_choices);
	AILevel->SetBackgroundColour(wxColour(249, 187, 41));
	AILevel->SetForegroundColour(wxColour(0, 0, 0));

	wxButton *StartOnePlayer = new wxButton(panelNewGameOnePlayer, ID_START_ONE_PLAYER, "Start Game!");
	wxButton *Home = new wxButton(panelNewGameOnePlayer, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_LoggedInUsers = new wxBoxSizer(wxHORIZONTAL);

	sizer_LoggedInUsers->Add(LoggedInUsers_label, 0, wxALIGN_CENTER | wxALL, 10);
	sizer_LoggedInUsers->Add(NewOnePlayerUsers, 0, wxALIGN_CENTER | wxALL, 10);

	sizer->AddStretchSpacer(4);
	sizer->Add(sizer_LoggedInUsers, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(MyColour, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(AILevel, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(StartOnePlayer, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelNewGameOnePlayer->SetSizer(sizer);

	windowNewGameOnePlayer->Maximize(true);
}
void UI::NewGameTwoPlayer(){
	windowNewGameTwoPlayer = new Window_UI("Chess | New Game | Two Player");
	windowNewGameTwoPlayer->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowNewGameTwoPlayer);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | New Game Two Player", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowNewGameTwoPlayer->SetStatusBar(StatusBarUI);

	windowNewGameTwoPlayer->SetBackgroundColour(wxColour(249, 187, 41));
	windowNewGameTwoPlayer->ClearBackground();

	panelNewGameTwoPlayer = new wxPanel(windowNewGameTwoPlayer);

	wxButton *Local = new wxButton(panelNewGameTwoPlayer, ID_NEW_GAME_TWO_PLAYER_LOCAL, "Local Game");
	wxButton *Online = new wxButton(panelNewGameTwoPlayer, ID_NEW_GAME_TWO_PLAYER_ONLINE, "Online Game");
	wxButton *Home = new wxButton(panelNewGameTwoPlayer, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	sizer->AddStretchSpacer(4);
	sizer->Add(Local, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Online, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelNewGameTwoPlayer->SetSizer(sizer);

	windowNewGameTwoPlayer->Maximize(true);
}
void UI::NewGameTwoPlayerLocal(){
	windowNewGameTwoPlayerLocal = new Window_UI("Chess | New Game | Two Player | Local");
	windowNewGameTwoPlayerLocal->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowNewGameTwoPlayerLocal);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | New Game One Player Local", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowNewGameTwoPlayerLocal->SetStatusBar(StatusBarUI);

	windowNewGameTwoPlayerLocal->SetBackgroundColour(wxColour(249, 187, 41));
	windowNewGameTwoPlayerLocal->ClearBackground();

	panelNewGameTwoPlayerLocal = new wxPanel(windowNewGameTwoPlayerLocal);

	Encryption("LoggedInPlayers.txt"); // Encrypt
	Encryption("LoggedInPlayersTemp.txt"); // Encrypt

	wxArrayString list;
	list.Add("GUEST");
	char temp[100];
	ifstream file("LoggedInPlayers.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	int count = list.GetCount();
	list.RemoveAt(count - 1);

	file.open("LoggedInPlayersTemp.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	count = list.GetCount();
	list.RemoveAt(count - 1);

	Encryption("LoggedInPlayers.txt"); // Decrypt
	Encryption("LoggedInPlayersTemp.txt"); // Decrypt

	wxStaticText *Player1_label = new wxStaticText(panelNewGameTwoPlayerLocal, wxID_ANY, "Select Player 1:");
	wxStaticText *Player2_label = new wxStaticText(panelNewGameTwoPlayerLocal, wxID_ANY, "Select Player 2:");

	NewTwoPlayerUser1 = new wxChoice(panelNewGameTwoPlayerLocal, ID_CHOICE_PLAYER1_NAME, wxDefaultPosition, wxDefaultSize, list);
	NewTwoPlayerUser2 = new wxChoice(panelNewGameTwoPlayerLocal, ID_CHOICE_PLAYER2_NAME, wxDefaultPosition, wxDefaultSize, list);
	NewTwoPlayerUser1->SetSelection(0);
	NewTwoPlayerUser2->SetSelection(0);

	wxString Colour_choices[] = { "Light", "Dark" };
	Player1Colour = new wxRadioBox(panelNewGameTwoPlayerLocal, ID_RADIO_PLAYER1_COLOUR, "Player 1 Colour", wxDefaultPosition, wxDefaultSize, 2, Colour_choices);
	Player1Colour->SetBackgroundColour(wxColour(249, 187, 41));
	Player1Colour->SetForegroundColour(wxColour(0, 0, 0));
	Player2Colour = new wxRadioBox(panelNewGameTwoPlayerLocal, ID_RADIO_PLAYER2_COLOUR, "Player 2 Colour", wxDefaultPosition, wxDefaultSize, 2, Colour_choices);
	Player2Colour->SetBackgroundColour(wxColour(249, 187, 41));
	Player2Colour->SetForegroundColour(wxColour(0, 0, 0));
	Player2Colour->SetSelection(1);

	StartTwoPlayer = new wxButton(panelNewGameTwoPlayerLocal, ID_START_TWO_PLAYER_LOCAL, "Start Game!");
	wxButton *Home = new wxButton(panelNewGameTwoPlayerLocal, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	Local = new wxBoxSizer(wxVERTICAL);
	Online = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *Players = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *Colours = new wxBoxSizer(wxHORIZONTAL);

	Players->Add(Player1_label, 0, wxALIGN_CENTER);
	Players->Add(NewTwoPlayerUser1, 0, wxALIGN_CENTER | wxALL, 10);
	Players->Add(Player2_label, 0, wxALIGN_CENTER);
	Players->Add(NewTwoPlayerUser2, 0, wxALIGN_CENTER | wxALL, 10);

	Colours->Add(Player1Colour, 0, wxALIGN_CENTER | wxALL, 10);
	Colours->Add(Player2Colour, 0, wxALIGN_CENTER | wxALL, 10);

	Local->Add(Players, 0, wxALIGN_CENTER | wxALL, 10);
	Local->Add(Colours, 0, wxALIGN_CENTER | wxALL, 10);

	sizer->AddStretchSpacer(4);
	sizer->Add(Players, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Colours, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(StartTwoPlayer, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->AddStretchSpacer(5);

	panelNewGameTwoPlayerLocal->SetSizer(sizer);

	windowNewGameTwoPlayerLocal->Maximize(true);
}
void UI::NewGameTwoPlayerOnline(){
	windowNewGameTwoPlayerOnline = new Window_UI("Chess | New Game | Two Player | Online");
	windowNewGameTwoPlayerOnline->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowNewGameTwoPlayerOnline);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | New Game One Player Online", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowNewGameTwoPlayerOnline->SetStatusBar(StatusBarUI);

	windowNewGameTwoPlayerOnline->SetBackgroundColour(wxColour(249, 187, 41));
	windowNewGameTwoPlayerOnline->ClearBackground();

	panelNewGameTwoPlayerOnline = new wxPanel(windowNewGameTwoPlayerOnline);

	Encryption("LoggedInPlayers.txt"); // Decrypt
	Encryption("LoggedInPlayersTemp.txt"); // Decrypt

	int totalUsers = 0;

	wxArrayString list;
	char temp[100];
	ifstream file("LoggedInPlayers.txt", ios::in);
	while (!file.eof()){
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	int count = list.GetCount();
	if (count > 0){
		list.RemoveAt(count - 1);
	}
	count = list.GetCount();
	totalUsers += count;

	file.open("LoggedInPlayersTemp.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	count = list.GetCount();
	if (count > 0){
		list.RemoveAt(count - 1);
	}
	count = list.GetCount();
	totalUsers += count;

	Encryption("LoggedInPlayers.txt"); // Encrypt
	Encryption("LoggedInPlayersTemp.txt"); // Encrypt

	if (totalUsers == 0){
		wxMessageBox("You have to log in to play an online game!", "Chess | Two Player Online Alert");
		isExit = false;
		delete windowNewGameTwoPlayerOnline;
		windowNewGameTwoPlayerOnline = NULL;
		isExit = true;
		return;
	}

	wxStaticText *PlayerOnline_label = new wxStaticText(panelNewGameTwoPlayerOnline, wxID_ANY, "Select player:");
	wxStaticText *OnlinePlayerToPlay_label = new wxStaticText(panelNewGameTwoPlayerOnline, wxID_ANY, "Select opponent:");
	wxStaticText *Message_label = new wxStaticText(panelNewGameTwoPlayerOnline, wxID_ANY, "Message");

	NewTwoPlayerUserOnline = new wxChoice(panelNewGameTwoPlayerOnline, wxID_ANY, wxDefaultPosition, wxDefaultSize, list);
	if (list.GetCount() > 0){
		NewTwoPlayerUserOnline->SetSelection(0);
	}

	list.Clear();
	AccountObj.OnlineUsers();

	// Remove users from same computer
	Encryption("LoggedInPlayers.txt");	// Decrypt
	Encryption("LoggedInPlayersTemp.txt");	// Decrypt

	char temp1[100], temp2[100];
	ifstream ifile1, ifile2;
	ofstream ofile;
	ofile.open("temp.txt", ios::out);
	ifile1.open("onlineUsers.txt", ios::in);

	while (!ifile1.eof()){
		bool flag = false;
		ifile1.getline(temp1, 100);

		ifile2.open("LoggedInPlayers.txt", ios::in);
		while (!ifile2.eof()){
			ifile2.getline(temp2, 100, '~');
			if (!strcmp(temp2, temp1)){
				flag = true;
				break;
			}
		}
		ifile2.close();

		if (flag){
			continue;
		}
		else {
			ofile << temp1 << "\n";
		}

	}
	ifile1.close();
	ofile.close();
	remove("onlineUsers.txt");
	rename("temp.txt", "onlineUsers.txt");

	ofile.open("temp.txt", ios::out);
	ifile1.open("onlineUsers.txt", ios::in);

	while (!ifile1.eof()){
		bool flag = false;
		ifile1.getline(temp1, 100);

		ifile2.open("LoggedInPlayersTemp.txt", ios::in);
		while (!ifile2.eof()){
			ifile2.getline(temp2, 100, '~');
			if (!strcmp(temp2, temp1)){
				flag = true;
				break;
			}
		}
		ifile2.close();

		if (flag){
			continue;
		}
		else {
			ofile << temp1 << "\n";
		}

	}
	ifile1.close();
	ofile.close();
	remove("onlineUsers.txt");
	rename("temp.txt", "onlineUsers.txt");

	Encryption("LoggedInPlayers.txt");	// Encrypt
	Encryption("LoggedInPlayersTemp.txt");	// Encrypt

	file.open("onlineUsers.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100);
		list.Add(temp);
	}
	file.close();
	count = list.GetCount();
	if (count > 0){
		list.RemoveAt(count - 1);
	}

	remove("onlineUsers.txt");

	count = list.GetCount();
	if (count == 0){
		wxMessageBox("No other users are online!", "Chess | Two Player Online Alert");
		isExit = false;
		delete windowNewGameTwoPlayerOnline;
		isExit = true;
		windowNewGameTwoPlayerOnline = NULL;
		return;
	}

	OnlinePlayerToPlay = new wxChoice(panelNewGameTwoPlayerOnline, wxID_ANY, wxDefaultPosition, wxDefaultSize, list);
	OnlinePlayerToPlay->SetSelection(0);

	wxString Colour_choices[] = { "Light", "Dark" };
	PlayerOnlineColour = new wxRadioBox(panelNewGameTwoPlayerOnline, wxID_ANY, "Your colour", wxDefaultPosition, wxDefaultSize, 2, Colour_choices);
	PlayerOnlineColour->SetBackgroundColour(wxColour(249, 187, 41));
	PlayerOnlineColour->SetForegroundColour(wxColour(0, 0, 0));

	wxButton *SendRequest = new wxButton(panelNewGameTwoPlayerOnline, ID_SEND_REQUEST, "Send Request");
	wxButton *CheckRequest = new wxButton(panelNewGameTwoPlayerOnline, ID_CHECK_REQUEST, "Check for requests");
	wxButton *Home = new wxButton(panelNewGameTwoPlayerOnline, ID_HOME, "Home");

	Message = new wxTextCtrl(panelNewGameTwoPlayerOnline, wxID_ANY, "Let\'s Play!");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *OnlineUsername = new wxBoxSizer(wxHORIZONTAL);
	OnlineUsername->Add(PlayerOnline_label, 0, wxALIGN_CENTER | wxALL, 10);
	OnlineUsername->Add(NewTwoPlayerUserOnline, 0, wxALIGN_CENTER | wxALL, 10);

	wxBoxSizer *OnlineOpponent = new wxBoxSizer(wxHORIZONTAL);
	OnlineOpponent->Add(OnlinePlayerToPlay_label, 0, wxALIGN_CENTER | wxALL, 10);
	OnlineOpponent->Add(OnlinePlayerToPlay, 0, wxALIGN_CENTER | wxALL, 10);

	sizer->AddStretchSpacer(4);
	sizer->Add(OnlineUsername, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(CheckRequest, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(PlayerOnlineColour, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(OnlineOpponent, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Message_label, 0, wxALIGN_CENTER | wxALL);
	sizer->Add(Message, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(SendRequest, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->AddStretchSpacer(5);

	panelNewGameTwoPlayerOnline->SetSizer(sizer);

	windowNewGameTwoPlayerOnline->Maximize(true);
	windowNewGameTwoPlayerOnline->Show(false);
}
void UI::CreateAccount(){
	windowCreateAccount = new Window_UI("Chess | Create Account");
	windowCreateAccount->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowCreateAccount);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | Create Account", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowCreateAccount->SetStatusBar(StatusBarUI);

	windowCreateAccount->SetBackgroundColour(wxColour(255, 178, 125));
	windowCreateAccount->ClearBackground();

	panelCreateAccount = new wxPanel(windowCreateAccount);

	wxStaticText *Username_label = new wxStaticText(panelCreateAccount, wxID_ANY, "Username");
	wxStaticText *Password_label = new wxStaticText(panelCreateAccount, wxID_ANY, "Password");
	wxStaticText *ConfirmPassword_label = new wxStaticText(panelCreateAccount, wxID_ANY, "Confirm Password");
	wxStaticText *Email_label = new wxStaticText(panelCreateAccount, wxID_ANY, "Email ID");

	CreateAccount_Username_field = new wxTextCtrl(panelCreateAccount, ID_CREATE_ACCOUNT_TEXT, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	CreateAccount_Password_field = new wxTextCtrl(panelCreateAccount, ID_CREATE_ACCOUNT_TEXT, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);
	CreateAccount_ConfirmPassword_field = new wxTextCtrl(panelCreateAccount, ID_CREATE_ACCOUNT_TEXT, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);
	CreateAccount_Email_field = new wxTextCtrl(panelCreateAccount, ID_CREATE_ACCOUNT_TEXT, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

	wxButton *CreateAccount = new wxButton(panelCreateAccount, ID_CREATE_ACCOUNT_ACTION, "Create Account");
	wxButton *Home = new wxButton(panelCreateAccount, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_Username = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_Password = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_ConfirmPassword = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_Email = new wxBoxSizer(wxHORIZONTAL);

	sizer_Username->Add(Username_label, 1, wxALIGN_CENTER | wxALL, 10);
	sizer_Username->Add(CreateAccount_Username_field, 1, wxALIGN_CENTER | wxALL, 10);

	sizer_Password->Add(Password_label, 1, wxALIGN_CENTER | wxALL, 10);
	sizer_Password->Add(CreateAccount_Password_field, 1, wxALIGN_CENTER | wxALL, 10);

	sizer_ConfirmPassword->Add(ConfirmPassword_label, 1, wxALIGN_CENTER | wxALL, 10);
	sizer_ConfirmPassword->Add(CreateAccount_ConfirmPassword_field, 1, wxALIGN_CENTER | wxALL, 10);

	sizer_Email->Add(Email_label, 1, wxALIGN_CENTER | wxALL, 10);
	sizer_Email->Add(CreateAccount_Email_field, 1, wxALIGN_CENTER | wxALL, 10);

	sizer->AddStretchSpacer(4);
	sizer->Add(sizer_Username, 0, wxALIGN_CENTER);
	sizer->Add(sizer_Password, 0, wxALIGN_CENTER);
	sizer->Add(sizer_ConfirmPassword, 0, wxALIGN_CENTER);
	sizer->Add(sizer_Email, 0, wxALIGN_CENTER);
	sizer->Add(CreateAccount, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelCreateAccount->SetSizer(sizer);

	windowCreateAccount->Maximize(true);
}
void UI::Login(){
	windowLogin = new Window_UI("Chess | Login");
	windowLogin->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowLogin);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | Login", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowLogin->SetStatusBar(StatusBarUI);

	windowLogin->SetBackgroundColour(wxColour(246, 125, 37));
	windowLogin->ClearBackground();

	panelLogin = new wxPanel(windowLogin);

	wxStaticText *Username_label = new wxStaticText(panelLogin, wxID_ANY, "Username");
	wxStaticText *Password_label = new wxStaticText(panelLogin, wxID_ANY, "Password");

	Login_Username_field = new wxTextCtrl(panelLogin, ID_LOGIN_TEXT, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	Login_Password_field = new wxTextCtrl(panelLogin, ID_LOGIN_TEXT, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);

	KeepLoggedIn = new wxCheckBox(panelLogin, wxID_ANY, "Keep me logged in");
	KeepLoggedIn->SetValue(true);
	wxButton *Login = new wxButton(panelLogin, ID_LOGIN_ACTION, "Login");
	wxButton *forgotUsername = new wxButton(panelLogin, ID_FORGOT_USERNAME, "Forgot username");
	wxButton *forgotPassword = new wxButton(panelLogin, ID_FORGOT_PASSWORD, "Forgot password");
	wxButton *Home = new wxButton(panelLogin, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_Username = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_Password = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_Buttons = new wxBoxSizer(wxHORIZONTAL);

	sizer_Username->Add(Username_label, 1, wxALIGN_CENTER);
	sizer_Username->Add(Login_Username_field, 1, wxALIGN_CENTER);

	sizer_Password->Add(Password_label, 1, wxALIGN_CENTER);
	sizer_Password->Add(Login_Password_field, 1, wxALIGN_CENTER);

	sizer_Buttons->Add(forgotUsername, 1, wxALIGN_CENTER | wxALL, 5);
	sizer_Buttons->Add(forgotPassword, 1, wxALIGN_CENTER | wxALL, 5);

	sizer->AddStretchSpacer(4);
	sizer->Add(sizer_Username, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(sizer_Password, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(KeepLoggedIn, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Login, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(sizer_Buttons, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelLogin->SetSizer(sizer);

	windowLogin->Maximize(true);
}
void UI::Logout(){
	windowLogout = new Window_UI("Chess | Logout");
	windowLogout->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowLogout);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | Logout", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowLogout->SetStatusBar(StatusBarUI);

	windowLogout->SetBackgroundColour(wxColour(250, 154, 93));
	windowLogout->ClearBackground();

	panelLogout = new wxPanel(windowLogout);

	Encryption("LoggedInPlayers.txt"); // Decrypt
	Encryption("LoggedInPlayersTemp.txt"); // Decrypt
	wxArrayString list;
	char temp[100];
	ifstream file("LoggedInPlayers.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	int count = list.GetCount();
	list.RemoveAt(count - 1);

	//Temp users
	file.open("LoggedInPlayersTemp.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	count = list.GetCount();
	list.RemoveAt(count - 1);
	Encryption("LoggedInPlayers.txt"); // Encrypt
	Encryption("LoggedInPlayersTemp.txt"); // Encrypt

	if (count == 1){
		wxMessageBox("No one is logged in!", "Chess Alert");
		isExit = false;
		delete windowLogout;
		windowLogout = NULL;
		isExit = true;
		return;
	}

	wxStaticText *Username_label = new wxStaticText(panelLogout, wxID_ANY, "Username");
	LogoutUsers = new wxChoice(panelLogout, wxID_ANY, wxDefaultPosition, wxDefaultSize, list);
	if (list.GetCount() > 0){
		LogoutUsers->SetSelection(0);
	}

	wxButton *Logout = new wxButton(panelLogout, ID_LOGOUT_ACTION, "Logout");
	wxButton *Home = new wxButton(panelLogout, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_Username = new wxBoxSizer(wxHORIZONTAL);

	sizer_Username->Add(Username_label, 1, wxALIGN_CENTER | wxALL, 10);
	sizer_Username->Add(LogoutUsers, 1, wxALIGN_CENTER | wxALL, 10);

	sizer->AddStretchSpacer(4);
	sizer->Add(sizer_Username, 0, wxALIGN_CENTER);
	sizer->Add(Logout, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelLogout->SetSizer(sizer);

	windowLogout->Maximize(true);
}
void UI::Account(){
	windowAccount = new Window_UI("Chess | My Account");
	windowAccount->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowAccount);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | My Account", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowAccount->SetStatusBar(StatusBarUI);

	windowAccount->SetBackgroundColour(wxColour(250, 157, 28));
	windowAccount->ClearBackground();

	panelAccount = new wxPanel(windowAccount);

	Encryption("LoggedInPlayers.txt"); // Decrypt
	Encryption("LoggedInPlayersTemp.txt"); // Decrypt
	wxArrayString list;
	char temp[100];
	ifstream file("LoggedInPlayers.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	int count = list.GetCount();
	list.RemoveAt(count - 1);

	// Temp users
	file.open("LoggedInPlayersTemp.txt", ios::in);
	while (!file.eof()) {
		file.getline(temp, 100, '~');
		list.Add(temp);
	}
	file.close();
	count = list.GetCount();
	list.RemoveAt(count - 1);
	Encryption("LoggedInPlayers.txt"); // Encrypt
	Encryption("LoggedInPlayersTemp.txt"); // Encrypt

	if (count == 1){
		wxMessageBox("No one is logged in!", "Chess Alert");
		isExit = false;
		delete windowAccount;
		windowAccount = NULL;
		isExit = true;
		return;
	}

	wxStaticText *Username_label = new wxStaticText(panelAccount, wxID_ANY, "Username");
	AccountUsers = new wxChoice(panelAccount, wxID_ANY, wxDefaultPosition, wxDefaultSize, list);
	if (list.GetCount() > 0){
		AccountUsers->SetSelection(0);
	}

	wxButton *changePassword = new wxButton(panelAccount, ID_CHANGE_PASSWORD, "Change password");
	//wxButton *changeColour = new wxButton(panelAccount, ID_CHANGE_COLOUR, "Change my colour");
	wxButton *Home = new wxButton(panelAccount, ID_HOME, "Home");
	wxButton *Stats = new wxButton(panelAccount, ID_STATS, "View Statistics");
	wxButton* DeleteAccountButton = new wxButton(panelAccount, ID_DELETE_ACCOUNT, "Delete my account");

	//GameTable_Number = new wxListBox(panelAccount, ID_GAME_TABLE);
	//GameTable_Name = new wxListBox(panelAccount, ID_GAME_TABLE);
	//GameTable_Player = new wxListBox(panelAccount, ID_GAME_TABLE);
	//GameTable_Date = new wxListBox(panelAccount, ID_GAME_TABLE);

	//GameTable_Number->Append("SERIAL NUMBER");
	//GameTable_Name->Append("GAME NAME");
	//GameTable_Player->Append("OPPONENT NAME");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_Users = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_Change = new wxBoxSizer(wxHORIZONTAL);
	//wxBoxSizer *sizer_GameTable = new wxBoxSizer(wxHORIZONTAL);

	sizer_Users->Add(Username_label, 0, wxALIGN_CENTER | wxALL, 10);
	sizer_Users->Add(AccountUsers, 0, wxALIGN_CENTER | wxALL, 10);

	//sizer_GameTable->Add(GameTable_Number, 0, wxALIGN_CENTER);
	//sizer_GameTable->Add(GameTable_Name, 0, wxALIGN_CENTER);
	//sizer_GameTable->Add(GameTable_Player, 0, wxALIGN_CENTER);

	//sizer_Change->Add(changeColour, 0, wxALIGN_CENTER | wxALL, 10);
	sizer_Change->Add(changePassword, 0, wxALIGN_CENTER | wxALL, 10);

	sizer->AddStretchSpacer(4);
	sizer->Add(sizer_Users, 0, wxALIGN_CENTER | wxALL, 10);
	//sizer->Add(sizer_GameTable, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Stats, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(sizer_Change, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(DeleteAccountButton, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelAccount->SetSizer(sizer);

	windowAccount->Maximize(true);
	windowAccount->Show(false);
}
void UI::Load(){
	windowLoad = new Window_UI("Chess | My Account");
	windowLoad->SetIcon(icon);

	StatusBarUI = new wxStatusBar(windowLoad);
	StatusBarUI->SetFieldsCount(3);
	StatusBarUI->SetStatusText("OCTPP Chess 2015 | Load Game", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarUI->SetStatusText(status, 2);
	windowLoad->SetStatusBar(StatusBarUI);

	windowLoad->SetBackgroundColour(wxColour(253, 175, 23));
	windowLoad->ClearBackground();

	panelLoad = new wxPanel(windowLoad);

	wxButton *Home = new wxButton(panelLoad, ID_HOME, "Home");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_Username = new wxBoxSizer(wxHORIZONTAL);

	sizer->AddStretchSpacer(4);
	//sizer->Add(Logout, 0, wxALIGN_CENTER | wxALL, 10);
	sizer->Add(Home, 0, wxALIGN_CENTER | wxALL, 30);
	sizer->AddStretchSpacer(5);

	panelLoad->SetSizer(sizer);

	windowLoad->Maximize(true);
	windowLoad->Show(true);
}

void UI::StartOnePlayer(){
	sessionState = ONE_PLAYER;
	windowGame = new Window_Game("Chess");
	windowGame->SetIcon(icon);

	StatusBarGame = new wxStatusBar(windowGame, wxID_ANY, wxSTB_DEFAULT_STYLE);
	StatusBarGame->SetFieldsCount(3);
	StatusBarGame->SetStatusText("Play against the computer!", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarGame->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarGame->SetStatusText(status, 2);
	windowGame->SetStatusBar(StatusBarGame);

	RealGame.Initialize();
	board_rotation = false;

	drawPane = new DrawPane(windowGame);
	killedPane = new KilledPane(windowGame);
	promotionPane = new PromotionPane(windowGame);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *main_left = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *main_right = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *TopPane = new wxBoxSizer(wxHORIZONTAL);

	wxTextCtrl *PlayerOne = new wxTextCtrl(windowGame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY | wxTE_RICH);
	wxTextCtrl *PlayerTwo = new wxTextCtrl(windowGame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY | wxTE_RICH);
	InfoBar = new wxTextCtrl(windowGame, wxID_ANY, "V/S", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY);

	if (Player1.Colour == LIGHT){
		PlayerOne->SetDefaultStyle(wxTextAttr(colour_light));
		PlayerTwo->SetDefaultStyle(wxTextAttr(colour_dark));
	}
	else {
		PlayerOne->SetDefaultStyle(wxTextAttr(colour_dark));
		PlayerTwo->SetDefaultStyle(wxTextAttr(colour_light));
	}

	PlayerOne->AppendText(Player1.Name);
	PlayerTwo->AppendText(Player2.Name);

	LogWindow = new wxTextCtrl(windowGame, wxID_ANY, "Game Log", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	wxButton* QuitGame = new wxButton(windowGame, ID_QUITGAME, "Quit Game");

	main_left->Add(killedPane, 18, wxEXPAND | wxALL, 0);

	main_right->Add(QuitGame, 0, wxALIGN_CENTER | wxEXPAND);
	main_right->Add(LogWindow, 1, wxEXPAND);

	TopPane->Add(PlayerTwo, 1, wxALIGN_CENTER | wxEXPAND);
	TopPane->Add(InfoBar, 0, wxALIGN_CENTER);
	TopPane->Add(PlayerOne, 1, wxALIGN_CENTER | wxEXPAND);

	main->Add(main_left, 2, wxEXPAND);
	main->Add(promotionPane, 2, wxEXPAND);
	main->Add(drawPane, 32, wxEXPAND);
	main->Add(main_right, 1, wxEXPAND);

	sizer->Add(TopPane, 0, wxALIGN_CENTER | wxEXPAND);
	sizer->Add(main, 1, wxALIGN_CENTER | wxEXPAND);

	windowGame->SetSizer(sizer);

	//windowGame->ShowFullScreen(true, wxFULLSCREEN_NOBORDER);
	windowGame->Maximize(true);
	windowGame->Show(false);
}
void UI::StartTwoPlayerLocal(){
	sessionState = TWO_PLAYER_LOCAL;
	windowGame = new Window_Game("Chess");
	windowGame->SetIcon(icon);

	StatusBarGame = new wxStatusBar(windowGame, wxID_ANY, wxSTB_DEFAULT_STYLE);
	StatusBarGame->SetFieldsCount(3);
	StatusBarGame->SetStatusText("Play!", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarGame->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarGame->SetStatusText(status, 2);
	windowGame->SetStatusBar(StatusBarGame);

	RealGame.Initialize();
	board_rotation = true;

	drawPane = new DrawPane(windowGame);
	killedPane = new KilledPane(windowGame);
	promotionPane = new PromotionPane(windowGame);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* main_left = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* main_right = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* TopPane = new wxBoxSizer(wxHORIZONTAL);

	wxTextCtrl *PlayerOne = new wxTextCtrl(windowGame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY | wxTE_RICH);
	wxTextCtrl *PlayerTwo = new wxTextCtrl(windowGame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY | wxTE_RICH);
	InfoBar = new wxTextCtrl(windowGame, wxID_ANY, "V/S", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY);

	if (Player1.Colour == LIGHT){
		PlayerOne->SetDefaultStyle(wxTextAttr(colour_light));
		PlayerTwo->SetDefaultStyle(wxTextAttr(colour_dark));
	}
	else {
		PlayerOne->SetDefaultStyle(wxTextAttr(colour_dark));
		PlayerTwo->SetDefaultStyle(wxTextAttr(colour_light));
	}

	PlayerOne->AppendText(Player1.Name);
	PlayerTwo->AppendText(Player2.Name);

	LogWindow = new wxTextCtrl(windowGame, wxID_ANY, "Game Log", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	wxButton* QuitGame = new wxButton(windowGame, ID_QUITGAME, "Quit Game");

	main_left->Add(killedPane, 18, wxEXPAND);

	main_right->Add(QuitGame, 0, wxALIGN_CENTER | wxEXPAND);
	main_right->Add(LogWindow, 1, wxEXPAND);

	TopPane->Add(PlayerTwo, 1, wxALIGN_CENTER | wxEXPAND);
	TopPane->Add(InfoBar, 0, wxALIGN_CENTER);
	TopPane->Add(PlayerOne, 1, wxALIGN_CENTER | wxEXPAND);

	main->Add(main_left, 1, wxEXPAND);
	main->Add(promotionPane, 1, wxEXPAND);
	main->Add(drawPane, 16, wxEXPAND);
	main->Add(main_right, 1, wxEXPAND);

	sizer->Add(TopPane, 0, wxALIGN_CENTER | wxEXPAND);
	sizer->Add(main, 1, wxALIGN_CENTER | wxEXPAND);

	windowGame->SetSizer(sizer);

	//windowGame->ShowFullScreen(true, wxFULLSCREEN_NOBORDER);
	windowGame->Maximize(true);
	windowGame->Show(false);
}
void UI::StartTwoPlayerOnline(){
	// Session state has already been set in other functions
	windowGame = new Window_Game("Chess");
	windowGame->SetIcon(icon);

	StatusBarGame = new wxStatusBar(windowGame, wxID_ANY, wxSTB_DEFAULT_STYLE);
	StatusBarGame->SetFieldsCount(3);
	StatusBarGame->SetStatusText("Play online!", 0);
	wxString status = "Internet: ";
	status += internetConnected == true ? "Connected" : "Not connected";
	StatusBarGame->SetStatusText(status, 1);
	status = "Server: ";
	status += serverConnected == true ? "Connected" : "Not connected";
	StatusBarGame->SetStatusText(status, 2);
	windowGame->SetStatusBar(StatusBarGame);

	RealGame.Initialize();
	board_rotation = false;

	drawPane = new DrawPane(windowGame);
	killedPane = new KilledPane(windowGame);
	promotionPane = new PromotionPane(windowGame);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* main_left = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* main_right = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* TopPane = new wxBoxSizer(wxHORIZONTAL);

	wxTextCtrl *PlayerOne = new wxTextCtrl(windowGame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY | wxTE_RICH);
	wxTextCtrl *PlayerTwo = new wxTextCtrl(windowGame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY | wxTE_RICH);
	InfoBar = new wxTextCtrl(windowGame, wxID_ANY, "V/S", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_READONLY);

	if (Player1.Colour == LIGHT){
		PlayerOne->SetDefaultStyle(wxTextAttr(colour_light));
		PlayerTwo->SetDefaultStyle(wxTextAttr(colour_dark));
	}
	else {
		PlayerOne->SetDefaultStyle(wxTextAttr(colour_dark));
		PlayerTwo->SetDefaultStyle(wxTextAttr(colour_light));
	}

	PlayerOne->AppendText(Player1.Name);
	PlayerTwo->AppendText(Player2.Name);

	LogWindow = new wxTextCtrl(windowGame, wxID_ANY, "Game Log", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	wxButton* QuitGame = new wxButton(windowGame, ID_QUITGAME, "Quit Game");

	main_left->Add(killedPane, 18, wxEXPAND);

	main_right->Add(QuitGame, 0, wxALIGN_CENTER | wxEXPAND);
	main_right->Add(LogWindow, 1, wxEXPAND);

	TopPane->Add(PlayerTwo, 1, wxALIGN_CENTER | wxEXPAND);
	TopPane->Add(InfoBar, 0, wxALIGN_CENTER);
	TopPane->Add(PlayerOne, 1, wxALIGN_CENTER | wxEXPAND);

	main->Add(main_left, 1, wxEXPAND);
	main->Add(promotionPane, 1, wxEXPAND);
	main->Add(drawPane, 16, wxEXPAND);
	main->Add(main_right, 1, wxEXPAND);

	sizer->Add(TopPane, 0, wxALIGN_CENTER | wxEXPAND);
	sizer->Add(main, 1, wxALIGN_CENTER | wxEXPAND);

	windowGame->SetSizer(sizer);

	//windowGame->ShowFullScreen(true, wxFULLSCREEN_NOBORDER);
	windowGame->Maximize(true);
	windowGame->Show(false);
}