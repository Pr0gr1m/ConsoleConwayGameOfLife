#include <iostream>
#include <Windows.h>
#include <thread>
#include <conio.h>
#include <format>
#include <algorithm>
#include <cstring>

constexpr int gridWidth = 50;
constexpr int gridHeight = 50;

#define ind(row,column) ((row) * (gridWidth) + (column)) 

int main()
{
	auto h = GetStdHandle(STD_OUTPUT_HANDLE);
	
	auto setFlicker = [&h](bool visible)
	{
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(h, &info);
		info.bVisible = visible;
		SetConsoleCursorInfo(h, &info);
	};

	constexpr int gridWidth = 50;
	constexpr int gridHeight = 50;

	bool *grid = new bool[gridWidth * gridHeight]();
	bool* newGrid = new bool[gridWidth * gridHeight]();

	int selectedRow = 0, selectedColumn = 0;

	SMALL_RECT windowRect = { 0, 0, gridWidth, gridHeight };
	SetConsoleWindowInfo(h, TRUE, &windowRect);

	setFlicker(true);
	bool run = true;
	while(run)
	{
		if (_kbhit())
		{
			char input = _getch();
			if (input == 'e')
			{
				break;
			}

			switch (input)
			{
				case 'w':
					selectedRow += -1;
					selectedRow = max(selectedRow, 0);
					selectedRow = min(selectedRow, gridWidth - 1);
					break;
				case 's':
					selectedRow -= -1;
					selectedRow = max(selectedRow, 0);
					selectedRow = min(selectedRow, gridWidth - 1);
					break;
				case 'a':
					selectedColumn += -1;
					selectedColumn = max(selectedColumn, 0);
					selectedColumn = min(selectedColumn, gridHeight - 1);
					break;
				case 'd':
					selectedColumn -= -1;
					selectedColumn = max(selectedColumn, 0);
					selectedColumn = min(selectedColumn, gridHeight - 1);
					break;
				case ' ':
					grid[ind(selectedRow, selectedColumn)] = 1;

					COORD coord;
					coord.X = static_cast<SHORT>(selectedColumn);
					coord.Y = static_cast<SHORT>(selectedRow);
					SetConsoleCursorPosition(h, coord);
					std::cout << 'x';

					if (selectedColumn != gridWidth - 1)
						selectedColumn += 1;

					break;
			}

			COORD coord;
			coord.X = static_cast<SHORT>(selectedColumn);
			coord.Y = static_cast<SHORT>(selectedRow);
			SetConsoleCursorPosition(h, coord);
		}
	}

	DWORD mode = 0;
	GetConsoleMode(h, &mode);
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(h, mode);

	setFlicker(false);
	run = true;
	while(run)
	{
		if (_kbhit() && _getch() == 'x')
		{
			run = false;
		}

		for (int row = 0; row < gridWidth; ++row)
		{
			for (int column = 0; column < gridHeight; ++column)
			{
				COORD coord;
				coord.X = static_cast<SHORT>(column);
				coord.Y = static_cast<SHORT>(row);
				SetConsoleCursorPosition(h, coord);

				if (grid[ind(row, column)])
				{
					std::cout << std::format("\x1b[48;2;255;255;255m");
					std::cout << ' ';
				}
				else
				{
					std::cout << std::format("\x1b[48;2;0;0;0m");
					std::cout << ' ';
				}
			}
		}

		for (int row = 0; row < gridWidth; ++row)
		{
			for (int column = 0; column < gridHeight; ++column)
			{
				int aliveNeighbors = 0;
				for (int i = -1; i <= 1; ++i)
				{
					for (int j = -1; j <= 1; ++j)
					{
						if (i == 0 && j == 0) continue;
						int neighborRow = row + i;
						int neighborColumn = column + j;
						if (neighborRow >= 0 && neighborRow < gridWidth &&
							neighborColumn >= 0 && neighborColumn < gridHeight)
						{
							aliveNeighbors += grid[ind(neighborRow, neighborColumn)];
						}
					}
				}

				newGrid[ind(row, column)] = grid[ind(row, column)];

				if (grid[ind(row, column)])
				{
					if (aliveNeighbors < 2 || aliveNeighbors > 3)
					{
						newGrid[ind(row, column)] = false;
					}
				}
				else
				{
					if (aliveNeighbors == 3)
					{
						newGrid[ind(row, column)] = true;
					}
				}
			}
		}

		memcpy(grid, newGrid, sizeof(bool) * gridWidth * gridHeight);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	std::cout << std::format("\x1b[48;2;0;0;0m");

	setFlicker(true);

	delete[] grid;
	delete[] newGrid;

	return 0;
}