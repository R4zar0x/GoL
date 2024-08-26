#include <windows.h>
#include <CommCtrl.h>
#include <string>
#include <time.h>

#include "resource.h"

#define SETTINGS_EDIT_SIZE  6
#define TEMPLATE_EDIT_SIZE  3

#define FLASHER_VERTICAL    1
#define FLASHER_HORIZONTAL  2
#define GLIDER_0            3
#define GLIDER_90           4
#define GLIDER_180          5
#define GLIDER_270          6
#define STAR                7

// Structures

struct EDT
{
    HWND hWnd;
    INT IDC;
};
struct Array
{
    INT* ptr;
    INT size;
};

// Settings

class Settings
{
public:
    Settings() :
        size(10),
        MouseWheelSensitivity(100)
    {}
    ~Settings()
    {
        this->size = 0;
        this->MouseWheelSensitivity = 0;
    }

    INT     getSize() const
    {
        return this->size;
    }
    INT     getMouseWheelSensitivity() const
    {
        return this->MouseWheelSensitivity;
    }

    void    setSize(INT value)
    {
        this->size = value;
    }
    void    setMouseWheelSensitivity(INT value)
    {
        this->MouseWheelSensitivity = value;
    }

private:
    INT     size;
    INT     MouseWheelSensitivity;
};

// Tamplate

class Template
{
public:
    Template() :
        Template(3)
    {
        for (size_t i = 0; i < this->size; i++)
        {
            for (size_t j = 0; j < this->size; j++)
            {
                this->setCell(j, i, 1);
            }
        }

        this->setCell(this->center, this->center, 0);

    }
    Template(INT size)
    {
        this->size =
            (size & 0x1) ?
            size :
            (size + 1);
        this->center = size / 2;
        this->byteSize =
            (this->size % 8 == 0) ?
            (this->size / 8) :
            (this->size / 8 + 1);
        this->neightbourSize = 0;

        this->conditionAliveSize = 2;
        this->pConditionAlive = new INT[this->conditionAliveSize];
        this->pConditionAlive[0] = 2;
        this->pConditionAlive[1] = 3;

        this->conditionGenerateSize = 1;
        this->pConditionGenerate = new INT[this->conditionGenerateSize];
        this->pConditionGenerate[0] = 3;

        this->arr = new CHAR * [this->size];
        for (size_t i = 0; i < this->size; i++)
        {
            this->arr[i] = new CHAR[this->byteSize];
        }

        for (size_t i = 0; i < this->size; i++)
        {
            for (size_t j = 0; j < this->byteSize; j++)
            {
                this->arr[i][j] = 0x0u;
            }
        }
    }
    Template(const Template& other)
    {
        this->size = other.size;
        this->center = other.center;
        this->byteSize = other.byteSize;
        this->neightbourSize = other.neightbourSize;

        this->conditionAliveSize = other.conditionAliveSize;
        this->pConditionAlive = new INT[this->conditionAliveSize];
        memcpy(this->pConditionAlive, other.pConditionAlive, this->conditionAliveSize * sizeof(INT));

        this->conditionGenerateSize = other.conditionGenerateSize;
        this->pConditionGenerate = new INT[this->conditionGenerateSize];
        memcpy(this->pConditionGenerate, other.pConditionGenerate, this->conditionGenerateSize * sizeof(INT));

        this->arr = new CHAR * [this->size];
        for (size_t i = 0; i < this->size; i++)
        {
            this->arr[i] = new CHAR[this->byteSize];
            memcpy(this->arr[i], other.arr[i], this->byteSize * sizeof(CHAR));
        }

        if (other.pNeightbour)
        {
            this->pNeightbour = new POINT[this->neightbourSize];
            memcpy(this->pNeightbour, other.pNeightbour, this->neightbourSize * sizeof(POINT));
        }
        else
        {
            this->pNeightbour = nullptr;
        }
    }
    ~Template()
    {
        delete[] this->arr;
        delete[] this->pNeightbour;
        delete[] this->pConditionAlive;
        delete[] this->pConditionGenerate;

        this->size = 0;
        this->center = 0;
        this->byteSize = 0;
        this->neightbourSize = 0;
        this->conditionAliveSize = 0;
        this->conditionGenerateSize = 0;
    }

    BOOL    getCell(INT width, INT height) const
    {
        INT w = (width + this->size) % this->size;
        INT h = (height + this->size) % this->size;
        return (this->arr[h][w / 8] >> (w % 8)) & 0x1u;
    }
    BOOL    getCell(POINT& point) const
    {
        INT w = (point.x + this->size) % this->size;
        INT h = (point.y + this->size) % this->size;
        return (this->arr[h][w / 8] >> (w % 8)) & 0x1u;
    }
    POINT*  getNeightbour() const
    {
        return this->pNeightbour;
    }
    INT     getSize() const
    {
        return this->size;
    }
    INT     getNeightbourSize() const
    {
        return this->neightbourSize;
    }
    INT     getConditionAliveSize() const
    {
        return this->conditionAliveSize;
    }
    INT     getConditionGenerateSize() const
    {
        return this->conditionGenerateSize;
    }
    INT*    getPConditionAlive() const
    {
        return this->pConditionAlive;
    }
    INT*    getPConditionGenerate() const
    {
        return this->pConditionGenerate;
    }

    void    setCell(INT width, INT height, BOOL value)
    {
        if (value)
            this->arr[height][width / 8] |= 0x1 << (width % 8);
        else
            this->arr[height][width / 8] &= ~(0x1 << (width % 8));
    }
    void    setCell(POINT& point, BOOL value)
    {
        if (value)
            this->arr[point.y][point.x / 8] |= 0x1 << (point.x % 8);
        else
            this->arr[point.y][point.x / 8] &= ~(0x1 << (point.x % 8));
    }
    void    setPConditionAlive(INT* arr, INT size)
    {
        delete[] this->pConditionAlive;

        this->conditionAliveSize = size;
        this->pConditionAlive = new INT[size];

        for (LONG i = 0; i < size; i++)
        {
            this->pConditionAlive[i] = arr[i];
        }
    }
    void    setPConditionGenerate(INT* arr, INT size)
    {
        delete[] this->pConditionGenerate;

        this->conditionGenerateSize = size;
        this->pConditionGenerate = new INT[size];

        for (LONG i = 0; i < size; i++)
        {
            this->pConditionGenerate[i] = arr[i];
        }
    }

    POINT   returnRelativePoint(INT width, INT height) const
    {
        return { width - this->center, height - this->center };
    }

    void    generate()
    {
        this->neightbourSize = 0;
        this->pNeightbour = nullptr;

        for (size_t i = 0; i < this->size; i++)
        {
            for (size_t j = 0; j < this->size; j++)
            {
                if (this->getCell(j, i))
                {
                    this->neightbourSize++;

                    POINT* intNew = new POINT[this->neightbourSize];
                    if (this->pNeightbour != nullptr)
                        memcpy(intNew, this->pNeightbour, this->neightbourSize * sizeof(POINT));

                    intNew[this->neightbourSize - 1] = this->returnRelativePoint(j, i);
                    this->pNeightbour = intNew;
                }
            }
        }
    }

    BOOL    isEmpty() const
    {
        for (LONG i = 0; i < this->size; i++)
        {
            for (LONG j = 0; j < this->size; j++)
            {
                if (this->getCell(j, i))
                {
                    return FALSE;
                }
            }
        }
        return TRUE;
    }
    BOOL    validateNeightboursAlive(INT value) const
    {
        for (INT i = 0; i < this->conditionAliveSize; i++)
        {
            if (value == this->pConditionAlive[i])
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    BOOL    validateNeightboursGenerate(INT value) const
    {
        for (INT i = 0; i < this->conditionGenerateSize; i++)
        {
            if (value == this->pConditionGenerate[i])
            {
                return TRUE;
            }
        }
        return FALSE;
    }

private:
    INT     size;
    INT     center;
    INT     byteSize;
    CHAR**  arr = nullptr;

    INT     neightbourSize;
    POINT*  pNeightbour = nullptr;

    INT     conditionAliveSize;
    INT*    pConditionAlive = nullptr;

    INT     conditionGenerateSize;
    INT*    pConditionGenerate = nullptr;
};

// Grid

class Grid
{
public:
    Grid() :
        Grid(16, 16)
    {}
    Grid(INT w, INT h) :
        width(w),
        height(h)
    {
        this->byteWidth =
            (this->width % 8 == 0) ?
            (this->width / 8) :
            (this->width / 8 + 1);

        this->arr = new CHAR * [this->height];
        for (size_t i = 0; i < this->height; i++)
        {
            this->arr[i] = new CHAR[this->byteWidth];
        }

        for (size_t i = 0; i < this->height; i++)
        {
            for (size_t j = 0; j < this->byteWidth; j++)
            {
                this->arr[i][j] = 0x0u;
            }
        }
    }
    ~Grid()
    {
        delete[] this->arr;

        this->width = 0;
        this->height = 0;
        this->byteWidth = 0;
    }

    INT     getWidth()
    {
        return this->width;
    }
    INT     getHeight()
    {
        return this->height;
    }

    BOOL    getCell(INT width, INT height)
    {
        INT w = (width + this->width) % this->width;
        INT h = (height + this->height) % this->height;

        return (this->arr[h][w / 8] >> (w % 8)) & 0x1;
    }
    BOOL    getCell(POINT point)
    {
        INT w = (point.x + this->width) % this->width;
        INT h = (point.y + this->height) % this->height;

        return (this->arr[h][w / 8] >> (w % 8)) & 0x1;
    }

    void    setCell(INT width, INT height, BOOL value)
    {
        INT w = (width + this->width) % this->width;
        INT h = (height + this->height) % this->height;

        if (value)
            this->arr[h][w / 8] |= 0x1 << (w % 8);
        else
            this->arr[h][w / 8] &= ~(0x1 << (w % 8));
    }
    void    setCell(POINT point, BOOL value)
    {
        INT w = (point.x + this->width) % this->width;
        INT h = (point.y + this->height) % this->height;

        if (value)
            this->arr[h][w / 8] |= 0x1 << (w % 8);
        else
            this->arr[h][w / 8] &= ~(0x1 << (w % 8));
    }

    void    evolve(Template* tmplt)
    {
        tmplt->generate();

        CHAR** newGrid = new CHAR * [this->height];
        for (LONG i = 0; i < this->height; i++)
        {
            newGrid[i] = new CHAR[this->width];
        }

        for (LONG i = 0; i < this->height; i++)
        {
            for (LONG j = 0; j < this->width; j++)
            {
                newGrid[i][j] = 0x0;
            }
        }

        for (LONG i = 0; i < this->height; i++)
        {
            for (LONG j = 0; j < this->width; j++)
            {
                BOOL bit = this->evolveBit({ j, i }, tmplt);
                newGrid[i][j / 8] |= bit << (j % 8);
            }
        }

        delete[] this->arr;

        this->arr = newGrid;
    }

    BOOL    evolveBit(POINT bitPoint, Template* tmplt)
    {
        POINT* arr = tmplt->getNeightbour();
        INT size = tmplt->getNeightbourSize();

        INT aliveNeightbours = 0;
        POINT buffer = { 0, 0 };

        for (size_t i = 0; i < size; i++)
        {
            buffer.x = bitPoint.x + arr[i].x;
            buffer.y = bitPoint.y + arr[i].y;

            aliveNeightbours += this->getCell(buffer.x, buffer.y);
        }

        if (this->getCell(bitPoint.x, bitPoint.y)) // alive
        {
            if (tmplt->validateNeightboursAlive(aliveNeightbours)) // must be alive
                return TRUE;
            else
                return FALSE;
        }
        else //dead
        {
            if (tmplt->validateNeightboursGenerate(aliveNeightbours)) // must be generated
                return TRUE;
            else
                return FALSE;
        }
    }

private:
    INT     width;
    INT     height;
    INT     byteWidth;

    CHAR**  arr = nullptr;
};

class Fild
{
public:
    Fild() :
        Fild(16, 16)
    {}
    Fild(INT width, INT height) :
        basePosition({ 0, 0 }),
        pGrid(new Grid(width, height)),
        pSettings(new Settings()),
        pTemplate(new Template())
    {}
    ~Fild()
    {
        delete this->pGrid;
        delete this->pSettings;
        delete this->pTemplate;
    }

    void    setGrid(Grid* grid)
    {
        this->pGrid = grid;
    }
    void    setSettings(Settings* settings)
    {
        this->pSettings = settings;
    }
    void    setTemplate(Template* tmplt)
    {
        this->pTemplate = tmplt;
    }
    void    setBasePosition(POINT value)
    {
        this->basePosition = value;
    }

    Grid*       getGrid() const
    {
        return this->pGrid;
    }
    Settings*   getSettings() const
    {
        return this->pSettings;
    }
    Template*   getTemplate() const
    {
        return this->pTemplate;
    }
    POINT       getBasePosition() const
    {
        return this->basePosition;
    }

    BOOL    pointInRect(POINT& point) 
    {
        INT size = this->pSettings->getSize();
        RECT rc = {
            this->basePosition.x,
            this->basePosition.y,
            this->basePosition.x + size * this->pGrid->getWidth(),
            this->basePosition.y + size * this->pGrid->getHeight()
        };

        if ((rc.left <= point.x && point.x <= rc.right) &&
            (rc.top < point.y && point.y < rc.bottom))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    POINT   selectedCell(POINT point)
    {
        return {
            (point.x - this->basePosition.x) / this->pSettings->getSize(),
            (point.y - this->basePosition.y) / this->pSettings->getSize()
        };
    }

    void    draw(HWND hWnd) const
    {
        PAINTSTRUCT ps;
        POINT buffer;

        INT squareSize = this->pSettings->getSize();
        INT width = this->pGrid->getWidth();
        INT height = this->pGrid->getHeight();

        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH redbrush = CreateSolidBrush(RGB(255, 0, 0));
        HBRUSH graybrush = CreateSolidBrush(RGB(128, 128, 128));
        HBRUSH blackbrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH whitebrush = CreateSolidBrush(RGB(255, 255, 255));

        for (size_t i = 0; i < height; i++)
        {
            for (size_t j = 0; j < width; j++)
            {
                buffer.x = basePosition.x + squareSize * i;
                buffer.y = basePosition.y + squareSize * j;

                RECT rc = {
                    buffer.x,
                    buffer.y,
                    buffer.x + squareSize,
                    buffer.y + squareSize
                };

                FillRect(hdc, &rc, (this->pGrid->getCell(i, j)) ? blackbrush : whitebrush);
                FrameRect(hdc, &rc, graybrush);
            }
        }

        RECT rc = {
            basePosition.x,
            basePosition.y,
            basePosition.x + squareSize * height,
            basePosition.y + squareSize * width
        };

        FrameRect(hdc, &rc, redbrush);

        DeleteObject(redbrush);
        DeleteObject(graybrush);
        DeleteObject(blackbrush);
        DeleteObject(whitebrush);
        EndPaint(hWnd, &ps);
    }

    void    evolve(HWND hWnd)
    {
        this->pGrid->evolve(this->pTemplate);

        InvalidateRect(hWnd, NULL, TRUE);
    }
    void    evolveMany(HWND hWnd, INT iterations)
    {
        for (size_t i = 0; i < iterations; i++)
        {
            this->pGrid->evolve(this->pTemplate);
        }

        InvalidateRect(hWnd, NULL, TRUE);
    }

private:
    POINT   basePosition;

    Grid*       pGrid = nullptr;
    Settings*   pSettings = nullptr;
    Template*   pTemplate = nullptr;

};

static HINSTANCE hInst;
static Fild* fild = nullptr;
static INT iterations;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static void     templateDlgValues(HWND hDlg, Template* tmplt, INT& sizeRect, POINT& basePoint);
static BOOL     templatePointInRect(POINT& basePoint, POINT& point, INT size, INT sizeRect);
static POINT    templateSelectedCell(HWND hDlg, POINT& basePosition, POINT& point, INT sizeRect);
static INT*     templateCharBufferToArr(CHAR* buff, INT& length);
static Array    templateGetDataFromEdit(HWND hDlg, EDT values);

static BOOL     validateGridSizeForSpawn(HWND hWnd, INT size);

static void     writeCursorPos(HWND hWnd, POINT& CursorPos);

static void     setFlasherVertical  (Grid* grid, POINT pt);
static void     setFlasherHorizontal(Grid* grid, POINT pt);

static void     setGlider0  (Grid* grid, POINT pt);
static void     setGlider90 (Grid* grid, POINT pt);
static void     setGlider180(Grid* grid, POINT pt);
static void     setGlider270(Grid* grid, POINT pt);

static void setStar(Grid* grid, POINT pt);

LRESULT CALLBACK DlgProcIterations  (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcTemplate    (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcSettings    (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcAbout       (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcHelp        (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcInfo        (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wcex.lpszClassName = "QWE";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance;
    hWnd = CreateWindow(
        "QWE",
        "Game of Life",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        NULL,
        400,
        400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

static void     templateDlgValues(HWND hDlg, Template* tmplt, INT& sizeRect,POINT& basePoint)
{
    INT size = tmplt->getSize();

    HWND hCanvas = GetDlgItem(hDlg, IDC_CANVAS);
    RECT clientRect;
    
    GetClientRect(hCanvas, &clientRect);
    
    if (clientRect.bottom - (size * sizeRect) < 0)
    {
        sizeRect += ((clientRect.bottom - (size * sizeRect)) / size) - 1;
    }
    
    basePoint = { clientRect.right / 2 - (size * sizeRect) / 2, clientRect.bottom / 2 - (size * sizeRect) / 2 };
}
static BOOL     templatePointInRect(POINT& basePoint, POINT& point, INT size, INT sizeRect)
{
    RECT rc = {
        basePoint.x,
        basePoint.y,
        basePoint.x + size * sizeRect,
        basePoint.y + size * sizeRect
    };

    if ((rc.left <= point.x && point.x <= rc.right) &&
        (rc.top < point.y && point.y < rc.bottom))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
static POINT    templateSelectedCell(HWND hDlg, POINT& basePosition, POINT& point, INT sizeRect)
{
    return {
        (point.x - basePosition.x) / sizeRect,
        (point.y - basePosition.y) / sizeRect
    };
}
static INT*     templateCharBufferToArr(CHAR* buff, INT& length)
{
    INT* intArray = nullptr;
    INT sz = 0;

    std::string str = "";

    for (size_t i = 0; i < length + 1; i++)
    {
        if (buff[i] >= '0' && buff[i] <= '9')
        {
            str += buff[i];
        }
        else
        {
            sz++;
            INT* intNew = new INT[sz];
            if (intArray != nullptr)
                memcpy(intNew, intArray, (sz - 1) * sizeof(INT));

            intNew[sz - 1] = std::stoi(str);
            intArray = intNew;
            str = "";
        }
    }
    length = sz;
    return intArray;
}
static Array    templateGetDataFromEdit(HWND hDlg, EDT values)
{
    Array result;
    result.ptr = nullptr;
    result.size = 0;

    INT length = SendMessage(values.hWnd, WM_GETTEXTLENGTH, 0, 0);
    if (length != 0)
    {
        CHAR* buff = new CHAR[length + 1];
        GetDlgItemText(hDlg, values.IDC, buff, length + 1);

        result.size = length;
        result.ptr = templateCharBufferToArr(buff, result.size);

        delete[] buff;
    }
    else
    {
        result.ptr = new INT[1];
        result.ptr[0] = 0;

        result.size = 1;
    }

    return result;
}

static BOOL     validateGridSizeForSpawn(HWND hWnd, INT size)
{
    Grid* grid = fild->getGrid();
    if (grid->getWidth() < size && grid->getHeight() < size)
    {
        MessageBox(hWnd, "Can`t spawn object.\nFild is small", "Error", MB_OK | MB_ICONERROR);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void     writeCursorPos(HWND hWnd, POINT& CursorPos)
{
    GetCursorPos(&CursorPos);
    ScreenToClient(hWnd, &CursorPos);
}

static void     setFlasherVertical(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 3; i++)
    {
        for (LONG j = 0; j < 3; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 1, pt.y + 0, 1);
    grid->setCell(pt.x + 1, pt.y + 1, 1);
    grid->setCell(pt.x + 1, pt.y + 2, 1);
}
static void     setFlasherHorizontal(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 3; i++)
    {
        for (LONG j = 0; j < 3; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 0, pt.y + 1, 1);
    grid->setCell(pt.x + 1, pt.y + 1, 1);
    grid->setCell(pt.x + 2, pt.y + 1, 1);
}

static void     setGlider0(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 3; i++)
    {
        for (LONG j = 0; j < 3; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 1,     pt.y + 0,   1);
    grid->setCell(pt.x + 2,     pt.y + 0,   1);
    grid->setCell(pt.x + 0,     pt.y + 1,   1);
    grid->setCell(pt.x + 2,     pt.y + 1,   1);
    grid->setCell(pt.x + 2,     pt.y + 2,   1);
}
static void     setGlider90(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 3; i++)
    {
        for (LONG j = 0; j < 3; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 1,     pt.y + 0,   1);
    grid->setCell(pt.x + 2,     pt.y + 1,   1);
    grid->setCell(pt.x + 0,     pt.y + 2,   1);
    grid->setCell(pt.x + 1,     pt.y + 2,   1);
    grid->setCell(pt.x + 2,     pt.y + 2,   1);
}
static void     setGlider180(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 3; i++)
    {
        for (LONG j = 0; j < 3; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 0,     pt.y + 0,   1);
    grid->setCell(pt.x + 0,     pt.y + 1,   1);
    grid->setCell(pt.x + 2,     pt.y + 1,   1);
    grid->setCell(pt.x + 0,     pt.y + 2,   1);
    grid->setCell(pt.x + 1,     pt.y + 2,   1);
}
static void     setGlider270(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 3; i++)
    {
        for (LONG j = 0; j < 3; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 1,     pt.y + 0,   1);
    grid->setCell(pt.x + 2,     pt.y + 0,   1);
    grid->setCell(pt.x + 0,     pt.y + 1,   1);
    grid->setCell(pt.x + 0,     pt.y + 0,   1);
    grid->setCell(pt.x + 1,     pt.y + 2,   1);
}

static void     setStar(Grid* grid, POINT pt)
{
    for (LONG i = 0; i < 13; i++)
    {
        for (LONG j = 0; j < 13; j++)
        {
            grid->setCell(pt.x + j, pt.y + i, 0);
        }
    }

    grid->setCell(pt.x + 5,     pt.y + 1,   1);
    grid->setCell(pt.x + 6,     pt.y + 1,   1);
    grid->setCell(pt.x + 7,     pt.y + 1,   1);
    grid->setCell(pt.x + 3,     pt.y + 3,   1);
    grid->setCell(pt.x + 5,     pt.y + 3,   1);
    grid->setCell(pt.x + 7,     pt.y + 3,   1);
    grid->setCell(pt.x + 9,     pt.y + 3,   1);
    grid->setCell(pt.x + 1,     pt.y + 5,   1);
    grid->setCell(pt.x + 3,     pt.y + 5,   1);
    grid->setCell(pt.x + 9,     pt.y + 5,   1);
    grid->setCell(pt.x + 11,    pt.y + 5,   1);
    grid->setCell(pt.x + 1,     pt.y + 6,   1);
    grid->setCell(pt.x + 11,    pt.y + 6,   1);
    grid->setCell(pt.x + 1,     pt.y + 7,   1);
    grid->setCell(pt.x + 3,     pt.y + 7,   1);
    grid->setCell(pt.x + 9,     pt.y + 7,   1);
    grid->setCell(pt.x + 11,    pt.y + 7,   1);
    grid->setCell(pt.x + 3,     pt.y + 9,   1);
    grid->setCell(pt.x + 5,     pt.y + 9,   1);
    grid->setCell(pt.x + 7,     pt.y + 9,   1);
    grid->setCell(pt.x + 9,     pt.y + 9,   1);
    grid->setCell(pt.x + 5,     pt.y + 11,  1);
    grid->setCell(pt.x + 6,     pt.y + 11,  1);
    grid->setCell(pt.x + 7,     pt.y + 11,  1);
}

LRESULT CALLBACK DlgProcIterations(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_CLOSE:
        EndDialog(hDlg, wParam);
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
        {
            CHAR* buff;
            HWND hEdit = GetDlgItem(hDlg, IDC_EDIT1);
            int length = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);

            if (length != 0)
            {
                buff = new CHAR[length + 1];

                GetDlgItemText(hDlg, IDC_EDIT1, buff, length + 1);
                iterations = std::stoi(buff);

                delete[] buff;
                EndDialog(hDlg, wParam);
            }
            else
            {
                MessageBox(hDlg, "Something went wrong.\nYou didn`t enter the value", "Error", MB_OK | MB_ICONERROR);
            }
        }
        return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            break;
        }
        break;
    }
    return FALSE;
}
LRESULT CALLBACK DlgProcTemplate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static EDT edit[TEMPLATE_EDIT_SIZE];
    
    static BOOL isAffected;
    static INT sizeRect;
    static POINT basePoint;
    static Template* tmplt;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        isAffected = FALSE;
        sizeRect = 40;

        tmplt = new Template(*fild->getTemplate());
        templateDlgValues(hDlg, tmplt, sizeRect, basePoint);

        edit[0].IDC = IDC_EDIT1;
        edit[0].hWnd = GetDlgItem(hDlg, IDC_EDIT1);
        edit[1].IDC = IDC_EDIT2;
        edit[1].hWnd = GetDlgItem(hDlg, IDC_EDIT2);
        edit[2].IDC = IDC_EDIT3;
        edit[2].hWnd = GetDlgItem(hDlg, IDC_EDIT3);

        std::string str;
        INT sizeArr;
        INT* arr;
        
        SetWindowText(edit[0].hWnd, std::to_string(tmplt->getSize()).c_str());

        str = "";
        sizeArr = tmplt->getConditionAliveSize();
        arr = tmplt->getPConditionAlive();
        for (LONG i = 0; i < sizeArr; i++)
        {
            str += std::to_string(arr[i]);

            if (i != sizeArr - 1)
            {
                str += " ";
            }
        }
        SetWindowText(edit[1].hWnd, str.c_str());
        
        str = "";
        sizeArr = tmplt->getConditionGenerateSize();
        arr = tmplt->getPConditionGenerate();
        for (LONG i = 0; i < sizeArr; i++)
        {
            str += std::to_string(arr[i]); 
            
            if (i != sizeArr - 1)
            {
                str += " ";
            }
        }
        SetWindowText(edit[2].hWnd, str.c_str());
    }
        return TRUE;
    case WM_LBUTTONDOWN:
    {
        POINT mousePosition;
        writeCursorPos(GetDlgItem(hDlg, IDC_CANVAS), mousePosition);

        if (!templatePointInRect( basePoint, mousePosition, tmplt->getSize(), sizeRect))
        {
            return TRUE;
        }
        
        mousePosition = templateSelectedCell(hDlg, basePoint, mousePosition, sizeRect);

        if (mousePosition.x == tmplt->getSize() / 2 && mousePosition.y == tmplt->getSize() / 2)
        {
            return TRUE;
        }

        tmplt->setCell(mousePosition, tmplt->getCell(mousePosition) ? 0x0 : 0x1);

        InvalidateRect(hDlg, NULL, TRUE);
    }
        return TRUE;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDAPPLY:
        {
            INT value = 0;
            INT length = SendMessage(edit[0].hWnd, WM_GETTEXTLENGTH, 0, 0);
            if (length != 0)
            {
                CHAR* text = new CHAR[length + 1];

                GetDlgItemText(hDlg, edit[0].IDC, text, length + 1);
                value = std::stoi(text);

                delete[] text;
            }
            else
            {
                MessageBox(hDlg, "One of parameters is empty", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            if (value < 2)
            {
                MessageBox(hDlg, "Parameter is invalid", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            if (tmplt->getSize() == value)
            {
                MessageBox(hDlg, "Parameter is equal", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            tmplt = new Template(value);

            isAffected = TRUE;

            SetWindowText(edit[0].hWnd, std::to_string(tmplt->getSize()).c_str());

            templateDlgValues(hDlg, tmplt, sizeRect, basePoint);

            InvalidateRect(hDlg, NULL, TRUE);
        }
            return TRUE;
        case IDC_BUTTON1:
        {
            tmplt = new Template(tmplt->getSize());

            std::string str;
            INT sizeArr;
            INT* arr;

            SetWindowText(edit[0].hWnd, std::to_string(tmplt->getSize()).c_str());

            str = "";
            sizeArr = tmplt->getConditionAliveSize();
            arr = tmplt->getPConditionAlive();
            for (LONG i = 0; i < sizeArr; i++)
            {
                str += std::to_string(arr[i]);

                if (i != sizeArr - 1)
                {
                    str += " ";
                }
            }
            SetWindowText(edit[1].hWnd, str.c_str());

            str = "";
            sizeArr = tmplt->getConditionGenerateSize();
            arr = tmplt->getPConditionGenerate();
            for (LONG i = 0; i < sizeArr; i++)
            {
                str += std::to_string(arr[i]);

                if (i != sizeArr - 1)
                {
                    str += " ";
                }
            }
            SetWindowText(edit[2].hWnd, str.c_str());

            InvalidateRect(hDlg, NULL, TRUE);
        }
            return TRUE;
        case IDOK:
        {
            if (tmplt->isEmpty())
            {
                MessageBox(hDlg, "Template fild is empty", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            Array arr;
            
            arr = templateGetDataFromEdit(hDlg, edit[1]);
            tmplt->setPConditionAlive(arr.ptr, arr.size);

            arr = templateGetDataFromEdit(hDlg, edit[2]);
            tmplt->setPConditionGenerate(arr.ptr, arr.size);

            fild->setTemplate(new Template(*tmplt));
            tmplt = nullptr;

            EndDialog(hDlg, wParam);
        }
        return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            break;
        }
        break;
    }
        return FALSE;
    case WM_PAINT:
    {
        HWND hCanvas = GetDlgItem(hDlg, IDC_CANVAS);
        if (!hCanvas)
        {
            MessageBox(hDlg, "Can't draw", "Error", MB_OK | MB_ICONERROR);
            return TRUE;
        }

        PAINTSTRUCT ps;
        HDC hdcCanvas = BeginPaint(hDlg, &ps);

        HDC hdc = GetDC(hCanvas);

        INT size = tmplt->getSize();

        HBRUSH blackbrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH greenbrush = CreateSolidBrush(RGB(146, 208, 80));
        HBRUSH whitebrush = CreateSolidBrush(RGB(255, 255, 255));

        for (LONG i = 0; i < size; i++)
        {
            for (LONG j = 0; j < size; j++)
            {
                POINT buffer;
                buffer.x = basePoint.x + sizeRect * i;
                buffer.y = basePoint.y + sizeRect * j;

                RECT rc = {
                    buffer.x,
                    buffer.y,
                    buffer.x + sizeRect,
                    buffer.y + sizeRect
                };
                if (tmplt->getSize() / 2 == i && tmplt->getSize() / 2 == j)
                    FillRect(hdc, &rc, blackbrush);
                else
                    FillRect(hdc, &rc, (tmplt->getCell(i, j)) ? greenbrush : whitebrush);
                FrameRect(hdc, &rc, blackbrush);
            }
        }

        DeleteObject(blackbrush);
        DeleteObject(greenbrush);
        DeleteObject(whitebrush);

        ReleaseDC(hCanvas, hdc);
        EndPaint(hDlg, &ps);
        return TRUE;
    }
    
    case WM_CLOSE:
        EndDialog(hDlg, wParam);
        return TRUE;
    default:
        break;
    }
    return FALSE;
}
LRESULT CALLBACK DlgProcSettings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND* hWnd;

    static EDT edit[SETTINGS_EDIT_SIZE];

    static Grid* grid;
    static Settings* settings;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        hWnd = (HWND*)lParam;

        grid = fild->getGrid();
        settings = fild->getSettings();

        edit[0].IDC = IDC_EDIT1;
        edit[0].hWnd = GetDlgItem(hDlg, IDC_EDIT1);
        edit[1].IDC = IDC_EDIT2;
        edit[1].hWnd = GetDlgItem(hDlg, IDC_EDIT2);
        edit[2].IDC = IDC_EDIT3;
        edit[2].hWnd = GetDlgItem(hDlg, IDC_EDIT3);
        edit[3].IDC = IDC_EDIT4;
        edit[3].hWnd = GetDlgItem(hDlg, IDC_EDIT4);
        edit[4].IDC = IDC_EDIT5;
        edit[4].hWnd = GetDlgItem(hDlg, IDC_EDIT5);
        edit[5].IDC = IDC_EDIT6;
        edit[5].hWnd = GetDlgItem(hDlg, IDC_EDIT6);

        SetWindowText(edit[0].hWnd, std::to_string(grid->getWidth()).c_str());
        SetWindowText(edit[1].hWnd, std::to_string(grid->getHeight()).c_str());
        SetWindowText(edit[2].hWnd, std::to_string(fild->getBasePosition().x).c_str());
        SetWindowText(edit[3].hWnd, std::to_string(fild->getBasePosition().y).c_str());
        SetWindowText(edit[4].hWnd, std::to_string(settings->getSize()).c_str());
        SetWindowText(edit[5].hWnd, std::to_string(settings->getMouseWheelSensitivity()).c_str());
    }
        return TRUE;
    case WM_CLOSE:
        EndDialog(hDlg, wParam);
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDABOUT:
        {
            INT buffer[SETTINGS_EDIT_SIZE] = {};
            for (LONG i = 0; i < SETTINGS_EDIT_SIZE; i++)
            {
                INT length = SendMessage(edit[i].hWnd, WM_GETTEXTLENGTH, 0, 0);
                if (length != 0)
                {
                    CHAR* text = new CHAR[length + 1];

                    GetDlgItemText(hDlg, edit[i].IDC, text, length + 1);
                    buffer[i] = std::stoi(text);

                    delete[] text;
                }
                else
                {
                    MessageBox(hDlg, "One of parameters is empty", "Error", MB_OK | MB_ICONERROR);
                    return TRUE;
                }
            }

            for (LONG i = 0; i < SETTINGS_EDIT_SIZE; i++)
            {
                if (buffer[i] < 1 && i != 2 && i != 3)
                {
                    MessageBox(hDlg, "One of parameters is zero", "Error", MB_OK | MB_ICONERROR);
                    return TRUE;
                }
            }

            if (buffer[0] != grid->getWidth() || buffer[1] != grid->getHeight())
            {
                fild = new Fild(buffer[0], buffer[1]);
            }
            if (buffer[2] != fild->getBasePosition().x)
            {
                fild->setBasePosition({ buffer[2], fild->getBasePosition().y });
            }
            if (buffer[3] != fild->getBasePosition().y)
            {
                fild->setBasePosition({ fild->getBasePosition().x, buffer[3] });
            }
            if (buffer[4] != settings->getSize())
            {
                settings->setSize(buffer[4]);
            }
            if (buffer[5] != settings->getMouseWheelSensitivity())
            {
                settings->setMouseWheelSensitivity(buffer[5]);
            }

            InvalidateRect(*hWnd, NULL, TRUE);
        }
            return TRUE;
        case IDOK:
        {
            INT buffer[SETTINGS_EDIT_SIZE] = {};
            for (LONG i = 0; i < SETTINGS_EDIT_SIZE; i++)
            {
                INT length = SendMessage(edit[i].hWnd, WM_GETTEXTLENGTH, 0, 0);
                if (length != 0)
                {
                    CHAR* text = new CHAR[length + 1];

                    GetDlgItemText(hDlg, edit[i].IDC, text, length + 1);
                    buffer[i] = std::stoi(text);

                    delete[] text;
                }
                else
                {
                    MessageBox(hDlg, "One of parameters is empty", "Error", MB_OK | MB_ICONERROR);
                    return TRUE;
                }
            }

            for (LONG i = 0; i < SETTINGS_EDIT_SIZE; i++)
            {
                if (buffer[i] < 1 && i != 2 && i != 3)
                {
                    MessageBox(hDlg, "One of parameters is zero", "Error", MB_OK | MB_ICONERROR);
                    return TRUE;
                }
            }

            if (buffer[0] != grid->getWidth() || buffer[1] != grid->getHeight())
            {
                fild = new Fild(buffer[0], buffer[1]);
            }
            if (buffer[2] != fild->getBasePosition().x)
            {
                fild->setBasePosition({ buffer[2], fild->getBasePosition().y });
            }
            if (buffer[3] != fild->getBasePosition().y)
            {
                fild->setBasePosition({ fild->getBasePosition().x, buffer[3] });
            }
            if (buffer[4] != settings->getSize())
            {
                settings->setSize(buffer[4]);
            }
            if (buffer[5] != settings->getMouseWheelSensitivity())
            {
                settings->setMouseWheelSensitivity(buffer[5]);
            }
            
            InvalidateRect(*hWnd, NULL, TRUE);
            EndDialog(hDlg, wParam);
        }
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            break;
        }
        break;
    }
    return FALSE;
}
LRESULT CALLBACK DlgProcAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_CLOSE:
        EndDialog(hDlg, wParam);
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            break;
        }
        break;
    }
    return FALSE;
}
LRESULT CALLBACK DlgProcHelp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {      
        HWND hList = GetDlgItem(hDlg, IDC_EDIT1);

        HANDLE fileHandle = CreateFile(
            "help.txt",
            GENERIC_READ,
            NULL,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        LARGE_INTEGER fileSize;
        GetFileSizeEx(fileHandle, &fileSize);

        CHAR* buffer = new CHAR[fileSize.QuadPart + 1];

        DWORD bytesRead;
        if (ReadFile(fileHandle, buffer, fileSize.QuadPart, &bytesRead, NULL))
        {
            SetWindowText(hList, buffer);
        }
        else
        {
            MessageBox(hDlg, "Can not open system file", "Error", MB_OK | MB_ICONERROR);
        }

        CloseHandle(fileHandle);
    }
        return TRUE;
    case WM_CLOSE:
        EndDialog(hDlg, wParam);
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            break;
        }
        break;
    }
    return FALSE;
}
LRESULT CALLBACK DlgProcInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        CHAR buffer[] =
"The Game of Life, also known simply as Life, is a cellular automaton \
devised by the British mathematician John Horton Conway in 1970. It is \
a zero-player game, meaning that its evolution is determined by its \
initial state, requiring no further input. One interacts with the \
Game of Life by creating an initial configuration and observing how \
it evolves. It is Turing complete and can simulate a universal \
constructor or any other Turing machine.";
        
        HWND hList = GetDlgItem(hDlg, IDC_EDIT1);
        
        SetWindowText(hList, buffer);
    }
        return TRUE;
    case WM_CLOSE:
        EndDialog(hDlg, wParam);
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
            EndDialog(hDlg, wParam);
            return TRUE;
        case IDCLOSE:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            break;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL runEvolution;
    static INT structID;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        hWnd = hWnd;
        srand(time(NULL));
        SetTimer(hWnd, 1, 500, NULL);

        fild = new Fild;
        fild->setBasePosition({65, 45});

        Settings* settings = fild->getSettings();
        settings->setSize(15);

        Grid* grid = fild->getGrid();
        setGlider0(grid, { 5, 5 });

        runEvolution = FALSE;
        structID = NULL;
    }
        break;
    case WM_TIMER:
    {
        if (runEvolution)
        {
            fild->evolve(hWnd);
        }
    }
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
            // File
        case ID_FILE_SETDEFAULTMAP:
        {
            Grid* grid = fild->getGrid();
            INT wd = grid->getWidth();
            INT hg = grid->getHeight();

            for (size_t i = 0; i < wd; i++)
            {
                for (size_t j = 0; j < hg; j++)
                {
                    grid->setCell(j, i, 0);
                }
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;
        case ID_FILE_SETRANDOMMAP:
        {
            Grid* grid = fild->getGrid();
            INT wd = grid->getWidth();
            INT hg = grid->getHeight();

            for (size_t i = 0; i < wd; i++)
            {
                for (size_t j = 0; j < hg; j++)
                {
                    grid->setCell(j, i, rand() % 2);
                }
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;
        case ID_FILE_MAKEONEITERATION:
        {
            fild->evolve(hWnd);
        }
            break;
        case ID_FILE_MAKEALOTOFITERATIONS:
        {
            iterations = 0;

            DialogBox(hInst,
                MAKEINTRESOURCE(IDD_ITERATIONS),
                hWnd,
                DlgProcIterations
            );
            if (iterations)
                fild->evolveMany(hWnd, iterations);
        }
            break;
        case ID_FILE_RUN:
        {
            runEvolution = TRUE;
        }
            break;
        case ID_FILE_STOP:
        {
            runEvolution = FALSE;
        }
            break;
            // Structures
        case ID_SETFLASHER_VERTICAL:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 3))
            {
                break;
            }

            structID = FLASHER_VERTICAL;
        }
        break;
        case ID_SETFLASHER_HORIZONTAL:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 3))
            {
                break;
            }

            structID = FLASHER_HORIZONTAL;
        }
        break;
        case ID_SETGLIDER_ROTATE0GRAD:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 3))
            {
                break;
            }

            structID = GLIDER_0;
        }
        break;
        case ID_SETGLIDER_ROTATE90GRAD:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 3))
            {
                break;
            }

            structID = GLIDER_90;
        }
        break;
        case ID_SETGLIDER_ROTATE180GRAD:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 3))
            {
                break;
            }

            structID = GLIDER_180;
        }
        break;
        case ID_SETGLIDER_ROTATE270GRAD:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 3))
            {
                break;
            }

            structID = GLIDER_270;
        }
        break;
        case ID_STRUCTURES_SETSTAR:
        {
            if (runEvolution)
            {
                break;
            }
            if (validateGridSizeForSpawn(hWnd, 13))
            {
                break;
            }

            structID = STAR;
        }
        break;
            // Vicinity
        case ID_VICINITY_VONNEUMANN1STORDER:
        {
            Template* tmplt = new Template(3);

            tmplt->setCell(1, 0, 1);
            tmplt->setCell(0, 1, 1);
            tmplt->setCell(2, 1, 1);
            tmplt->setCell(1, 2, 1);

            fild->setTemplate(tmplt);
        }
            break;
        case ID_VICINITY_VONNEUMANN2NDORDER:
        {
            Template* tmplt = new Template(5);

            tmplt->setCell(2, 0, 1);
            tmplt->setCell(1, 1, 1);
            tmplt->setCell(2, 1, 1);
            tmplt->setCell(3, 1, 1);
            tmplt->setCell(0, 2, 1);
            tmplt->setCell(1, 2, 1);
            tmplt->setCell(3, 2, 1);
            tmplt->setCell(4, 2, 1);
            tmplt->setCell(1, 3, 1);
            tmplt->setCell(2, 3, 1);
            tmplt->setCell(3, 3, 1);
            tmplt->setCell(2, 4, 1);

            fild->setTemplate(tmplt);
        }
            break;
        case ID_VICINITY_MOORE1STORDER:
        {
            Template* tmplt = new Template();

            fild->setTemplate(tmplt);
        }
            break;
        case ID_VICINITY_MOORE2NDORDER:
        {
            Template* tmplt = new Template(5);

            for (size_t i = 0; i < 5; i++)
            {
                for (size_t j = 0; j < 5; j++)
                {
                    if (i == 2 && j == 2)
                        tmplt->setCell(j, i, 0);
                    else
                        tmplt->setCell(j, i, 1);
                }
            }

            fild->setTemplate(tmplt);
        }
            break;
        case ID_VICINITY_SETOWN:
        {
            DialogBox(hInst,
                MAKEINTRESOURCE(IDD_TEMPLATE),
                hWnd,
                DlgProcTemplate
            );
        }
            break;
            // Program
        case ID_PROGRAM_SETTINGS:
        {
            DialogBoxParam(
                hInst,
                MAKEINTRESOURCE(IDD_SETTINGS), 
                hWnd, 
                DlgProcSettings, 
                (LPARAM)&hWnd
            );
        }
            break;
        case ID_PROGRAM_INFO:
        {
            DialogBox(hInst,
                MAKEINTRESOURCE(IDD_INFO),
                hWnd,
                DlgProcInfo
            );
        }
            break;
        case ID_PROGRAM_HELP:
        {
            DialogBox(hInst,
                MAKEINTRESOURCE(IDD_HELP),
                hWnd,
                DlgProcHelp
            );
        }
            break;
        case ID_PROGRAM_ABOUTAUTHOR:
        {
            DialogBox(hInst,
                MAKEINTRESOURCE(IDD_ABOUT),
                hWnd,
                DlgProcAbout
            );
        }
        break;
        default:
            break;
        }
    }
    break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT: 
        {
            POINT buffer = fild->getBasePosition();
            INT size = fild->getSettings()->getSize();

            fild->setBasePosition({ buffer.x - size, buffer.y });

            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;
        case VK_RIGHT:
        {
            POINT buffer = fild->getBasePosition();
            INT size = fild->getSettings()->getSize();

            fild->setBasePosition({ buffer.x + size, buffer.y });

            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;
        case VK_UP:
        {
            POINT buffer = fild->getBasePosition();
            INT size = fild->getSettings()->getSize();

            fild->setBasePosition({ buffer.x, buffer.y - size });

            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;
        case VK_DOWN:
        {
            POINT buffer = fild->getBasePosition();
            INT size = fild->getSettings()->getSize();

            fild->setBasePosition({ buffer.x, buffer.y + size });

            InvalidateRect(hWnd, NULL, TRUE);
        }
            break;
        default:
            break;
        }
        break;
    case WM_LBUTTONDOWN:
    {
        POINT mousePosition;
        writeCursorPos(hWnd, mousePosition);

        if (!fild->pointInRect(mousePosition))
        {
            break;
        }
        
        if (runEvolution)
        {
            break;
        }

        mousePosition = fild->selectedCell(mousePosition);

        if (structID)
        {
            switch (structID)
            {
            case FLASHER_VERTICAL:
            {
                setFlasherVertical(fild->getGrid(), mousePosition);
            }
                break;
            case FLASHER_HORIZONTAL:
            {
                setFlasherHorizontal(fild->getGrid(), mousePosition);
            }
                break;
            case GLIDER_0:
            {
                setGlider0(fild->getGrid(), mousePosition);
            }
                break;
            case GLIDER_90:
            {
                setGlider90(fild->getGrid(), mousePosition);
            }
                break; 
            case GLIDER_180:
            {
                setGlider180(fild->getGrid(), mousePosition);
            }
                break;
            case GLIDER_270:
            {
                setGlider270(fild->getGrid(), mousePosition);
            }
                break;
            case STAR:
            {
                setStar(fild->getGrid(), mousePosition); 
            }
                break;
            default:
                break;
            }
            
            structID = NULL;
        }
        else
        {
            Grid* grid = fild->getGrid();
            grid->setCell(mousePosition, (grid->getCell(mousePosition) ? 0x0 : 0x1));
        }

        InvalidateRect(hWnd, NULL, TRUE);
    }
        break;
    case WM_MOUSEWHEEL:
    {
        Settings* settings = fild->getSettings();

        INT zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        INT buffer = settings->getSize();
        INT sensivity = settings->getMouseWheelSensitivity();

        settings->setSize(buffer + (zDelta / sensivity));

        InvalidateRect(hWnd, NULL, TRUE);
    }
        break;
    case WM_PAINT:
    {
        if (fild != nullptr)
        {
            fild->draw(hWnd);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return FALSE;
}