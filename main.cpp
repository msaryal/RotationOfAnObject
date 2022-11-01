#define _WIN32_WINNT 0x0500
#include<iostream>
#include<cmath>
#include <windows.h>
using namespace std;

int WIDTH;
int HEIGHT;

struct Cube
{
    float Dots[8][4];
};
//Отрисовка куба
void DrawCube(Cube * Cube3D, int colorRed, int colorGreen, int colorBlue)
{
    POINT op;
    HWND hWnd = GetConsoleWindow();
    HDC hDc = GetDC(hWnd);
    SetDCPenColor(hDc, RGB(colorRed, colorGreen, colorBlue));
    SelectObject(hDc, GetStockObject(DC_PEN));
    MoveToEx(hDc, WIDTH + (int)Cube3D->Dots[0][0], HEIGHT - (int)Cube3D->Dots[0][1], &op);
    for(int i=0;i<8;i++)
    LineTo(hDc, WIDTH + (int)Cube3D->Dots[i][0], HEIGHT - (int)Cube3D->Dots[i][1]);
    MoveToEx(hDc, WIDTH + (int)Cube3D->Dots[1][0], HEIGHT - (int)Cube3D->Dots[1][1], &op);
    LineTo(hDc, WIDTH + (int)Cube3D->Dots[6][0], HEIGHT - (int)Cube3D->Dots[6][1]);
    MoveToEx(hDc, WIDTH + (int)Cube3D->Dots[3][0], HEIGHT - (int)Cube3D->Dots[3][1], &op);
    LineTo(hDc, WIDTH + (int)Cube3D->Dots[0][0], HEIGHT - (int)Cube3D->Dots[0][1]);
    LineTo(hDc, WIDTH + (int)Cube3D->Dots[7][0], HEIGHT - (int)Cube3D->Dots[7][1]);
    LineTo(hDc, WIDTH + (int)Cube3D->Dots[4][0], HEIGHT - (int)Cube3D->Dots[4][1]);
    MoveToEx(hDc, WIDTH + (int)Cube3D->Dots[5][0], HEIGHT - (int)Cube3D->Dots[5][1], &op);
    LineTo(hDc, WIDTH + (int)Cube3D->Dots[2][0], HEIGHT - (int)Cube3D->Dots[2][1]);
}
//Отрисовка оси
void DrawAxis(float x0, float y0, float x1, float y1, int colorRed, int colorGreen, int colorBlue)
{
    POINT op;
    HWND hWnd = GetConsoleWindow();
    HDC hDc = GetDC(hWnd);
    SetDCPenColor(hDc, RGB(colorRed, colorGreen, colorBlue));
    SelectObject(hDc, GetStockObject(DC_PEN));
    MoveToEx(hDc, WIDTH + (int)x0, HEIGHT - (int)y0, &op);
    LineTo(hDc, WIDTH + (int)x1, HEIGHT - (int)y1);
}
//Обнуление матрицы 2x4
void MZero(float(*A)[4])
{
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 4; j++)
            A[i][j] = 0;
}
//Инициализвация куба
void CubeInit(Cube* Cube3D)
{
    float A[8][4] = { 400,100,200,1,
    600, 100, 200, 1,
    600, 300, 200, 1,
    400, 300, 200, 1,
    400, 300, 400, 1,
    600, 300, 400, 1,
    600, 100, 400, 1,
    400, 100, 400, 1, };
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++)
            Cube3D->Dots[i][j] = A[i][j];
}
//Умножение матрицы координат оси на матрицу поворота
void AxisMultiM(float(*Axis)[4], float(*A)[4])
{
    float C[2][4] = { 0 };
    int i, j, g;
    for (g = 0; g < 4; g++)
        for (i = 0; i < 2; i++)
            for (j = 0; j < 4; j++)
                C[i][j] += Axis[i][g] * A[g][j];
    for (i = 0; i < 2; i++)
        for (j = 0; j < 4; j++)
            Axis[i][j] = C[i][j];
}
//Умножение матрицы точек куба на матрицу поворота
void XMultiM(Cube* Cube3D, float(*A)[4])
{
    float C[8][4] = { 0 };
    int i, j, g;
    for (g = 0; g < 4; g++)
        for (i = 0; i < 8; i++)
            for (j = 0; j < 4; j++)
                C[i][j] += Cube3D->Dots[i][g] * A[g][j];
    for (i = 0; i < 8; i++)
        for (j = 0; j < 4; j++)
            Cube3D->Dots[i][j] = C[i][j];
}
//Расчет-заполнение обратной матрицы поворота вокруг оси y
void FillRyMinus(float(*A)[4], float d, float cx, float cy, float cz)
{
    A[0][0] = d;
    A[0][1] = 0;
    A[0][2] = -cx;
    A[0][3] = 0;
    A[1][0] = 0;
    A[1][1] = 1;
    A[1][2] = 0;
    A[1][3] = 0;
    A[2][0] = cx;
    A[2][1] = 0;
    A[2][2] = d;
    A[2][3] = 0;
    A[3][0] = 0;
    A[3][1] = 0;
    A[3][2] = 0;
    A[3][3] = 1;
}
//Расчет-заполнение обратной матрицы поворота вокруг оси x
void FillRxMinus(float(*A)[4], float d, float cx, float cy, float cz)
{
    A[0][0] = 1;
    A[0][1] = 0;
    A[0][2] = 0;
    A[0][3] = 0;
    A[1][0] = 0;
    A[1][1] = cz / d;
    A[1][2] = -cy / d;
    A[1][3] = 0;
    A[2][0] = 0;
    A[2][1] = cy / d;
    A[2][2] = cz / d;
    A[2][3] = 0;
    A[3][0] = 0;
    A[3][1] = 0;
    A[3][2] = 0;
    A[3][3] = 1;
}
//Расчет-заполнение матрицы поворота вокруг оси z(роль произвольной оси после комбинаций поворотов вокруг x и y)
void FillRv(float(*Rv)[4], float a)
{
    Rv[0][0] = cos(a);
    Rv[1][1] = cos(a);
    Rv[1][0] = -sin(a);
    Rv[0][1] = sin(a);
    Rv[2][2] = 1.0;
    Rv[3][3] = 1.0;
}
//Расчет-заполнение матрицы поворота вокруг оси y
void FillRy(float(*A)[4], float d, float cx, float cy, float cz)
{
    A[0][0] = d;
    A[0][1] = 0;
    A[0][2] = cx;
    A[0][3] = 0;
    A[1][0] = 0;
    A[1][1] = 1;
    A[1][2] = 0;
    A[1][3] = 0;
    A[2][0] = -cx;
    A[2][1] = 0;
    A[2][2] = d;
    A[2][3] = 0;
    A[3][0] = 0;
    A[3][1] = 0;
    A[3][2] = 0;
    A[3][3] = 1;
}
//Расчет-заполнение матрицы поворота вокруг оси x
void FillRx(float(*A)[4], float d, float cx, float cy, float cz)
{
    A[0][0] = 1;
    A[0][1] = 0;
    A[0][2] = 0;
    A[0][3] = 0;
    A[1][0] = 0;
    A[1][1] = cz / d;
    A[1][2] = cy / d;
    A[1][3] = 0;
    A[2][0] = 0;
    A[2][1] = -cy/d;
    A[2][2] = cz/d;
    A[2][3] = 0;
    A[3][0] = 0;
    A[3][1] = 0;
    A[3][2] = 0;
    A[3][3] = 1;
}
//Расчет-заполнение матрицы переноса
void FillT(float(*A)[4], float x0, float y0, float z0)
{
    A[0][0] = 1;
    A[0][1] = 0;
    A[0][2] = 0;
    A[0][3] = 0;
    A[1][0] = 0;
    A[1][1] = 1;
    A[1][2] = 0;
    A[1][3] = 0;
    A[2][0] = 0;
    A[2][1] = 0;
    A[2][2] = 1;
    A[2][3] = 0;
    A[3][0] = -x0;
    A[3][1] = -y0;
    A[3][2] = -z0;
    A[3][3] = 1;
}
int main()
{
    setlocale(LC_ALL, "Russian");
    Cube* Cube3D = new Cube;
    Cube* Cube3DResult = new Cube;
    //Матрицы осей
    float XAxis[2][4] = { 0 };
    float YAxis[2][4] = { 0 };
    float ZAxis[2][4] = { 0 };
    float UserAxis[2][4] = {0};
    //Матрицы поворотов
    float T[4][4] = { 0 };
    float Rx[4][4] = { 0 };
    float Ry[4][4] = { 0 };
    float Rv[4][4] = { 0 };
    float TMinus[4][4] = { 0 };
    float RxMinus[4][4] = { 0 };
    float RyMinus[4][4] = { 0 };
    float d;
    float a = 180;
    //Пользовательская ось(для расчетов)
    float x1, y1, z1, x0, y0, z0;
    //Координаты нормализованного направляющего вектора пользовательской оси
    float cx, cy, cz;
    int i, j;
    //Размеры экрана
    HDC hDCScreen = GetDC(NULL);
    WIDTH = GetDeviceCaps(hDCScreen, HORZRES);
    HEIGHT = GetDeviceCaps(hDCScreen, VERTRES);
    WIDTH /=2 ;
    HEIGHT /= 2;
    ReleaseDC(NULL, hDCScreen);

    while (1)
    {
        //Заполнение матриц куба данными о точках
        CubeInit(Cube3D);
        CubeInit(Cube3DResult);
        cout << "После ввода данных изменять сцену можно с помощью конопок <Стрелки>" << endl << "Для изменения данных нажмите Shift, для выхода - Escape" << endl;
        cout << "Измененный куб выделен белым цветом" << endl;
        cout << "Задайте ось поворота(6 цифр):" << endl;
        cin >> UserAxis[1][0] >> UserAxis[1][1] >> UserAxis[1][2] >> UserAxis[0][0] >> UserAxis[0][1] >> UserAxis[0][2];
        cout << "Задайте угол поворота:" << endl;
        cin >> a;
        //Обнуление матриц осей
        MZero(XAxis);
        MZero(YAxis);
        MZero(ZAxis);
        //Инициализация матриц осей
        XAxis[0][0] = WIDTH;
        XAxis[0][3] = 1;
        XAxis[1][3] = 1;
        YAxis[0][1] = WIDTH;
        YAxis[0][3] = 1;
        YAxis[1][3] = 1;
        ZAxis[0][2] = WIDTH;
        ZAxis[0][3] = 1;
        ZAxis[1][3] = 1;
        //Данные для расчета направляющего вектора оси
        x0 = UserAxis[1][0];
        y0 = UserAxis[1][1];
        z0 = UserAxis[1][2];
        x1 = UserAxis[0][0];
        y1 = UserAxis[0][1];
        z1 = UserAxis[0][2];
        //Расчет направляющего вектора и нормализация
        cx = (x1 - x0) / sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2) + pow(z1 - z0, 2));
        cy = (y1 - y0) / sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2) + pow(z1 - z0, 2));
        cz = (z1 - z0) / sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2) + pow(z1 - z0, 2));

        d = sqrt(pow(cy, 2) + pow(cz, 2));

        //Расчет-заполнение матриц
        FillT(T, x0, y0, z0);
        FillT(TMinus, -x0, -y0, -z0);

        FillRx(Rx, d, cx, cy, cz);
        FillRxMinus(RxMinus, d, cx, cy, cz);

        FillRy(Ry, d, cx, cy, cz);
        FillRyMinus(RyMinus, d, cx, cy, cz);

        FillRv(Rv, (a / 180) * 3.141592658);

        //Отрисовка изначального куба
        DrawCube(Cube3D, 100, 100, 100);

        //Поворот куба по формуле X =T*Rx*Ry*Rv*Ry^-1*Rx^-1*T^-1
        XMultiM(Cube3DResult, T);
        XMultiM(Cube3DResult, Rx);
        XMultiM(Cube3DResult, Ry);
        XMultiM(Cube3DResult, Rv);
        XMultiM(Cube3DResult, RyMinus);
        XMultiM(Cube3DResult, RxMinus);
        XMultiM(Cube3DResult, TMinus);
        //Отрисовка результата и осей
        DrawCube(Cube3DResult, 255, 255, 255);
        DrawAxis(XAxis[1][0], XAxis[1][1], XAxis[0][0], XAxis[0][1], 255, 0, 0);
        DrawAxis(YAxis[1][0], YAxis[1][1], YAxis[0][0], YAxis[0][1], 0, 255, 0);
        DrawAxis(ZAxis[1][0], ZAxis[1][1], ZAxis[0][0], ZAxis[0][1], 0, 0, 255);
        //Повороты сцены (умножаем все объекты на соответсвующую матрицу поворота(Rx,Rx^-1,Ry или Ry^-1))
        while (1)
        {
            //В скобках указаны готовые значения cos(15) и sin(15), тк cos(a) = cz/d и sin(a) = cy/d
            if (GetAsyncKeyState(0x28))
            {
                FillRx(Rx, 1, 0, 0.2588, 0.9659);
                XMultiM(Cube3D, Rx);
                XMultiM(Cube3DResult, Rx);
                AxisMultiM(XAxis, Rx);
                AxisMultiM(YAxis, Rx);
                AxisMultiM(ZAxis, Rx);
                AxisMultiM(UserAxis, Rx);
                system("cls");
                DrawAxis(XAxis[1][0], XAxis[1][1], XAxis[0][0], XAxis[0][1], 255, 0, 0);
                DrawAxis(YAxis[1][0], YAxis[1][1], YAxis[0][0], YAxis[0][1], 0, 255, 0);
                DrawAxis(ZAxis[1][0], ZAxis[1][1], ZAxis[0][0], ZAxis[0][1], 0, 0, 255);
                DrawAxis(UserAxis[1][0], UserAxis[1][1], UserAxis[0][0], UserAxis[0][1], 255, 255, 255);
                DrawCube(Cube3D, 75, 75, 75);
                DrawCube(Cube3DResult, 255, 255, 255);
            }
            if (GetAsyncKeyState(0x26))
            {
                FillRxMinus(Rx, 1, 0, 0.2588, 0.9659);
                XMultiM(Cube3D, Rx);
                XMultiM(Cube3DResult, Rx);
                AxisMultiM(XAxis, Rx);
                AxisMultiM(YAxis, Rx);
                AxisMultiM(ZAxis, Rx);
                AxisMultiM(UserAxis, Rx);
                system("cls");
                DrawAxis(XAxis[1][0], XAxis[1][1], XAxis[0][0], XAxis[0][1], 255, 0, 0);
                DrawAxis(YAxis[1][0], YAxis[1][1], YAxis[0][0], YAxis[0][1], 0, 255, 0);
                DrawAxis(ZAxis[1][0], ZAxis[1][1], ZAxis[0][0], ZAxis[0][1], 0, 0, 255);
                DrawAxis(UserAxis[1][0], UserAxis[1][1], UserAxis[0][0], UserAxis[0][1], 255, 255, 255);
                DrawCube(Cube3D, 75, 75, 75);
                DrawCube(Cube3DResult, 255, 255, 255);
            }
            if (GetAsyncKeyState(0x25))
            {
                FillRy(Ry, 0.9659, 0.2588, 0, 0);
                XMultiM(Cube3D, Ry);
                XMultiM(Cube3DResult, Ry);
                AxisMultiM(XAxis, Ry);
                AxisMultiM(YAxis, Ry);
                AxisMultiM(ZAxis, Ry);
                AxisMultiM(UserAxis, Ry);
                system("cls");
                DrawAxis(XAxis[1][0], XAxis[1][1], XAxis[0][0], XAxis[0][1], 255, 0, 0);
                DrawAxis(YAxis[1][0], YAxis[1][1], YAxis[0][0], YAxis[0][1], 0, 255, 0);
                DrawAxis(ZAxis[1][0], ZAxis[1][1], ZAxis[0][0], ZAxis[0][1], 0, 0, 255);
                DrawAxis(UserAxis[1][0], UserAxis[1][1], UserAxis[0][0], UserAxis[0][1], 255, 255, 255);
                DrawCube(Cube3D, 75, 75, 75);
                DrawCube(Cube3DResult, 255, 255, 255);
            }
            if (GetAsyncKeyState(0x27))
            {
                FillRyMinus(Ry, 0.9659, 0.2588, 0, 0);
                XMultiM(Cube3D, Ry);
                XMultiM(Cube3DResult, Ry);
                AxisMultiM(XAxis, Ry);
                AxisMultiM(YAxis, Ry);
                AxisMultiM(ZAxis, Ry);
                AxisMultiM(UserAxis, Ry);
                system("cls");
                DrawAxis(XAxis[1][0], XAxis[1][1], XAxis[0][0], XAxis[0][1], 255, 0, 0);
                DrawAxis(YAxis[1][0], YAxis[1][1], YAxis[0][0], YAxis[0][1], 0, 255, 0);
                DrawAxis(ZAxis[1][0], ZAxis[1][1], ZAxis[0][0], ZAxis[0][1], 0, 0, 255);
                DrawAxis(UserAxis[1][0], UserAxis[1][1], UserAxis[0][0], UserAxis[0][1], 255, 255, 255);
                DrawCube(Cube3D, 75, 75, 75);
                DrawCube(Cube3DResult, 255, 255, 255);
            }
            if (GetAsyncKeyState(0x1B))
            {
                system("cls");
                exit(0);
            }
            if (GetAsyncKeyState(0x10))
            {
                system("cls");
                break;
            }
        }
    }
    return 0;
}