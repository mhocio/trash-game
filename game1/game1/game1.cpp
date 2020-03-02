// game1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <random>
#include "game1.h"
#include <time.h>
#include <list>

using namespace std;

#define MAX_LOADSTRING 100

HBITMAP backGrondBitmap = NULL;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

int seconds = 0;
int minutes = 0;

int backgroundTimer = 0;

struct Player {
	HWND player_hwnd;
	int x;
	int y;
	int h;
	int w;

	double velocity;
	bool droping; 
	bool jumping;
	bool tryJUMP_key;

	bool left_move;
	bool right_move;

	bool on_floor;
	bool standing;

	int score;
	int HighScore;
};

struct PlatformLeft {
	HWND platform_hwnd;
	int x;
	int y;
	int h;
	int w;
};

struct PlatformRight {
	HWND platform_hwnd;
	int x;
	int y;
	int h;
	int w;
};

struct CornerWindow {
	int bottom;
	int right;
};

CornerWindow cornerWindow;

static list < POINT > pointsList;

 int move_size = 7;
 double GRAVITY = 0.6;
 double GRAVITY_DOWN = 1;
 int acceleration = 15;
 int frameRate = 1000/60;

 int gameSpeed = 3;

 int bounce_acceleration = 2;

RECT rc;
Player player;

int const arrayPlatformSize = 6;
int const PlatformDistanse = 150;
int platformsGap = 75;
int platformsGapIncrease = 0;
PlatformLeft ArrayPlatformLeft[arrayPlatformSize];
PlatformRight ArrayPlatformRight[arrayPlatformSize];

bool GameBackgrounded = false;

RECT rec_new;
const int window_x = 416;
const int window_y = 659;

static HDC offDC = NULL;
static HDC hdc = NULL;
static HBITMAP offOldBitmap = NULL;
static HBITMAP offBitmap = NULL;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterPlayerClass(HINSTANCE hInstance);
ATOM				MyRegisterPlatformClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    NewGame_INT_PTR(HWND, UINT, WPARAM, LPARAM);

HWND hMainWnd;

void Move(void);
void DrawPlayer(void);
bool CheckCollision(RECT* rec_ret, bool* bottomCollision, bool* topCollision, bool* sideCollision);
void CreateNewGame(void);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAME1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	MyRegisterPlayerClass(hInstance);
	MyRegisterPlatformClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAME1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0,128,255)));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAME1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterPlayerClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 0, 0)));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_SMALL);
	wcex.lpszClassName = L"PlayerWnd";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM MyRegisterPlatformClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 0, 255)));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_SMALL);
	wcex.lpszClassName = L"PlatformWnd";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //window
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_EX_TOPMOST),
	   (GetSystemMetrics(SM_CXSCREEN) - window_x) / 2, (GetSystemMetrics(SM_CYSCREEN) - window_y) / 2, window_x,window_y, nullptr, nullptr, hInstance, nullptr);

	SetWindowPos(hWnd, HWND_TOPMOST, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

   if (!hWnd)
   {
      return FALSE;
   }

   //bitmaps
   backGrondBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   //endof window

   hMainWnd = hWnd;

   //corner MAIN window
   cornerWindow.bottom = 600;
   cornerWindow.right = 400;
   //endof corner

   CreateNewGame();
   //InvalidateRect(hWnd, NULL, TRUE);

   //endof Platform

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//rectangle info
	//RECT rc;
	//GetClientRect(hWnd, &rc);
	//endof rectangle info
    switch (message)
    {

	case WM_KILLFOCUS:
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) |
			WS_EX_LAYERED);
		// Make this window 50% alpha
		SetLayeredWindowAttributes(hWnd, 0, (255 * 50) / 100, LWA_ALPHA);
		GameBackgrounded = true;
	}break;

	case WM_SETFOCUS:
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) |
			WS_EX_LAYERED);
		// Make this window 100% alpha
		SetLayeredWindowAttributes(hWnd, 0, (255 * 100) / 100, LWA_ALPHA);
		GameBackgrounded = false;
		player.left_move = player.right_move = false;
	}break;

	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, NULL); //counting seconds
		SetTimer(hWnd, 2, frameRate, NULL); //60 Hz
		SetTimer(hWnd, 3, 1, NULL); //jump
		SetTimer(hWnd, 4, 1000/10, NULL); // BACK timer
		hdc = GetDC(hWnd);
		offDC = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
		break;

	case WM_TIMER:
		switch (wParam) {

		case 4:
		{
			backgroundTimer++;
			InvalidateRect(hWnd, NULL, TRUE);
			/*backgroundTimer++;
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hMainWnd);*/
		}break;

		case 1:
		{
			if (!GameBackgrounded)
			{
				seconds++;
				if (seconds == 60)
				{
					seconds = 0;
					minutes++;
				}
			}
		}break;

		case 2:
			if(!GameBackgrounded)
		{

				//backgroundTimer++;

			//collision
			RECT rec_ret;
			bool bottomCollision = false;
			bool topCollision = false;
			bool sideCollision = false;

			if (CheckCollision(&rec_ret, &bottomCollision, &topCollision, &sideCollision)
			&& !player.standing	) //player on platform or hit it
			{
				if (topCollision)
				{
					player.standing = true;
					player.jumping = false;
					player.droping = false;
				}
				if (bottomCollision)
				{
					if (player.jumping)
					{
						player.jumping = false;
						player.droping = true;
						player.standing = false;
						player.velocity = bounce_acceleration;
					}
					else if (player.standing)
					{
						//player.y = cornerWindow.bottom - ArrayPlatformLeft[arrayPlatformSize - 1].h + player.h;
					}
				}
			}
			else // player dropped off the platform
			{
				if (player.standing && !player.jumping && !player.droping && !player.on_floor)
				{
					player.standing = false;
					player.droping = true;
					player.jumping = false;
					player.velocity = 0;
				}
			}

			// check if player is on the floor
			if (player.y + player.h >= cornerWindow.bottom)
			{
				player.jumping = false;
					player.droping = false;
					player.standing = true;
					player.velocity = 0;
					player.y = cornerWindow.bottom - player.h;
					player.on_floor = true;

					if (player.score > 0) ///////// END GAME!!!!!! END GAME!!!!! END GAME!!!!! END GAME!!!!! END GAME!!!!! END GAME!!!!!
					{
						player.score = 0;
						platformsGap = 100;
					}
			}
			else
				player.on_floor = false;

			//JUMP key
			if ((!player.jumping) && (!player.droping) && player.standing && player.tryJUMP_key)
			{
				player.velocity = acceleration;
				player.jumping = true;
				player.droping = false;
				player.standing = false;
				player.tryJUMP_key = false; // TURNS ON ONE TIME JUMP PER PRESS
			}

			//jump UP
			if (player.jumping && !player.standing && !player.droping)
			{
				player.y -= player.velocity;
				player.velocity -= GRAVITY;

				if (player.velocity <= 0)
				{
					player.jumping = false;
					player.droping = true;
					player.velocity = 0;
				}
			}

			//jump DROP
			else if (player.droping && !player.standing && !player.jumping) 
			{
				player.velocity += GRAVITY_DOWN;
				player.y += player.velocity;

				if (player.y + player.h >= cornerWindow.bottom)
				{
					player.jumping = false;
					player.droping = false;
					player.standing = true;
					player.velocity = 0;
					player.y = cornerWindow.bottom - player.h;
					player.on_floor = true;
				}
			}


			if (player.right_move)
			{
				player.x += move_size;
				if (player.x + player.w > cornerWindow.right) 
						player.x = cornerWindow.right - player.w;
			}
			else if (player.left_move)
			{
				player.x -= move_size;
				if (player.x < 00) 
					player.x = 0;
			}
			for (int i = 0; i < arrayPlatformSize; i++)
			{
				ArrayPlatformLeft[i].y += gameSpeed;
				ArrayPlatformRight[i].y += gameSpeed;

				if (ArrayPlatformLeft[i].y >= cornerWindow.bottom) //NEXT LEVEL
				{
					ArrayPlatformLeft[i].y = -300;
					ArrayPlatformRight[i].y = -300;
					player.score++;
					platformsGap += platformsGapIncrease;

					ArrayPlatformLeft[i].w = 50 + rand() % 200;
					ArrayPlatformLeft[i].x = 0;

					ArrayPlatformRight[i].x = platformsGap + ArrayPlatformLeft[i].w;
					ArrayPlatformRight[i].w = cornerWindow.right - ArrayPlatformRight[i].x;

					//gameSpeed = 1000 / 60 - player.score * 5;

					if (player.HighScore < player.score)
						player.HighScore = player.score;
				}


			}
			
			player.y += gameSpeed;
			if (player.y + player.w > cornerWindow.bottom)
				player.y--;

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hMainWnd);
		}break;

	}
		break;

	case WM_KEYDOWN:
	{

		if (wParam == 38 || wParam == 32) // up arrow or space
		{
			player.tryJUMP_key = true;
		}
		if (wParam == 37) // left arrow
		{
			player.left_move = true;
		}
		if (wParam == 39) // right arrow
		{
			player.right_move = true;
		}
	}break;

	case WM_KEYUP:
	{
		if (wParam == 37) // left arrow
		{
			player.left_move = false;
		}
		if (wParam == 39) // right arrow
		{
			player.right_move = false;
		}

		if (wParam == 38 || wParam == 32) // up arrow or space
		{
			player.tryJUMP_key = false;
		}
	}break;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
			minMaxInfo -> ptMaxSize.x = minMaxInfo -> ptMaxTrackSize.x = window_x;
			minMaxInfo -> ptMaxSize.y = minMaxInfo -> ptMaxTrackSize.y = window_y;
			minMaxInfo->ptMinTrackSize.x = window_x;
			minMaxInfo->ptMinTrackSize.y = window_y;
			}
		break;

	case WM_SIZE:
	{
		int clientWidth = LOWORD(lParam);
		int clientHeight = HIWORD(lParam);
		hdc = GetDC(hWnd);
		if (offOldBitmap != NULL)
		{
			SelectObject(offDC, offOldBitmap);
		}
		if (offBitmap != -NULL)
		{
			DeleteObject(offBitmap);
		}
		offBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		offOldBitmap = (HBITMAP)SelectObject(offDC, offBitmap);
		ReleaseDC(hWnd, hdc);
	}break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_FILE_NEWGAME:
				DialogBox(hInst, MAKEINTRESOURCE(IDM_NEWGAME), hWnd, NewGame_INT_PTR);
				break;
			case ID_SPEED_SLOW:
				CheckMenuItem((HMENU)IDC_GAME1, ID_SPEED_FAST, MF_UNCHECKED);
				CheckMenuItem((HMENU)IDC_GAME1, ID_SPEED_MEDIUM, MF_UNCHECKED);
				CheckMenuItem((HMENU)IDC_GAME1, ID_SPEED_SLOW, MFS_CHECKED);
				gameSpeed = 1;
				break;
			case ID_SPEED_MEDIUM:
				gameSpeed = 2;
				break;
			case ID_SPEED_FAST:
				gameSpeed = 3;
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {



		//background
		/*HBITMAP oldBB = (HBITMAP)SelectObject(memDC, back);
		BitBlt(hdc, 0, 0, MainRect.right, MainRect.bottom, memDC, 0, -backgroundtimer, SRCCOPY);
		BitBlt(hdc, 0, 0, MainRect.right, MainRect.bottom, memDC, 0, -backgroundtimer + 1428, SRCCOPY);*/
		/// JANUSZ

		//BITMAP bm;

		//HDC hdcMem = CreateCompatibleDC(offDC);
		//SelectObject(hdcMem, backGrondBitmap);

		//GetObject(backGrondBitmap, sizeof(bm), &bm);

		//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

		////SelectObject(hdcMem, hbmOld);
		//DeleteDC(hdcMem);

		//EndPaint(hWnd, &ps);

		//////////////

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect(hWnd, &rc);



		HBRUSH oldBrush;


		///WORKING
		 oldBrush = (HBRUSH)SelectObject(offDC, (HBRUSH)GetStockObject(GRAY_BRUSH));
		Rectangle(offDC, 0, 0, rc.right, rc.bottom);
		SelectObject(offDC, (HBRUSH)GetStockObject(BLACK_BRUSH)); //background
		///WORKING

		//HBITMAP oldBB = (HBITMAP)SelectObject(offDC, backGrondBitmap);
		//BitBlt(hdc, 0, 0, rc.right, rc.bottom, offDC, 0, 0, SRCCOPY);
		//SelectObject(offDC, oldBB);
		//BitBlt(hdc, 0, 0, rc.right, rc.bottom, offDC, 0, -backgroundTimer, SRCCOPY);
		//////SelectObject(offDC, (HBRUSH)GetStockObject(BLACK_BRUSH)); //background
		//SelectObject(offDC, oldBB);


		//BITMAP bm;

		//HDC hdcMem = CreateCompatibleDC(hdc);
		//SelectObject(hdcMem, backGrondBitmap);
		////HBITMAP hbmOld = SelectObject(hdcMem, BITMAP);

		//GetObject(backGrondBitmap, sizeof(bm), &bm);

		//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, offDC, 0, 0, SRCCOPY);

		//SelectObject(hdcMem, hbmOld);
		//DeleteDC(hdcMem);

		//EndPaint(hWnd, &ps);

		//SetBkMode(hdc, TRANSPARENT);


		//
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
		 oldBrush = (HBRUSH)SelectObject(offDC, brush);

		for (int i = 0; i < arrayPlatformSize; i++)
		{
			Rectangle(offDC, ArrayPlatformLeft[i].x, ArrayPlatformLeft[i].y,
				ArrayPlatformLeft[i].x + ArrayPlatformLeft[i].w, ArrayPlatformLeft[i].y + ArrayPlatformLeft[i].h);

			Rectangle(offDC, ArrayPlatformRight[i].x, ArrayPlatformRight[i].y,
				ArrayPlatformRight[i].x + ArrayPlatformRight[i].w, ArrayPlatformRight[i].y + ArrayPlatformRight[i].h);
		}
			SelectObject(offDC, oldBrush);
			DeleteObject(brush);



			brush = CreateSolidBrush(RGB(255, 0, 0));
			oldBrush = (HBRUSH)SelectObject(offDC, brush);

			Rectangle(offDC, player.x, player.y, player.x + player.w, player.y + player.h);

			 SelectObject(offDC, oldBrush);
			 DeleteObject(brush);

			 TCHAR s[256];
			 if (seconds >= 10)
				_stprintf_s(s, 256, _T("Score: %d [%d:%d] Best: %d"), player.score, minutes, seconds, player.HighScore);
			 else
				 _stprintf_s(s, 256, _T("Score: %d [%d:0%d] Best: %d"), player.score, minutes, seconds, player.HighScore);
			 //SetWindowText(hMainWnd, s);

			HFONT font = CreateFont(
			- MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY)
			 , 7),// Height
			0, // Width
			 0, // Escapement
			 0, // Orientation
			 FW_BOLD, // Weight
			 false, // Italic
			 FALSE, // Underline
			 0, // StrikeOut
			 EASTEUROPE_CHARSET, // CharSet
			 OUT_DEFAULT_PRECIS, // OutPrecision
			 CLIP_DEFAULT_PRECIS, // ClipPrecision
			 DEFAULT_QUALITY, // Quality
			 DEFAULT_PITCH | FF_SWISS, //PitchAndFamily
			  _T(" Verdana ")); // Facename
		  HFONT oldFont = (HFONT)SelectObject(hdc, font);
		  SetBkMode(offDC, TRANSPARENT);
		  DrawText(offDC, s, (int)_tcslen(s), &rc, DT_CENTER | DT_NOCLIP | DT_TOP | DT_SINGLELINE);
		  SelectObject(offDC, oldFont);
		  DeleteObject(font);

		 BitBlt(hdc, 0, 0, rc.right, rc.bottom, offDC, 0, 0, SRCCOPY);
		 EndPaint(hWnd, &ps);

        }
        break;

	case WM_ERASEBKGND:
		 return 1;


    case WM_DESTROY:
		if (offOldBitmap != NULL)
			 SelectObject(offDC, offOldBitmap);
		 if (offDC != NULL) 
			 DeleteDC(offDC);
		 if (offBitmap != NULL)
			 DeleteObject(offBitmap);

        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK NewGame_INT_PTR(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			/*player.x = 180;
			player.y = cornerWindow.bottom - player.h;*/
			//// HERE SHOULD BE A NEW GAME
			CreateNewGame();
			return (INT_PTR)TRUE;
		}
		else
			EndDialog(hDlg, LOWORD(wParam));
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Move(void)
{

}

bool CheckCollision(RECT * lprcDst, bool *bottomCollision, bool *topCollision, bool *sideCollision)
{
	//collision
	//RECT lprcDst;
	RECT player_rect;
	player_rect.left = player.x;
	player_rect.top = player.y;
	player_rect.right = player.x + player.w;
	player_rect.bottom = player.y + player.h;

	RECT block_rect;

	for (int i = 0; i < arrayPlatformSize; i++)
	{
		block_rect.left = ArrayPlatformLeft[i].x;
		block_rect.top = ArrayPlatformLeft[i].y;
		block_rect.right = ArrayPlatformLeft[i].x + ArrayPlatformLeft[i].w;
		block_rect.bottom = ArrayPlatformLeft[i].y + ArrayPlatformLeft[i].h;

		bool if_intersect = IntersectRect(lprcDst, &player_rect, &block_rect);

		if (if_intersect)
		{
			if (lprcDst->bottom == player.y + player.h && !player.jumping)
			{
				*topCollision = true;
				player.y = ArrayPlatformLeft[i].y - player.h;
				//return true;
			}

			else if (lprcDst->top + ArrayPlatformLeft[i].h == player.y + player.h)
			{
				*bottomCollision = true;
				if (player.on_floor)
				{
					player.y = cornerWindow.bottom - player.h - ArrayPlatformLeft[i].h;
				}
				else
				{
					player.y = ArrayPlatformLeft[i].y + player.h;
				}
			}
			
			if (lprcDst->left == player.x)
			{
				//player.x = ArrayPlatformLeft[i].x + ArrayPlatformLeft[i].w;
				//return true;
			}

			return true;
		}

		block_rect.left = ArrayPlatformRight[i].x;
		block_rect.top = ArrayPlatformRight[i].y;
		block_rect.right = ArrayPlatformRight[i].x + ArrayPlatformRight[i].w;
		block_rect.bottom = ArrayPlatformRight[i].y + ArrayPlatformRight[i].h;

		if_intersect = IntersectRect(lprcDst, &player_rect, &block_rect);

		if (if_intersect)
		{
			if (lprcDst->bottom == player.y + player.h && !player.jumping)
			{
				*topCollision = true;
				player.y = ArrayPlatformRight[i].y - player.h;
				//return true;
			}

			else if (lprcDst->top + ArrayPlatformRight[i].h == player.y + player.h)
			{
				*bottomCollision = true;
				if (player.on_floor)
				{
					player.y = cornerWindow.bottom - player.h - ArrayPlatformRight[i].h;
				}
				else
				{
					player.y = ArrayPlatformRight[i].y + player.h;
				}
			}

			if (lprcDst->right == player.x)
			{
				//player.x -= move_size;
				//return true;
			}

			return true;
		}
	}

	return false;
}

void CreateNewGame(void)
{
	//player
	player.w = 32; player.h = 32;
	player.x = 180;
	player.y = cornerWindow.bottom - player.h;
	player.droping = player.jumping = player.left_move = player.right_move = false;
	player.on_floor = true;
	player.standing = true;
	//endof player

	srand(time(NULL));
	//platforms
	for (int i = 0; i < arrayPlatformSize; i++)
	{
		ArrayPlatformLeft[i].h = ArrayPlatformRight[i].h = 32;

		ArrayPlatformLeft[i].y = (i - 1) * PlatformDistanse;
		ArrayPlatformRight[i].y = (i - 1) * PlatformDistanse;

		ArrayPlatformLeft[i].w = 50 + rand() % 200;
		ArrayPlatformLeft[i].x = 0;

		ArrayPlatformRight[i].x = platformsGap + ArrayPlatformLeft[i].w;
		ArrayPlatformRight[i].w = cornerWindow.right - ArrayPlatformRight[i].x;
	}
	//endof platforms
	
	seconds = minutes = 0;

	platformsGap = 75;

	player.score = player.HighScore = 0;

}