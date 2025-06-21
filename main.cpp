/*********************************************************************************************
**
**	File Name:		main.cpp
**	Description:	This is the main file that runs the window and handles all the
**                  events
**
**	Author:			Clarke Needles
**	Created:		5/14/2025
**
**********************************************************************************************/

#include <tchar.h>
#include <cmath>
#include <ShlObj.h>
#include <stdlib.h>
#include <memory>
#include "winsettings.h"
#include "fractals.h"

// GLOBALS
HINSTANCE hInst; // Stored instance handle for use in Win32 API
static TCHAR szWindowClass[] = _T("FractalsApp"); // Main window class name
static TCHAR szTitle[] = _T("Fractals Playground Application"); // Application title bar
LARGE_INTEGER frequency = { 0 }; // For timing of the rendering 
LARGE_INTEGER startTime = { 0 };
LARGE_INTEGER endTime = { 0 };
LARGE_INTEGER ticks = { 0 };
bool render = false; // Variable to paint or not
bool timer = false; // Variable to time the render or not
bool canZoom = false; // Variable to zoom or not, only zoom after generation
bool recording = false; // Variable indicating recording or not
// WndProc variables
static PAINTSTRUCT ps;
static HDC hdc;
// Get the mouse click position
static POINT clickPoint = { 0 };
// For the off-screen pixel buffer
Colour* pixelBuffer = (Colour*)_aligned_malloc(sizeof(Colour) * WIDTHW * HEIGHTW, 32);;
// Current combo box selected items
std::wstring selectedOptions[3];
// For recording feature, initialize at NULL to start
GifWriter gif = { NULL, NULL, NULL };
// Objects for all the fractal options
Fractal* fractal = nullptr;


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Query the performance frequency for timing of rendering
    if (!QueryPerformanceFrequency(&frequency)) {
        MessageBox(NULL,
            _T("QueryPerformanceFrequency failed!"),
            NULL,
            NULL);

        return 1;
    }

    // Define the window class
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // Register the window class
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            NULL,
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // Create the window
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, // Non-resizeable window
        CW_USEDEFAULT, CW_USEDEFAULT,
        WIDTHW, HEIGHTW,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // Ensure window is created
    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            NULL,
            NULL);

        return 1;
    }

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Vector so combo boxes can be referred to individually
    static std::vector<HWND> hComboBoxV;

    switch (message)
    {
    case WM_CREATE:
        // Creating all of the combo-boxes
        for (const auto& cb : comboboxes)
        {
            HWND hLabel = CreateWindow(
                TEXT("STATIC"),               // Class name for static controls
                cb.label,   // Text for the label
                WS_VISIBLE | WS_CHILD,       // Styles
                cb.x + WIDTHB, cb.y, WIDTHB, HEIGHTB,
                hWnd,                        // Parent window
                NULL,                        // No menu or control identifier
                ((LPCREATESTRUCT)lParam)->hInstance,                       // Instance handle
                NULL);

            // Create a combo-box
            HWND hComboBox = CreateWindow(
                L"COMBOBOX",
                L"",
                CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                cb.x, cb.y, WIDTHB+1, cb.height,
                hWnd,
                (HMENU)cb.id,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL);

            // Ensure combobox and label is created
            if (!hComboBox || !hLabel)
            {
                MessageBox(NULL,
                    _T("Failed to create buttons."),
                    NULL,
                    NULL);

                return 1;
            }
            else {
                // Populate the combo-box with options
                for (int i = 0; i < cb.items; ++i)
                {
                    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)cb.options[i].c_str());
                }
            }

            // Adding the new combobox to the vector
            hComboBoxV.push_back(hComboBox);
        }

        // Scope so that we can create vars in a case
        {
            // Create generate button
            HWND hGenButton = CreateWindow(
                L"BUTTON",  // Predefined class; Unicode assumed 
                L"Generate",      // Button text 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                comboboxes[2].x + WIDTHB * 2 - 15, comboboxes[2].y, WIDTHB, HEIGHTB,  // Positioning
                hWnd,     // Parent window
                (HMENU)ID_GENBUTTON,       // Control ID
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL);

            // Create record button
            HWND hRecButton = CreateWindow(
                L"BUTTON",  // Predefined class; Unicode assumed 
                L"Start Recording",      // Button text 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                comboboxes[2].x + WIDTHB * 2 - 15, comboboxes[2].y + HEIGHTB, WIDTHB, HEIGHTB,  // Positioning
                hWnd,     // Parent window
                (HMENU)ID_RECBUTTON,       // Control ID
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL);

            // Ensure buttons are created
            if (!hGenButton || !hRecButton)
            {
                MessageBox(NULL,
                    _T("Failed to create buttons."),
                    NULL,
                    NULL);

                return 1;
            }
        }

        // Create compatible DC 
        hdc = GetDC(hWnd);

        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        if (render)
        {
            // Transfer off-screen bitmap onto the window
            fractal->draw(hdc, pixelBuffer, &gif, recording);
            // The image has been generated to the window
            canZoom = true;

            if (timer)
            {
                // End timer once rendered and loaded
                QueryPerformanceCounter(&endTime);

                ticks.QuadPart = endTime.QuadPart - startTime.QuadPart;
                double secondsD = static_cast<double>(ticks.QuadPart) / frequency.QuadPart;
                // Converting float into a LPCWSTR
                // Only getting first 2 decimal pts and converting s to ms
                std::string concat = std::to_string(static_cast<int>(std::round(secondsD * 1000))) + "ms";
                std::wstring unicodeStr(concat.begin(), concat.end());

                TextOut(hdc, 0, HEIGHTW - 60, unicodeStr.c_str(), unicodeStr.length());

                timer = false;
            }

            render = false;
        }

        EndPaint(hWnd, &ps);
        break;
    case WM_COMMAND:
        // Check which control sent the WM_COMMAND message
        if (LOWORD(wParam) == ID_GENBUTTON)
        {
            // Getting the selected options from the combo boxes
            for (int i = 0; i < 3; ++i)
            {
                HWND hComboBox = GetDlgItem(hWnd, ID_LANGCB+i);
                TCHAR selectedText[256];
                int currOption = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                // If nothing selected, pick the first option
                if (currOption == CB_ERR)
                {
                    // Get the first option from the box
                    SendMessage(hComboBox, CB_GETLBTEXT, 0, (LPARAM)selectedText);
                    // Update the box to have the first option text
                    SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
                }
                else
                {
                    // Get the currently selected item from the box
                    SendMessage(hComboBox, CB_GETLBTEXT, currOption, (LPARAM)selectedText);
                }

                // Add the output to the selected items vector
                selectedOptions[i] = selectedText;
            }

            // Start timer at the start of render
            timer = true;
            QueryPerformanceCounter(&startTime);

            // Render the selected fractal to the pixel buffer
            // Resetting the complex plane zoom size after generation button
            if (selectedOptions[1]._Equal(comboboxes[1].options[0]))
            {
                fractal = new Mandelbrot();
            }
            else if (selectedOptions[1]._Equal(comboboxes[1].options[1]))
            {
                fractal = new BurningShip();
            }
            else if (selectedOptions[1]._Equal(comboboxes[1].options[2]))
            {
                fractal = new Multibrot();
            }
            else if (selectedOptions[1]._Equal(comboboxes[1].options[3]))
            {
                fractal = new Nova();
            }
            else if (selectedOptions[1]._Equal(comboboxes[1].options[4]))
            {
                fractal = new Pheonix();
            }
            else
            {

            }

            // Rendering the mandelbrot to the pixel buffer
            fractal->render(pixelBuffer, selectedOptions);

            // Painting to the window
            // Force a repaint to transfer the bitmap buffer to the window
            render = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (LOWORD(wParam) == ID_RECBUTTON)
        {
            wchar_t folderPath[MAX_PATH] = { 0 };
            wchar_t text[256]; // Text buffer for getting the button text
            HWND hRecButton = GetDlgItem(hWnd, ID_RECBUTTON);
            GetWindowText(hRecButton, text, sizeof(text));

            if (!wcscmp(text, L"Start Recording"))
            {
                // Initialize BROWSEINFO structure
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = L"Select a Folder"; // Title of the folder selection dialog
                bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; // Return only folders, use modern dialog

                // Display the dialog box
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

                if (pidl)
                {
                    // Get the folder path from the item ID list
                    if (!SHGetPathFromIDList(pidl, folderPath))
                    {
                        MessageBox(NULL,
                            _T("Failed to get folder path."),
                            NULL,
                            NULL);

                        return 1;
                    }

                    // Free the PIDL allocated by SHBrowseForFolder
                    CoTaskMemFree(pidl);
                }
                else
                {
                    // No folder selected, exit
                    break;
                }

                SendMessage(hRecButton, WM_SETTEXT, 0, (LPARAM)L"Stop Recording");

                // This is converting the file path wchar_t to a char for GifBegin()
                wcscat_s(folderPath, L"\\output.gif"); // Add destination file to folder path
                size_t charLen = 0;
                wcstombs_s(&charLen, NULL, 0, folderPath, _TRUNCATE); // Get length for buffer
                char* fullPath = new char[charLen + 1];
                wcstombs_s(NULL, fullPath, charLen + 1, folderPath, _TRUNCATE); // Convet wchar_t to char

                // Start adding to the gif
                GifBegin(&gif, fullPath, WIDTHW, HEIGHTW, GIF_DELAY);
                recording = true;
            }
            else
            {
                SendMessage(hRecButton, WM_SETTEXT, 0, (LPARAM)L"Start Recording");

                GifEnd(&gif);
                recording = false;
            }
        }

        break;
    case WM_LBUTTONDOWN:
        if (canZoom)
        {
            GetCursorPos(&clickPoint);       // Retrieve screen coordinates
            ScreenToClient(hWnd, &clickPoint); // Convert to client coordinates

            fractal->moveScreen(&clickPoint);

            // Rendering the mandelbrot to the pixel buffer
            fractal->render(pixelBuffer, selectedOptions);

            // Painting to the window
            // Force a repaint to transfer the bitmap buffer to the window
            render = true;
            InvalidateRect(hWnd, NULL, TRUE);
            
        }

        break;
    case WM_MOUSEWHEEL:
        if (canZoom)
        {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

            // Scrolling up/down (zoomin in/out)
            fractal->zoomScreen(wheelDelta > 0 ? zoomType::Z_IN : zoomType::Z_OUT);

            // Rendering the mandelbrot to the pixel buffer
            fractal->render(pixelBuffer, selectedOptions);

            // Painting to the window
            // Force a repaint to transfer the bitmap buffer to the window
            render = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
