#include "board.h"
#include <iostream>

Board::Board(int width, int height)
{
	// Playable area is 10 x 20.
	m_numRows = 20;
	m_numCols = 10;

	m_XCord = -0.5f;
	m_YCord = -1.0f;

	// 4 vertices per squre, each vertex: (x, y, s, t) | total = rows * 4 (vertices) * 4 (datapoints) * 2 (sides)
	int totalVertices = ((m_numRows + 1) * 32) + (m_numCols * 16);
	m_vertices = std::vector<float>(totalVertices);

	m_indices = std::vector<unsigned int>(((m_numRows + 1) * 6 * 2) + (m_numCols * 6));

	float block_length = 2.0f / (m_numRows + 1);

	// Want to go from top down until middle.
	this->createSides(0, 0,block_length, m_XCord);
	this->createSides((m_numRows + 1) * 16, 6 * (m_numRows + 1), block_length, m_XCord + (block_length * (m_numCols + 1)));
	this->createBottom((m_numRows + 1) * 32, 12 * (m_numRows + 1), block_length, m_XCord + block_length);
}

void Board::createBottom(unsigned int start_vert_idx, unsigned int start_ind_idx, float block_length, float xPos)
{
	unsigned int vert_idx = start_vert_idx;
	unsigned int ind_idx = start_ind_idx;

	for (int i = 0; i < m_numCols; i++, vert_idx+=16)
	{
		// Bottom left
		m_vertices[vert_idx] = (block_length * i) + xPos;			// x-pos
		m_vertices[vert_idx + 1] = m_YCord;							// y-pos
		m_vertices[vert_idx + 2] = 0.0f;							// s-pos
		m_vertices[vert_idx + 3] = 0.0f;							// t-pos


		// Bottom right
		m_vertices[vert_idx + 4] = xPos + (block_length * (i + 1));	// x-pos
		m_vertices[vert_idx + 5] = m_YCord;							// y-pos
		m_vertices[vert_idx + 6] = 1.0f;							// s-pos
		m_vertices[vert_idx + 7] = 0.0f;							// t-pos

		// Top right
		m_vertices[vert_idx + 8] = xPos + (block_length * (i + 1));	// x-pos
		m_vertices[vert_idx + 9] = m_YCord + block_length;			// y-pos
		m_vertices[vert_idx + 10] = 1.0f;							// s-pos
		m_vertices[vert_idx + 11] = 1.0f;							// t-pos

		// Top left
		m_vertices[vert_idx + 12] = (block_length * i) + xPos;		// x-pos
		m_vertices[vert_idx + 13] = m_YCord + block_length;;		// y-pos
		m_vertices[vert_idx + 14] = 0.0f;							// s-pos
		m_vertices[vert_idx + 15] = 1.0f;							// t-pos

		// First triangle
		ind_idx = (i * 6) + start_ind_idx;
		int temp = vert_idx / 4;

		m_indices[ind_idx] = temp;
		m_indices[ind_idx + 1] = temp + 1;
		m_indices[ind_idx + 2] = temp + 2;

		// Second Triangle
		m_indices[ind_idx + 3] = temp;
		m_indices[ind_idx + 4] = temp + 2;
		m_indices[ind_idx + 5] = temp + 3;
	}
}

void Board::createSides(unsigned int start_vert_idx, unsigned int start_ind_idx, float block_length, float xPos)
{
	int vert_idx = start_vert_idx;
	int ind_idx = start_ind_idx;
	for (int i = 0; i < (m_numRows + 1); i++, vert_idx += 16)
	{
		// Bottom left
		m_vertices[vert_idx] = xPos;									// x-pos
		m_vertices[vert_idx + 1] = (block_length * i) + m_YCord;		// y-pos
		m_vertices[vert_idx + 2] = 0.0f;								// s-pos
		m_vertices[vert_idx + 3] = 0.0f;								// t-pos


		// Bottom right
		m_vertices[vert_idx + 4] = xPos + block_length;					// x-pos
		m_vertices[vert_idx + 5] = (block_length * i) + m_YCord;		// y-pos
		m_vertices[vert_idx + 6] = 1.0f;								// s-pos
		m_vertices[vert_idx + 7] = 0.0f;								// t-pos

		// Top right
		m_vertices[vert_idx + 8] = xPos + block_length;					// x-pos
		m_vertices[vert_idx + 9] = (block_length * (i + 1)) + m_YCord;	// y-pos
		m_vertices[vert_idx + 10] = 1.0f;								// s-pos
		m_vertices[vert_idx + 11] = 1.0f;								// t-pos

		// Top left
		m_vertices[vert_idx + 12] = xPos;								// x-pos
		m_vertices[vert_idx + 13] = (block_length * (i + 1)) + m_YCord;	// y-pos
		m_vertices[vert_idx + 14] = 0.0f;								// s-pos
		m_vertices[vert_idx + 15] = 1.0f;								// t-pos

		// First triangle
		ind_idx = (i * 6) + start_ind_idx;
		int temp = vert_idx / 4;

		m_indices[ind_idx] = temp;
		m_indices[ind_idx + 1] = temp + 1;
		m_indices[ind_idx + 2] = temp + 2;

		// Second Triangle
		m_indices[ind_idx + 3] = temp;
		m_indices[ind_idx + 4] = temp + 2;
		m_indices[ind_idx + 5] = temp + 3;
	}
}

int Board::numVertices()
{
	return m_vertices.size();
}

int Board::numIndices()
{
	return m_indices.size();
}

float* Board::getVertexPointer()
{
	return &m_vertices[0];
}

unsigned int* Board::getIndexPointer()
{
	return &m_indices[0];
}