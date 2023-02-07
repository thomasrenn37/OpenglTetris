#pragma once
#include <vector>
#include <array>
#include <chrono>
#include "shader.h"


class Board
{
public:
	Board(int width, int height);
	~Board();
	void Update();
	void Render();
	void SpawnPiece();
	float* getVertexPointer();
	unsigned int* getIndexPointer();
	unsigned int numVertices();
	size_t numIndices();
	void SetMoveDirection(int x, int y);
	void Flip();
	void Drop();

private:
	void createSides(float xPos);
	void CreateBlock(float xPos, float yPos, float r, float g, float b);
	float GetXPosition(int x);
	float GetYPosition(int y);
	unsigned int GetXIndex(float x);
	unsigned int GetYIndex(float y);
	void CreatePiece(const char piece_type);
	void createBottom(float xPos);
	std::vector<float> m_vertices;
	static constexpr unsigned int c_NUM_ELEMENTS_PER_VERT = 7;
	std::vector<unsigned int> m_indices;
	static constexpr unsigned int m_numRows = 20;
	static constexpr unsigned int m_numCols = 10;
	std::array<std::array< bool, m_numCols>, m_numRows> m_occupiedBlocks; // rows x columns
	float m_RightXCord;
	float m_LeftXCord;
	
	bool m_ActivePiece;
	bool m_FlipPiece;
	std::chrono::system_clock::time_point m_timer;

	ShaderProgram m_shaderProg;
	GLuint m_bufferHandle, m_vao, m_ebo;
	float m_block_length;

	void DeleteRow(unsigned int row);
	void PrintOccupied();
	unsigned int m_currentPieceIndex;
	unsigned int m_firstPieceIndex;

	int m_moveX;
	int m_moveY;
	float m_moveSpeed;
};
