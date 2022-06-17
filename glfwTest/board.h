#pragma once
#include <vector>

class Board
{
public:
	Board(int width, int height);
	float* getVertexPointer();
	unsigned int* getIndexPointer();
	int numVertices();
	int numIndices();

private:
	void createSides(unsigned int start_vert_idx, unsigned int start_ind_idx, float block_length, float xPos);
	void createBottom(unsigned int start_vertex_idx, unsigned int start_ind_idx, float block_length, float xPos);
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;
	int m_numRows;
	int m_numCols;
	float m_XCord;
	float m_YCord;
};
