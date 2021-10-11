#define _CRT_SECURE_NO_WARNNING 1
#include <stdio.h>
#include <windows.h> 
#include <time.h>

#define scount 80
#define screen_x 80
#define screen_y 25

HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
COORD star[scount];

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

void gotoxy(int x, int y) {
	COORD c = { x, y };
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setcolor(int fg, int bg) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

void drawship(int x, int y) {
	gotoxy(x, y);
	printf("<-0->");
}

void deleteship(int x, int y) {
	gotoxy(x, y);
	printf("     ");
}

void setcursor(bool visible) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

void clear_buffer() {
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 7;
		}
	}
}

void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos, &windowSize);
}

void init_star() {
	int x, y;
	for (int i = 0; i < scount; i++) {
		x = rand() % screen_x - 1;
		y = rand() % screen_y - 1;
		star[i] = { (short)x, (short)y };
	}
}

void drawstar() {
	for (int i = 0; i < scount; i++) {
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = '*';
		consoleBuffer[star[i].X + screen_x * star[i].Y].Attributes = 7;
	}
}

void star_fall()
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { (short)(rand() % screen_x),1 };
		}
		else {
			star[i] = { star[i].X, (short)(star[i].Y + 1) };
		}
	}
}

void fill_star_to_buffer() {
	int x, y;
	for (int i = 0; i < scount; i++) {
		x = star[i].X;
		y = star[i].Y;
		consoleBuffer[x + screen_x * y].Char.AsciiChar = '*';
		consoleBuffer[x + screen_x * y].Attributes = 0;
	}
}

void fill_data_to_buffer() {
	int r;
	char ch;
	int color;
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			r = rand() % 26 + 65;
			color = rand() % 256;
			ch = (char)r;
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ch;
			consoleBuffer[x + screen_x * y].Attributes = color;
		}
	}
}

void random() {
	int x;
	for (int i = 0; i < 10; i++) {
		x = rand() % 26 + 65;
		printf("%d = %c\n", x, x);
	}
}

int main() {
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	srand(time(NULL));
	setcursor(0);
	int posxtemp = 0;
	int posytemp = 0;
	int colorship = 7;
	init_star();
	drawstar();
	fill_buffer_to_console();
	int countendgame = 0;
	while (play) {
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT && 
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
						gotoxy(0, 0);
						printf("press : %c\n", eventBuffer[i].Event.KeyEvent.uChar.AsciiChar);
					}
					else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == 99 || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == 67) {
						deleteship(posxtemp, posytemp);
						colorship = 1 + rand() % 7;
						setcolor(colorship, 0);
						drawship(posxtemp, posytemp);
					}

				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {
						deleteship(posxtemp, posytemp);
						colorship = 1 + rand() % 7;
						setcolor(colorship, 0);
						drawship(posxtemp, posytemp);
					}
					else if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						RIGHTMOST_BUTTON_PRESSED) {
					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						deleteship(posxtemp, posytemp);
						setcolor(colorship, 0);
						drawship(posx, posy);
						posxtemp = posx;
						posytemp = posy;
						for (int i = 0; i < scount; i++) {
							if (posy == star[i].Y && (star[i].X - posx <= 4 && star[i].X - posx >= 0)) {
								countendgame += 1;
								star[i].X = rand() % 80;
								star[i].Y = rand() % 25;
								clear_buffer();
								drawstar();
								fill_buffer_to_console();
								if (countendgame == 10) {
									play = false;
								}
							}

						}
					}
				}
			}
			delete[] eventBuffer;
		}
		Sleep(100);
		setcolor(7, 0);
	}
	return 0;
}

