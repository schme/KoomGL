#include "win_platform.h"
#include "game.h"

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include "GL/glew.h"
#include "GL/wglew.h"

#include <wingdi.h>

#if ENABLE_CONSOLE
#include <io.h>
#include <fcntl.h>
#endif


const u64 memoryStackSize = 50*1024*1024;


static HDC DeviceContext;
static HGLRC RenderingContext;
static WINDOWPLACEMENT windowPlacement = { sizeof( windowPlacement) };
static b32 globalPlaying;
static i64 perfCountFrequency;
static u64 frame;

struct Win_WindowDimensions
{
    i32 width;
    i32 height;
};


static inline u32
SafeTruncateU32( u64 i)
{
    assert( i <= 0xffffffff);
    return (u32)i;
}


// DEBUG ONLY
void
FreeMemory( void *memory) {
    if( memory) {
        free( memory);
    }
}


/* Using malloc instead of VirtualAlloc because malloc
 * is better suited for small allocations
 */
// DEBUG ONLY
void*
ReadFile(const char *filename, u32* content_size)
{
    void *result = 0;
    HANDLE fileHandle = CreateFileA(
        filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );

    LARGE_INTEGER fileSize;
    if( GetFileSizeEx( fileHandle, &fileSize)) {

        u32 fileSize32 = SafeTruncateU32( fileSize.QuadPart);
        result = malloc(fileSize32);
        if( result) {
            DWORD bytesRead;
            if( ReadFile( fileHandle, result, fileSize32, &bytesRead, 0) &&
                (fileSize32 == bytesRead))
            {
                *content_size = fileSize32;
                printf("file %s read successfully\n", filename);

            } else {
                FreeMemory( result);
                result = 0;
                *content_size = 0;
            }
        }
        CloseHandle( fileHandle);
    }

    return result;
}


void
Win_ToggleFullscreen( HWND hwnd)
{
    DWORD dwStyle = GetWindowLong( hwnd, GWL_STYLE);
    if( dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if( GetWindowPlacement( hwnd, &windowPlacement) &&
            GetMonitorInfo( MonitorFromWindow( hwnd,
                    MONITOR_DEFAULTTOPRIMARY), &mi))
            {
            SetWindowLong( hwnd, GWL_STYLE,
                    dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos( hwnd, HWND_TOP,
                   mi.rcMonitor.left, mi.rcMonitor.top,
                   mi.rcMonitor.right - mi.rcMonitor.left,
                   mi.rcMonitor.bottom - mi.rcMonitor.top,
                   SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
     } else {
       SetWindowLong(hwnd, GWL_STYLE,
                     dwStyle | WS_OVERLAPPEDWINDOW);
       SetWindowPlacement(hwnd, &windowPlacement);
       SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
     }
}


Win_WindowDimensions
Win_GetWindowDimensions( HWND Window) {
    Win_WindowDimensions result;

    RECT ClientRect;
    GetClientRect( Window, &ClientRect);
    result.width = ClientRect.right - ClientRect.left;
    result.height = ClientRect.bottom - ClientRect.top;

    return result;
}


static int
Win_CreateGLContext()
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                              /* version */
        PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,                             /* colordepth of the framebuffer */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
        0,                              /* alpha bits (ignored) */
        0,                              /* no accumulation buffer */
        0, 0, 0, 0,                     /* accum bits (ignored) */
        24,                             /* depth buffer */
        8,                              /* no stencil buffer */
        0,                              /* no auxiliary buffers */
        0,                              /* main layer */
        0,                              /* reserved */
        0, 0, 0,                        /* no layer, visible, damage masks */
    };

    int pixelFormat = ChoosePixelFormat(DeviceContext, &pfd);
    if( !SetPixelFormat(DeviceContext, pixelFormat, &pfd)) {
        OutputDebugStringA( "FAILED: Setting pixelformat\n");
        return 0;
    }

    HGLRC tempContext = wglCreateContext( DeviceContext);
    wglMakeCurrent( DeviceContext, tempContext);

    if( glewInit() != GLEW_OK) {
        OutputDebugStringA( "FAILED: glewInit()\n");
        return 0;
    }

    int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 4,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0

    };

    if( wglewIsSupported( "WGL_ARB_create_context") == 1)
    {
        RenderingContext = wglCreateContextAttribsARB( DeviceContext, 0, attribs);
        wglMakeCurrent( NULL, NULL);
        wglDeleteContext( tempContext);
        wglMakeCurrent( DeviceContext, RenderingContext);
        OutputDebugStringA( "SUCCESS: GL 4.4 context received\n");

    } else {
        assert(0 && "No OpenGL 4.4 context received\n");
        RenderingContext = tempContext;
        OutputDebugStringA( "FAILED: GL 4.4 context. Using GL 2.1 or earlier\n");
    }

    if( !RenderingContext) {
        OutputDebugStringA( "FAILED: Getting RenderingContext\n");
        return 0;
    }

    i32 OpenGLVersion[2];
    glGetIntegerv( GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv( GL_MINOR_VERSION, &OpenGLVersion[1]);

    // OpenGL Extensions

#if VSYNC
    if (wglewIsSupported( "WGL_EXT_swap_control")) {
        wglSwapIntervalEXT(1);
    }  else {
        wglSwapIntervalEXT(1);
    }
#endif

    return 1;
}



inline LARGE_INTEGER
Win_GetWallClock() {
    LARGE_INTEGER Result;
    QueryPerformanceCounter( &Result);
    return Result;
}


inline r32
Win_GetSecondsElapsed( LARGE_INTEGER Start, LARGE_INTEGER End)
{
    float Result = ((r32)(End.QuadPart - Start.QuadPart) /
                    (r32)perfCountFrequency);
    return Result;
}


LRESULT CALLBACK
Win_WindowProc(  HWND Window,
                  UINT Message,
                  WPARAM wParam,
                  LPARAM lParam )
{
    LRESULT result = 0;
    switch( Message) {

        case WM_PAINT: {
            PAINTSTRUCT paint;
            BeginPaint( Window, &paint);
            //TODO(kasper): How to repaint properly with OpenGL
            EndPaint( Window, &paint);
        } break;
        case WM_SIZE: {
            Win_WindowDimensions dimensions = Win_GetWindowDimensions( Window);
            resize( dimensions.width, dimensions.height);
        } break;
        case WM_CREATE: {

            DeviceContext = GetDC(Window);
            if(Win_CreateGLContext()) {
                globalPlaying = true;
            }
        } break;
        case WM_ACTIVATEAPP: {

            OutputDebugStringA( "WM_ACTIVATEAPP\n");
        } break;
        case WM_CLOSE: {

            OutputDebugStringA("WM_CLOSE\n");
            globalPlaying = false;
            wglMakeCurrent( NULL, NULL);
            wglDeleteContext( RenderingContext);
            ReleaseDC( Window, DeviceContext);
        } break;
        case WM_DESTROY: {

            OutputDebugStringA("WM_DESTROY\n");
            globalPlaying = false;
        } break;
    default:
        result =  DefWindowProc(Window, Message, wParam, lParam);
    }
    return result;
}


void
Win_HandleMessages(GameInput *input) {

    MSG Message;
    while( PeekMessage( &Message, 0,0,0, PM_REMOVE)) {

        switch( Message.message ) {

            case WM_QUIT: {
                globalPlaying = false;
            } break;

            case WM_MBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN: {

                u32 VKCode = (u32)Message.wParam;
                b32 wasDown = ((Message.lParam & (1 << 30)) != 0);
                b32 isDown = ((Message.lParam & (1 << 31)) == 0);
                if( wasDown != isDown) {

                    switch (VKCode) {
                        case MK_LBUTTON: {
                            input->MouseButtons[0] = 1;
                        } break;
                        case MK_RBUTTON: {
                            input->MouseButtons[1] = 1;
                        } break;
                        case MK_MBUTTON: {
                            input->MouseButtons[2] = 1;
                        } break;
                        case MK_XBUTTON1: {
                            input->MouseButtons[3] = 1;
                        } break;
                        case MK_XBUTTON2: {
                            input->MouseButtons[4] = 1;
                        } break;
                    }
                }
            } break;

            case WM_SYSKEYUP:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN: {

                u32 VKCode = (u32)Message.wParam;
                b32 wasDown = ((Message.lParam & (1 << 30)) != 0);
                b32 isDown = ((Message.lParam & (1 << 31)) == 0);
                if( wasDown != isDown) {

                    switch (VKCode) {
                        case 'W': {
                            input->KEY_W = 1;
                            //OutputDebugStringA("W \n");
                        } break;
                        case 'A': {
                            input->KEY_A = 1;
                            //OutputDebugStringA("A \n");
                        } break;
                        case 'S': {
                            input->KEY_S = 1;
                            //OutputDebugStringA("S \n");
                        } break;
                        case 'D': {
                            input->KEY_D = 1;
                            //OutputDebugStringA("D \n");
                        } break;
                        case 'Q': {

                            //OutputDebugStringA("Q \n");
                        } break;
                        case 'E': {

                            //OutputDebugStringA("E \n");
                        } break;
                        case VK_UP: {
                            input->KEY_UP = 1;
                        } break;
                        case VK_LEFT: {
                            input->KEY_LEFT = 1;
                        } break;
                        case VK_DOWN: {
                            input->KEY_DOWN = 1;
                        } break;
                        case VK_RIGHT: {
                            input->KEY_RIGHT = 1;
                        } break;
                        case VK_SPACE: {
                            input->KEY_SPACE = 1;
                        } break;
                        case VK_ESCAPE: {
                            input->KEY_ESC = 1;
                            globalPlaying = false;
                        } break;
                    } // VKCode
                } //wasDown != isDown
                if( isDown) {

                    b32 AltKeyWasDown = (Message.lParam & (1 << 29));
                    if(( VKCode == VK_F4) && AltKeyWasDown) {
                        globalPlaying = false;
                    }
                    if(( VKCode == VK_RETURN) && AltKeyWasDown) {
                        if( Message.hwnd) {
                            Win_ToggleFullscreen( Message.hwnd);
                        }
                    }
                }

            } break;

        default: {
            TranslateMessage( &Message);
            DispatchMessageA( &Message);
        } break;

        } // switch (Message)
    } // while( PeekMessage())
}


int
CALLBACK WinMain(   HINSTANCE Instance,
                    HINSTANCE PrevInstance,
                    LPSTR CmdLine,
                    int ShowCommand )
{

#if ENABLE_CONSOLE
    if(!AllocConsole()) {
        assert(!"AllocConsole() failed");
    }

    i32 winStdOutFileDesc = _open_osfhandle((intptr_t)GetStdHandle( STD_OUTPUT_HANDLE), _O_TEXT);
    FILE *fpout = _fdopen( winStdOutFileDesc, "w");
    *stdout = *fpout;
    setvbuf( stdout, NULL, _IONBF, 0);

#endif

    // Timing
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency( &PerfCountFrequencyResult);
    perfCountFrequency = PerfCountFrequencyResult.QuadPart;

    b32 sleepIsGranular = (timeBeginPeriod( 1) == TIMERR_NOERROR);

#define GameUpdateHz 60
    r32 targetSecondsPerFrame = 1.0f / (float)GameUpdateHz;

    // WindowClass
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win_WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "KoomGLWindowClass";


    /* Has to be called before AudioEngine init */
    CoInitializeEx( NULL, COINIT_MULTITHREADED);

    if( !RegisterClassA( &WindowClass)) {
        OutputDebugStringA( "FAILED: RegisterWindowClass\n");
        return 0;
    }

    HWND Window =
            CreateWindowExA(
                    0,
                    WindowClass.lpszClassName,
                    "KoomGL",
                    WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT,
                    windowWidth, windowHeight,
                    NULL,   // PrevInstance
                    NULL,
                    Instance,
                    NULL );
    if( !Window) {
        OutputDebugStringA( "FAILED: Create Window\n");
        return 0;
    }


    // Timing
    LARGE_INTEGER LastCounter = Win_GetWallClock();
    u64 LastCycleCount = __rdtsc();
    r64 msPerFrame = 0.0f;

    MemoryStack gameMemory = {};
    gameMemory.stackSize = memoryStackSize;
    gameMemory.memoryPool = VirtualAlloc( 0, gameMemory.stackSize,
                            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    gameMemory.top = gameMemory.memoryPool;
    gameMemory.isInitialized = true;
    // TODO: Not like this you idiot.
    assert( gameMemory.memoryPool && " MemoryPool allocation failed!\n");

    gameInit( &gameMemory );

    /**
     * MainLoop
     */
    while( globalPlaying) {

        GameInput input = {};
        Win_HandleMessages(&input);

        POINT MouseP;
        GetCursorPos( &MouseP);
        ScreenToClient( Window, &MouseP);
        input.MouseX = MouseP.x;
        input.MouseY = MouseP.y;
        //TODO(kasper): Scroll wheel thing
        input.MouseZ = 0;

        input.frame = frame;
        input.deltaTime = msPerFrame;

        gameUpdateAndRender(input);

        // Timing
        LARGE_INTEGER workCounter = Win_GetWallClock();
        r32 workSecondElapsed = Win_GetSecondsElapsed( LastCounter, workCounter);
        r32 secondsElapsedForFrame = workSecondElapsed;


        if( secondsElapsedForFrame < targetSecondsPerFrame )
        {
            if( sleepIsGranular)
            {
                DWORD SleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame -
                                                    secondsElapsedForFrame));
#if !VSYNC
//#if 0
                if( SleepMS > 0) {
                    Sleep(SleepMS);
                }
#endif
            }

            float testSecondsElapsedForFrame = Win_GetSecondsElapsed( LastCounter, Win_GetWallClock());
        } else {
            //OutputDebugStringA( "MISSED FRAME\n");
        }


        LARGE_INTEGER EndCounter = Win_GetWallClock();
        msPerFrame = 1000.0f*Win_GetSecondsElapsed( LastCounter, EndCounter);
        LastCounter = EndCounter;

        SwapBuffers( DeviceContext);

        u64 endCycleCount = __rdtsc();
        u64 cyclesElapsed = endCycleCount - LastCycleCount;
        LastCycleCount = endCycleCount;

        i64 counterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;

        ++frame;

        r64 FPS = (r64)perfCountFrequency / (r64)counterElapsed;
        r64 MCPF = ((r64)cyclesElapsed / (1000.0f * 1000.0f));

        char timeStrBuffer[256];

        _snprintf_s( timeStrBuffer, sizeof( timeStrBuffer), "%.02fms/f, %.02fmc/f, frame %llu \n", msPerFrame, MCPF, frame);

        OutputDebugStringA( timeStrBuffer);
        //printf( "%s", timeStrBuffer);

    }

    return 0;
}
