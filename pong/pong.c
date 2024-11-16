#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 80
#define BALL_SIZE 15
#define BALL_SPEED 5
#define WIN_SCORE 10
#define MAX_BALL_SPEED 15

typedef struct {
    int x, y;
    int width, height;
} Paddle;

typedef struct {
    int x, y;
    int size;
    int speedX, speedY;
} Ball;

enum GameState {
    MENU,
    PLAYING,
    WIN_SCREEN
};

// Global variables
Paddle leftPaddle, rightPaddle;
Ball ball;
int scoreLeft = 0, scoreRight = 0;
BOOL gameRunning = FALSE;
enum GameState gameState = MENU;
BOOL useMouseControl = TRUE;

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeGame(void);
void UpdateGame(void);
void DrawGame(HWND hwnd);
void DrawMenu(HWND hwnd);
void DrawWinScreen(HWND hwnd);
void ResetBall(void);
void ResetGame(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Pong Window Class";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassW(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Pong Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    InitializeGame();
    SetTimer(hwnd, 1, 16, NULL);  // ~60 FPS

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        switch (gameState) {
        case MENU:
            DrawMenu(hwnd);
            break;
        case PLAYING:
            DrawGame(hwnd);
            break;
        case WIN_SCREEN:
            DrawWinScreen(hwnd);
            break;
        }
        return 0;

    case WM_TIMER:
        if (gameState == PLAYING && gameRunning) {
            UpdateGame();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_MOUSEMOVE:
        if (gameState == PLAYING && useMouseControl) {
            int mouseY = HIWORD(lParam);
            rightPaddle.y = mouseY - PADDLE_HEIGHT / 2;
            rightPaddle.y = max(0, min(rightPaddle.y, WINDOW_HEIGHT - PADDLE_HEIGHT));
        }
        return 0;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP:
            if (gameState == PLAYING && !useMouseControl)
                rightPaddle.y -= 10;
            break;
        case VK_DOWN:
            if (gameState == PLAYING && !useMouseControl)
                rightPaddle.y += 10;
            break;
        case 'K':
            if (gameState == MENU) {
                useMouseControl = FALSE;
                gameState = PLAYING;
                ResetGame();
            }
            break;
        case 'M':
            if (gameState == MENU) {
                useMouseControl = TRUE;
                gameState = PLAYING;
                ResetGame();
            }
            break;
        case 'P':
            if (gameState == PLAYING)
                gameRunning = !gameRunning;
            break;
        case VK_ESCAPE:
            if (gameState == PLAYING)
                gameState = MENU;
            else if (gameState == WIN_SCREEN)
                gameState = MENU;
            break;
        case 'R':
            if (gameState == WIN_SCREEN) {
                gameState = PLAYING;
                ResetGame();
            }
            break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void InitializeGame(void) {
    srand((unsigned int)time(NULL));

    leftPaddle.x = 50;
    leftPaddle.y = WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    leftPaddle.width = PADDLE_WIDTH;
    leftPaddle.height = PADDLE_HEIGHT;

    rightPaddle.x = WINDOW_WIDTH - 50 - PADDLE_WIDTH;
    rightPaddle.y = WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    rightPaddle.width = PADDLE_WIDTH;
    rightPaddle.height = PADDLE_HEIGHT;

    ResetBall();
}

void ResetBall(void) {
    ball.x = WINDOW_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = WINDOW_HEIGHT / 2 - BALL_SIZE / 2;
    ball.size = BALL_SIZE;
    ball.speedX = (rand() % 2 == 0 ? 1 : -1) * BALL_SPEED;
    ball.speedY = (rand() % 2 == 0 ? 1 : -1) * BALL_SPEED;
}

void ResetGame(void) {
    scoreLeft = 0;
    scoreRight = 0;
    InitializeGame();
    gameRunning = TRUE;
}

void UpdateGame(void) {
    // Store previous position
    int prevX = ball.x;
    int prevY = ball.y;

    // Update ball position
    ball.x += ball.speedX;
    ball.y += ball.speedY;

    // Ball collision with top and bottom walls
    if (ball.y <= 0 || ball.y + ball.size >= WINDOW_HEIGHT) {
        ball.speedY = -ball.speedY;
        ball.y = (ball.y <= 0) ? 0 : WINDOW_HEIGHT - ball.size;
    }

    // Ball collision with paddles
    if ((ball.x <= leftPaddle.x + leftPaddle.width &&
        prevX > leftPaddle.x + leftPaddle.width &&
        ball.y + ball.size >= leftPaddle.y &&
        ball.y <= leftPaddle.y + leftPaddle.height) ||
        (ball.x + ball.size >= rightPaddle.x &&
            prevX + ball.size < rightPaddle.x &&
            ball.y + ball.size >= rightPaddle.y &&
            ball.y <= rightPaddle.y + rightPaddle.height)) {

        // Reverse horizontal direction
        ball.speedX = -ball.speedX;

        // Adjust vertical speed based on where the ball hit the paddle
        Paddle* hitPaddle = (ball.x < WINDOW_WIDTH / 2) ? &leftPaddle : &rightPaddle;
        float relativeIntersectY = (hitPaddle->y + (hitPaddle->height / 2)) - (ball.y + (ball.size / 2));
        float normalizedRelativeIntersectionY = (relativeIntersectY / (hitPaddle->height / 2));
        ball.speedY = normalizedRelativeIntersectionY * MAX_BALL_SPEED;

        // Increase speed slightly
        if (abs(ball.speedX) < MAX_BALL_SPEED) {
            ball.speedX += (ball.speedX > 0) ? 1 : -1;
        }

        // Move ball outside paddle
        if (ball.x < WINDOW_WIDTH / 2) {
            ball.x = leftPaddle.x + leftPaddle.width + 1;
        }
        else {
            ball.x = rightPaddle.x - ball.size - 1;
        }
    }

    // Scoring and win condition
    if (ball.x <= 0) {
        scoreRight++;
        if (scoreRight >= WIN_SCORE) {
            gameState = WIN_SCREEN;
            gameRunning = FALSE;
        }
        else {
            ResetBall();
        }
    }
    else if (ball.x + ball.size >= WINDOW_WIDTH) {
        scoreLeft++;
        if (scoreLeft >= WIN_SCORE) {
            gameState = WIN_SCREEN;
            gameRunning = FALSE;
        }
        else {
            ResetBall();
        }
    }

    // AI for left paddle
    float paddleCenter = leftPaddle.y + (PADDLE_HEIGHT / 2);
    float ballCenter = ball.y + (ball.size / 2);
    if (ballCenter > paddleCenter + 10) {
        leftPaddle.y += 5;
    }
    else if (ballCenter < paddleCenter - 10) {
        leftPaddle.y -= 5;
    }
    leftPaddle.y = max(0, min(leftPaddle.y, WINDOW_HEIGHT - PADDLE_HEIGHT));
}

void DrawMenu(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    // Clear the buffer
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    FillRect(hdcBuffer, &rect, hBrush);
    DeleteObject(hBrush);

    // Draw menu title
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    SetBkMode(hdcBuffer, TRANSPARENT);
    HFONT hFont = CreateFont(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT hOldFont = SelectObject(hdcBuffer, hFont);

    RECT textRect = { 0, 50, WINDOW_WIDTH, 150 };
    DrawText(hdcBuffer, L"PONG", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(hFont);
    hFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    SelectObject(hdcBuffer, hFont);

    // Draw buttons
    int buttonWidth = 250;
    int buttonHeight = 50;
    int buttonX = (WINDOW_WIDTH - buttonWidth) / 2;
    int buttonY = 200;
    int buttonSpacing = 20;

    HBRUSH hButtonBrush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN hButtonPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    SelectObject(hdcBuffer, hButtonPen);
    SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));

    // Keyboard button
    RECT buttonRect;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    RoundRect(hdcBuffer, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom, 10, 10);
    DrawText(hdcBuffer, L"(K) KEYBOARD", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Mouse button
    buttonY += buttonHeight + buttonSpacing;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    RoundRect(hdcBuffer, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom, 10, 10);
    DrawText(hdcBuffer, L"(M) MOUSE", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Instructions
    buttonY += buttonHeight + buttonSpacing * 2;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    DrawText(hdcBuffer, L"P - PAUSE", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    buttonY += buttonHeight;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    DrawText(hdcBuffer, L"ESC - QUIT", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(hButtonBrush);
    DeleteObject(hButtonPen);

    SelectObject(hdcBuffer, hOldFont);
    DeleteObject(hFont);

    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);
    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcBuffer);
    EndPaint(hwnd, &ps);
}

void DrawGame(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Double buffering
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    // Clear the buffer
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    FillRect(hdcBuffer, &rect, hBrush);
    DeleteObject(hBrush);

    // Draw dotted line
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(255, 255, 255));
    HPEN hOldPen = SelectObject(hdcBuffer, hPen);
    MoveToEx(hdcBuffer, WINDOW_WIDTH / 2, 0, NULL);
    LineTo(hdcBuffer, WINDOW_WIDTH / 2, WINDOW_HEIGHT);
    SelectObject(hdcBuffer, hOldPen);
    DeleteObject(hPen);

    // Draw paddles
    hBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT leftPaddleRect = { leftPaddle.x, leftPaddle.y, leftPaddle.x + leftPaddle.width, leftPaddle.y + leftPaddle.height };
    RECT rightPaddleRect = { rightPaddle.x, rightPaddle.y, rightPaddle.x + rightPaddle.width, rightPaddle.y + rightPaddle.height };
    FillRect(hdcBuffer, &leftPaddleRect, hBrush);
    FillRect(hdcBuffer, &rightPaddleRect, hBrush);

    // Draw ball
    RECT ballRect = { ball.x, ball.y, ball.x + ball.size, ball.y + ball.size };
    FillRect(hdcBuffer, &ballRect, hBrush);
    DeleteObject(hBrush);

    // Draw scores
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    SetBkMode(hdcBuffer, TRANSPARENT);
    wchar_t scoreText[32];
    swprintf_s(scoreText, sizeof(scoreText) / sizeof(wchar_t), L"%d  %d", scoreLeft, scoreRight);
    HFONT hFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT hOldFont = SelectObject(hdcBuffer, hFont);
    SIZE textSize;
    GetTextExtentPoint32W(hdcBuffer, scoreText, (int)wcslen(scoreText), &textSize);
    TextOutW(hdcBuffer, (WINDOW_WIDTH - textSize.cx) / 2, 10, scoreText, (int)wcslen(scoreText));
    SelectObject(hdcBuffer, hOldFont);
    DeleteObject(hFont);

    // Copy the buffer to the screen
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);

    // Clean up
    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcBuffer);

    EndPaint(hwnd, &ps);
}

void DrawWinScreen(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    // Clear the buffer
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    FillRect(hdcBuffer, &rect, hBrush);
    DeleteObject(hBrush);

    // Draw dotted line
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(255, 255, 255));
    HPEN hOldPen = SelectObject(hdcBuffer, hPen);
    MoveToEx(hdcBuffer, WINDOW_WIDTH / 2, 0, NULL);
    LineTo(hdcBuffer, WINDOW_WIDTH / 2, WINDOW_HEIGHT);
    SelectObject(hdcBuffer, hOldPen);
    DeleteObject(hPen);

    // Draw win screen
    SetTextColor(hdcBuffer, RGB(255, 255, 255));
    SetBkMode(hdcBuffer, TRANSPARENT);
    HFONT hFont = CreateFont(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT hOldFont = SelectObject(hdcBuffer, hFont);

    wchar_t winText[64];
    swprintf_s(winText, sizeof(winText) / sizeof(wchar_t), L"%d  %d", scoreLeft, scoreRight);

    RECT textRect = { 0, 50, WINDOW_WIDTH, 150 };
    DrawText(hdcBuffer, winText, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Draw "WIN" text
    textRect.top = 150;
    textRect.bottom = 250;
    DrawText(hdcBuffer, L"WIN", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(hFont);
    hFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    SelectObject(hdcBuffer, hFont);

    // Draw buttons
    int buttonWidth = 250;
    int buttonHeight = 50;
    int buttonX = (WINDOW_WIDTH - buttonWidth) / 2;
    int buttonY = 300;
    int buttonSpacing = 20;

    HPEN hButtonPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    SelectObject(hdcBuffer, hButtonPen);
    SelectObject(hdcBuffer, GetStockObject(NULL_BRUSH));

    // Play Again button
    RECT buttonRect;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    RoundRect(hdcBuffer, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom, 10, 10);
    DrawText(hdcBuffer, L"(R) PLAY AGAIN", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Main Menu button
    buttonY += buttonHeight + buttonSpacing;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    RoundRect(hdcBuffer, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom, 10, 10);
    DrawText(hdcBuffer, L"(ESC) MAIN MENU", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Play Solitaire button
    buttonY += buttonHeight + buttonSpacing;
    SetRect(&buttonRect, buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    RoundRect(hdcBuffer, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom, 10, 10);
    DrawText(hdcBuffer, L"PLAY SOLITAIRE!", -1, &buttonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(hButtonPen);

    SelectObject(hdcBuffer, hOldFont);
    DeleteObject(hFont);

    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);
    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcBuffer);
    EndPaint(hwnd, &ps);
}