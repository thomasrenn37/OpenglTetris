#pragma once
#include <vector>
#include <chrono>
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
	void SetMoveDirection(int x, int y);

private:
	void createSides(float xPos);
	void CreateBlock(float xPos, float yPos, float r, float g, float b);
	float GetXPosition(int x);
	float GetYPosition(int y);
	void CreatePiece(const char piece_type);
	void createBottom(float xPos);
	std::vector<float> m_vertices;
	const unsigned int c_NUM_ELEMENTS_PER_VERT = 7;
	std::vector<unsigned int> m_indices;
	int m_numRows;
	int m_numCols;
	float m_RightXCord;
	float m_LeftXCord;
	float m_YCord;
	bool m_ActivePiece;
	std::chrono::system_clock::time_point m_timer;

	ShaderProgram m_shaderProg;
	GLuint m_bufferHandle, m_vao, m_ebo;
	float m_block_length;

	void Move();
	unsigned int m_currentPieceIndex;

	int m_moveX;
	int m_moveY;
	float m_moveSpeed;
};
