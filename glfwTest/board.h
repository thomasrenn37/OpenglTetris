#pragma once
#include <vector>
#include "shader.h"

class Board
{
public:
	Board(int width, int height);
	~Board();
	void Render();
	void SpawnPiece();
	float* getVertexPointer();
	unsigned int* getIndexPointer();
	size_t numVertices();
	size_t numIndices();

private:
	void createSides(float xPos);
	void CreateBlock(float xPos, float yPos);
	float GetXPosition(int x);
	float GetYPosition(int y);
	void CreatePiece(const char piece_type);
	void createBottom(float xPos);
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;
	int m_numRows;
	int m_numCols;
	float m_RightXCord;
	float m_LeftXCord;
	float m_YCord;
	ShaderProgram m_shaderProg;
	GLuint m_bufferHandle, m_vao, m_ebo;
	float m_block_length;
};
