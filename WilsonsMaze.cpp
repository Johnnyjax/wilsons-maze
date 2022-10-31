#include <stack>
#include <numeric>
using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class WilsonsMaze : public olc::PixelGameEngine {
public:
	WilsonsMaze() {
		sAppName = "Wilson\'s Maze";
	}

private:
	int m_nMazeWidth;
	int m_nMazeHeight;
	int* m_maze;

	enum {
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
		CELL_SCANNED = 0x20,
	};

	vector<int> m_UnvisitedCells;

	stack<pair<int, int>> m_scanned_cell_stack;
	int vectorSize;
	int m_nPathWidth;

public:
	template<typename T>
	void remove(std::vector<T>& v, const T& target)
	{
		for (auto it = std::begin(v); it != std::end(v); )
		{
			if (*it == target)
				it = v.erase(it);
			else
				++it;
		}
	}

	bool OnUserCreate() override{

		// Maze parameters
		m_nMazeWidth = 40;
		m_nMazeHeight = 25;
		m_nPathWidth = 3;

		m_maze = new int[m_nMazeWidth * m_nMazeHeight];
		memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

		m_UnvisitedCells.resize(m_nMazeWidth * m_nMazeHeight);
		std::iota(m_UnvisitedCells.begin(), m_UnvisitedCells.end(), 0);

		int randomWidth = rand() % m_nMazeWidth - 1;
		int randomHeight = rand() % m_nMazeHeight - 1;

		m_scanned_cell_stack.push(make_pair(randomWidth,randomHeight ));
		m_maze[randomHeight * m_nMazeWidth + randomWidth] = CELL_SCANNED;
		m_maze[128] = CELL_VISITED;
		//remove(m_UnvisitedCells, 128);
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override { 
		//this_thread::sleep_for(500ms);
		auto offset = [&](int x, int y) {
			return (m_scanned_cell_stack.top().second + y) * m_nMazeWidth + (m_scanned_cell_stack.top().first + x);
		};

		// Do Maze Algorithm
		if (m_UnvisitedCells.size() > 1) {
			// Step 1: Create a set of the unvisited neighbours

			vector<int> neighbours;

			// North Neighbour
			if ((m_scanned_cell_stack.top().second > 0)) neighbours.push_back(0);
			// East Neighbour
			if ((m_scanned_cell_stack.top().first < m_nMazeWidth - 1)) neighbours.push_back(1);
			// South Neighbour
			if ((m_scanned_cell_stack.top().second < m_nMazeHeight - 1)) neighbours.push_back(2);
			// West Neighbour
			if ((m_scanned_cell_stack.top().first > 0)) neighbours.push_back(3);

			// Are there any neighbours available?
			if (!neighbours.empty()) {
				// Choose one available neighbour at random
				int next_cell_dir = neighbours[(rand() % neighbours.size())];

				// Create a path between the neighbour and the current cell
				switch (next_cell_dir) {
				case 0: // North
					m_maze[offset(0, 0)] |= CELL_PATH_N;
					m_maze[offset(0, -1)] |= CELL_PATH_S;
					m_scanned_cell_stack.push(make_pair((m_scanned_cell_stack.top().first + 0), (m_scanned_cell_stack.top().second - 1)));
					break;
				case 1: // East
					m_maze[offset(0, 0)] |= CELL_PATH_E;
					m_maze[offset(0, 1)] |= CELL_PATH_W;
					m_scanned_cell_stack.push(make_pair((m_scanned_cell_stack.top().first + 1), (m_scanned_cell_stack.top().second + 0)));
					break;
				case 2: // South
					m_maze[offset(0, 0)] |= CELL_PATH_S;
					m_maze[offset(0, 1)] |= CELL_PATH_N;
					m_scanned_cell_stack.push(make_pair((m_scanned_cell_stack.top().first + 0), (m_scanned_cell_stack.top().second + 1)));
					break;
				case 3: // West
					m_maze[offset(0, 0)] |= CELL_PATH_W;
					m_maze[offset(-1, 0)] |= CELL_PATH_E;
					m_scanned_cell_stack.push(make_pair((m_scanned_cell_stack.top().first - 1), (m_scanned_cell_stack.top().second + 0)));
					break;
				}
				if (!(m_maze[offset(0, 0)] & CELL_VISITED)) {
					if (m_maze[offset(0, 0)] & CELL_SCANNED) {
						int repeated_value = offset(0, 0);
						do {
							m_scanned_cell_stack.pop();
							
							if (repeated_value == offset(0, 0)) {
								m_maze[offset(0, 0)] |= CELL_SCANNED;
								if (!(m_maze[offset(0, 1)] & CELL_SCANNED))
									m_maze[offset(0, 0)] &= ~(CELL_PATH_S);
								if (!(m_maze[offset(1, 0)] & CELL_SCANNED))
									m_maze[offset(0, 0)] &= ~(CELL_PATH_E);
								break;
							}
							m_maze[offset(0, 0)] &= ~(CELL_SCANNED | CELL_PATH_E | CELL_PATH_S);
						} while (repeated_value != offset(0, 0));
						
					}
					else {
						m_maze[offset(0, 0)] |= CELL_SCANNED;
					}
				}
				else {
					m_scanned_cell_stack.pop();
					while (!(m_scanned_cell_stack.empty())) {
						m_maze[offset(0, 0)] &= ~(CELL_SCANNED);
						m_maze[offset(0, 0)] |= CELL_VISITED;
						remove(m_UnvisitedCells, offset(0, 0));
						m_scanned_cell_stack.pop();
					}
					int randomPos;
					if (m_UnvisitedCells.size() > 1) 
						randomPos = (rand() % (m_UnvisitedCells.size() - 1));
					else
						randomPos = 0;

					vectorSize = m_UnvisitedCells.size();
					int visitedCell = m_UnvisitedCells[randomPos];
					m_scanned_cell_stack.push(make_pair(m_UnvisitedCells[randomPos] % m_nMazeWidth, m_UnvisitedCells[randomPos]/ m_nMazeWidth));
					m_maze[offset(0, 0)] |= CELL_SCANNED;
				}
			}
		}

		// ====== DRAWING STUFF =======

		// Clear screen
		Clear(olc::BLACK);

		// Draw Maze
		for (int x = 0; x < m_nMazeWidth; x++) {
			for (int y = 0; y < m_nMazeHeight; y++) {
				for (int py = 0; py < m_nPathWidth; py++) {
					for (int px = 0; px < m_nPathWidth; px++) {
						if (m_maze[y * m_nMazeWidth + x] & CELL_VISITED)
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::WHITE);
						else if (m_maze[y * m_nMazeWidth + x] & CELL_SCANNED) {
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::RED);
						}
						else
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::BLUE);
					}
				}

				if (!(m_maze[y * m_nMazeWidth + x] & CELL_SCANNED)) {
					for (int p = 0; p < m_nPathWidth; p++) {
						if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_S)
							Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth, olc::WHITE);
						if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_E)
							Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p, olc::WHITE);
					}
				}
				else {
					for (int p = 0; p < m_nPathWidth; p++) {
						if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_S)
							Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth, olc::RED);
						if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_E)
							Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p, olc::RED);
					}
				}
			}
		}

		// Draw unit - the top of the stack
		for (int py = 0; py < m_nPathWidth; py++)
			for (int px = 0; px < m_nPathWidth; px++)
				Draw(m_scanned_cell_stack.top().first * (m_nPathWidth + 1) + px, m_scanned_cell_stack.top().second * (m_nPathWidth + 1) + py, olc::GREEN); // Draw Cell



		return true;
	}
};


int main() {
	WilsonsMaze maze;
	if (maze.Construct(160, 100, 5, 5))
		maze.Start();
	return 0;
}