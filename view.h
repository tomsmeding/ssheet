// Tom Smeding (s1685694), Tim Brouwer (s1663615), Ruben Turkenburg (s1659685)

#pragma once

#include <ncurses.h>
#include <string>
#include <vector>
#include <utility>
#include "spreadsheet.h"
#include "celladdress.h"
#include "maybe.h"

using namespace std;

/*
The View, the class that handles all the direct screen output and communicates
with ncurses.
*/

class SheetView{
public:
	SheetView(Spreadsheet &sheet);
	~SheetView();
	//redraws the cell at that address (probably a changed cell);
	//does nothing if outside screen
	void redrawCell(CellAddress addr);

	void redraw(bool full=false); //redraws entire (visible) screen
	int getChar(); //reads a character

	void setCursorPosition(CellAddress addr); //moves the cursor to that position

	CellAddress getCursorPosition(); //gets the cursor position

	//places an edit window (pop-up?) over the specified cell with the specified default
	//value, and returns the entered value. (Nothing if escape pressed)
	Maybe<string> getStringWithEditWindowOverCell(CellAddress loc,string defval);

	//asks a string of the user in the status bar, with
	//specified prompt and default (pre-filled) value
	Maybe<string> askStringOfUser(string prompt,string prefilled);

	void displayStatusString(string s); //displays s in the status bar

private:
	Spreadsheet &sheet;
	CellAddress scroll=CellAddress(0,0);
	CellAddress cursor=CellAddress(0,0);
	string cursordispval;

	void redrawCell(CellAddress addr,bool dorefresh);
	int rowToY(int row) const;
	int columnToX(int column) const;

	void drawBoxAround(int x,int y,int w,int h);

	Maybe<string> getTextBoxString(int wid,string buffer);
};
