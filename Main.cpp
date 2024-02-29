#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>

// Desired aspect ratio
const float ASPECT_RATIO = 16.0f / 9.0f;

// Window dimensions
const int WINDOW_WIDTH = 2560;
const int WINDOW_HEIGHT = static_cast<int>(WINDOW_WIDTH / ASPECT_RATIO);

// Player position
float playerX = 0.0f;
float playerY = 0.0f;
float playerWidth = 20.0f;
float playerHeight = 20.0f;

// Time variables
LARGE_INTEGER frequency;
LARGE_INTEGER lastTime;

// Movement flags
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;

// Fullscreen flag
bool fullscreen = false;

// Function to handle keyboard input
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            switch (wParam) {
                case 'A':
                    moveLeft = true;
                    break;
                case 'D':
                    moveRight = true;
                    break;
                case 'W':
                    moveUp = true;
                    break;
                case 'S':
                    moveDown = true;
                    break;
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
                case VK_F3:
                    // Toggle fullscreen
                    fullscreen = !fullscreen;
                    if (fullscreen) {
                        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
                    } else {
                        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
                        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                    }
                    break;
            }
            break;
        case WM_KEYUP:
            switch (wParam) {
                case 'A':
                    moveLeft = false;
                    break;
                case 'D':
                    moveRight = false;
                    break;
                case 'W':
                    moveUp = false;
                    break;
                case 'S':
                    moveDown = false;
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Function to initialize OpenGL
void initOpenGL(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    int format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, format, &pfd);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);
    ReleaseDC(hwnd, hdc);

    // Set viewport to QHD resolution (2560x1440)
    glViewport(0, 0, 2560, 1440);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 2560, 0, 1440); // Adjust projection matrix to QHD resolution
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Initialize time variables
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
}

// Function to draw the player
void drawPlayer() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the player's body
    glBegin(GL_QUADS);
    glVertex2f(playerX, playerY);
    glVertex2f(playerX + playerWidth, playerY);
    glVertex2f(playerX + playerWidth, playerY + playerHeight);
    glVertex2f(playerX, playerY + playerHeight);
    glEnd();

    // Draw the black smiley face
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for the smiley face
    glPointSize(3.0f); // Adjust the point size for drawing facial features
    glBegin(GL_POINTS);
    // Draw the eyes
    glVertex2f(playerX + playerWidth * 0.3f, playerY + playerHeight * 0.7f);
    glVertex2f(playerX + playerWidth * 0.7f, playerY + playerHeight * 0.7f);
    // Draw the mouth (a simple curve)
    for (int i = 0; i <= 180; ++i) {
        float angle = i * 3.14159f / 180.0f;
        float x = playerX + playerWidth * 0.5f + 5.0f * -cos(angle);
        float y = playerY + playerHeight * 0.4f + 5.0f * -sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

// Function to update the game logic
void updateGame(double elapsedTime) {
    // Update player position based on movement flags
    if (moveLeft)
        playerX -= 200.0f * elapsedTime;
    if (moveRight)
        playerX += 200.0f * elapsedTime;
    if (moveUp)
        playerY += 200.0f * elapsedTime;
    if (moveDown)
        playerY -= 200.0f * elapsedTime;
}

// Function to render the scene
void renderScene(HWND hwnd) {
    // Calculate elapsed time
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    double elapsedTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
    lastTime = currentTime;

    // Update game logic based on elapsed time
    updateGame(elapsedTime);

    HDC hdc = GetDC(hwnd);
    wglMakeCurrent(hdc, wglGetCurrentContext());

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawPlayer();

    SwapBuffers(hdc);
    ReleaseDC(hwnd, hdc);
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "PlatformerWindow";
    RegisterClassA(&wc);

    // Calculate window size to maintain aspect ratio
    RECT windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    int adjustedWidth = windowRect.right - windowRect.left;
    int adjustedHeight = windowRect.bottom - windowRect.top;

    // Center the window
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - adjustedWidth) / 2;
    int posY = (screenHeight - adjustedHeight) / 2;

    // Create window
    HWND hwnd = CreateWindowExA(
        0,
        "PlatformerWindow",
        "OpenGL Platformer",
        WS_OVERLAPPEDWINDOW,
        posX, posY, adjustedWidth, adjustedHeight,
        NULL, NULL, hInstance, NULL);

    // Show the window
    ShowWindow(hwnd, SW_SHOW);

    initOpenGL(hwnd);

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        renderScene(hwnd);
    }

    return 0;
}