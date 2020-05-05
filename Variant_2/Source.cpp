#include <Windows.h>
#include <tchar.h>

#define Theme CreateSolidBrush(RGB(135, 206, 250)) //светло-голубой цвет

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdShow, int nCmdShow)
{
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	wc.hbrBackground = Theme;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	TCHAR ClassName[] = _T("MyClass");
	wc.lpszClassName = ClassName;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, _T("Класс не зарегистрирован"), _T("Ошибка!"), MB_OK);
		return 0;
	}

	HWND hWnd = CreateWindow(ClassName, _T("Лабораторная работа №1"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBox(NULL, _T("Окно не создано"), _T("Ошибка!"), MB_OK);
		return 0;
	}
	else
	{
		ShowWindow(hWnd, nCmdShow);
		MessageBox(hWnd, _T("В главное окно светло-голубого цвета выводится желтый круг диаметром 200 пикселов, при каждом нажатии ЛКМ слева выедается часть круга, аналогично ПКМ"),
			_T("Условие"), MB_OK);
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static PAINTSTRUCT ps;

	static RECT left_circle; //окружность для левого съедания
	static RECT main_circle; //главная окружность
	static RECT right_circle; //окружность для правого съедания

	//static RECT wndInfo;//информация о расположении окна

	const int diameter = 200; //диаметр окружности
	const int step = 50; //шаг откусывания
	const int border = diameter / 2; //граница откусывания
	POINT centr; //центр клиентской области

	static int cbLeft = 0, cbRight = 0; //счетчики для перерисовки при изменении размеров окна

	switch (msg)
	{

	case WM_SIZE:
		centr.x = LOWORD(lParam) / 2;
		centr.y = HIWORD(lParam) / 2;

		main_circle.left = centr.x - diameter / 2;
		main_circle.top = centr.y - diameter / 2;
		main_circle.right = main_circle.left + diameter;
		main_circle.bottom = main_circle.top + diameter;

		left_circle.left = cbLeft * step + main_circle.left - diameter;
		left_circle.top = main_circle.top;
		left_circle.right = cbLeft * step + main_circle.left;
		left_circle.bottom = main_circle.bottom;

		right_circle.left = -cbRight * step + main_circle.right;
		right_circle.top = main_circle.top;
		right_circle.right = -cbRight * step + main_circle.right + diameter;
		right_circle.bottom = main_circle.bottom;

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		SelectObject(hdc, GetStockObject(NULL_PEN));//позволяет убрать обводку

		SelectObject(hdc, CreateSolidBrush(RGB(255, 255, 0)));
		Ellipse(hdc, main_circle.left, main_circle.top, main_circle.right, main_circle.bottom);

		SelectObject(hdc, Theme);
		Ellipse(hdc, left_circle.left, left_circle.top, left_circle.right, left_circle.bottom);
		Ellipse(hdc, right_circle.left, right_circle.top, right_circle.right, right_circle.bottom);

		EndPaint(hWnd, &ps);

		break;

	case WM_LBUTTONDOWN:
		if (left_circle.right + step <= main_circle.left + border)
		{
			cbLeft++;
			left_circle.left += step;
			left_circle.right += step;
		}
		else
			MessageBox(hWnd, _T("Левая часть съедена"), _T(""), MB_OK);
		InvalidateRect(hWnd, NULL, true);
		break;

	case WM_RBUTTONDOWN:
		if (right_circle.left - step >= main_circle.left + border)
		{
			cbRight++;
			right_circle.left -= step;
			right_circle.right -= step;
		}
		else
			MessageBox(hWnd, _T("Правая часть съедена"), _T(""), MB_OK);
		InvalidateRect(hWnd, NULL, true);
		break;


	case WM_DESTROY: PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}