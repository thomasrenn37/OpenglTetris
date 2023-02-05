#include "board.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <math.h>

#define GLEW_STATIC // Need to define to be able to statically link.
#include <glew.h>
#include <glfw3.h>

#include "Texture.h"
#include "shader.h"

Board::Board(int width, int height)
{
	// Set each block to be unoccupied.
	for (int i = 0; i < m_numRows; i++)
	{
		for (int j = 0; j < m_numCols; j++)
		{
			m_occupiedBlocks[i][j] = false;
		}
	}


	m_block_length = 2.f / (m_numRows + 1);
	m_LeftXCord = -5.0f * m_block_length;
	m_RightXCord = 6.0f * m_block_length;

	// Set up the vectors that will be used with the opengl buffers
	m_vertices = std::vector<float>();
	m_indices = std::vector<unsigned int>();

	m_moveX = 0;
	m_moveY = 0;
	m_ActivePiece = false;
	m_FlipPiece = false;

	// Create the board of tetris.
	this->createSides(m_LeftXCord);
	this->createSides(m_RightXCord);
	this->createBottom(m_LeftXCord + m_block_length);

	/* ---------- Generate the handles to the opengl objects ------------ */
	
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_bufferHandle);
	glGenBuffers(1, &m_ebo);

	// Set up the vertex array and bind the buffers and data to it.
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferHandle);

	// Set up the vertex buffer and index buffer to store vertex data.
	glNamedBufferData(m_bufferHandle, sizeof(float) * numVertices(), getVertexPointer(), GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices(), getIndexPointer(), GL_STREAM_DRAW);

	
	// Enable the position data for the screen position, texture position and color.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * c_NUM_ELEMENTS_PER_VERT, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * c_NUM_ELEMENTS_PER_VERT, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * c_NUM_ELEMENTS_PER_VERT, (void*)(sizeof(float) * 4));
	glEnableVertexAttribArray(2);

	// Generate the texture for the current context.
	unsigned int texture = Texture::generate2DTexture("resources/base_tetris_block.png");


	m_shaderProg = ShaderProgram();

	m_shaderProg.addShader("shaders/vertex.glsl", SHADER_TYPE::VERTEX_SHADER);
	m_shaderProg.addShader("shaders/frag.glsl", SHADER_TYPE::FRAGMENT_SHADER);
	m_shaderProg.Link();
	m_shaderProg.use();

	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(m_shaderProg.program(), "blockTexture"), 0);
}

Board::~Board()
{
	// Unbinds the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Board::Render()
{
	Move();
	m_shaderProg.use();
	glUniform1i(glGetUniformLocation(m_shaderProg.program(), "blockTexture"), 0);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, numVertices(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	if (!m_ActivePiece)
		SpawnPiece();
}

void Board::createBottom(float xPos)
{
	for (int i = 0; i < m_numCols; i++)
	{
		CreateBlock(xPos + (i * m_block_length), -1.0f + m_block_length, 0.5f, 0.5f, 0.5f);
	}
}



void Board::Move()
{
	if (m_currentPieceIndex == 0)
	{
		m_moveX = 0;
		m_moveY = 0;

		return;
	}

	// Add downwards momentum if we have elapsed 0.5 s.
	std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - m_timer;
	if (elapsed_seconds.count() > 0.5f)
	{
		m_moveY = -1.0f + m_moveY;
		m_timer = std::chrono::system_clock::now();
	}

	// Move in the x direction if necessary.
	if (m_moveX != 0)
	{
		bool illegalMove = false;
		for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += (c_NUM_ELEMENTS_PER_VERT * 4))
		{
			float newMove = m_vertices[i] + (m_block_length * m_moveX);
			
			// Make sure the move is not out of bounds and there are not already pieces there.
			if (newMove < GetXPosition(0) || newMove > GetXPosition(m_numCols - 1))
			{
				illegalMove = true;
				break;
			}
			else if (m_occupiedBlocks[GetYIndex(m_vertices[i + 1])][GetXIndex(newMove)])
			{
				illegalMove = true;
			}

		}

		// Move the piece in the x axis if the move is legal.
		if (!illegalMove) 
		{
			for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += c_NUM_ELEMENTS_PER_VERT)
			{
				m_vertices[i] += m_block_length * m_moveX;
			}
		}

		m_moveX = 0;
	}

	// Move in the y direction if needed.
	if (m_moveY != 0)
	{
		bool illegalMove = false;

		for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += (c_NUM_ELEMENTS_PER_VERT * 4))
		{
			float newY = m_vertices[i + 1] + (m_block_length * m_moveY);
			
			// Check to see if the next row is the last row or there is already a piece on that row.
			if (newY < GetYPosition(m_numRows - 1))
			{
				illegalMove = true;
			}
			else if (m_occupiedBlocks[GetYIndex(newY)][GetXIndex(m_vertices[i])])
			{
				illegalMove = true;
			}
		}

		if (!illegalMove)
		{
			// Move the piece along the y axis.
			for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += c_NUM_ELEMENTS_PER_VERT)
			{
				m_vertices[i + 1] += m_block_length * m_moveY;
			}
			m_moveY = 0;
		}

		if (illegalMove)
		{
			// Occupy the spaces in the board so the player can no longer go there.
			for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += (c_NUM_ELEMENTS_PER_VERT * 4))
			{
				m_occupiedBlocks[GetYIndex(m_vertices[i + 1])][GetXIndex(m_vertices[i])] = true;
			}

			m_ActivePiece = false;
			m_currentPieceIndex = 0;
		}
		
		m_moveY = 0;
	}


	// Flip the piece
	if (m_FlipPiece && m_ActivePiece)
	{
		bool illegalMove = false;

		// Get the second block and calculate the origin in terms of the origin
		int start = c_NUM_ELEMENTS_PER_VERT * 4 + m_currentPieceIndex;

		// Use the first 3 vertices to calculate origin, first x0 - x1, then y1 - y2
		// Get the roation x and y origins.
		float block_origin[2];
		block_origin[0] = m_vertices[start];
		block_origin[1] = m_vertices[start + 1];
		
		// 4 vertieces * 2 vertex data * 4 number of blocks per piece
		float new_verts[32] = { 0 };

		int j = 0;
		for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += c_NUM_ELEMENTS_PER_VERT)
		{
			float prev_x = m_vertices[i] - block_origin[0];
			float prev_y = m_vertices[i + 1] - block_origin[1];
			
			float new_x = (-prev_y  + block_origin[0]);
			float new_y = (prev_x + block_origin[1]);
			
			new_verts[j] = new_x;
			new_verts[j + 1] = new_y;

			// Check to see if the player is in bounds.
			if (GetXIndex(new_verts[j]) < 0 || GetXIndex(new_verts[j]) > (m_numCols - 1))
			{
				illegalMove = true;
				break;
			}
			else if (GetYIndex(new_verts[j + 1]) > (m_numRows))
			{
				illegalMove = true;
				break;
			}
			
			// Check if it will hit any other pieces.
			if (m_vertices[i + 1] < GetYPosition(m_numRows - 1))
			{
				illegalMove = true;
			}

			j += 2;
		}


		if (!illegalMove)
		{
			// Rotation counter-clockwise messes up the drawing order of the vertices,
			// and therefore affects how the blocks are located with the GetIndex functions.
			// Re-arrange the blocks on how they are drawn
			for (int block = 0; block < 4; block++)
			{
				float temp[2];

				// Assume 0-based indexing in the new vertex array.
				// Need to sort the new array in this new order:
				// 1 -> 0,  3 -> 1, 0 -> 2, 2 -> 3

				// 1 -> 0 
				temp[0] = new_verts[block * 8];
				temp[1] = new_verts[block * 8 + 1];
				new_verts[block * 8] = new_verts[block * 8 + (2 * 1)];
				new_verts[block * 8 + 1] = new_verts[block * 8 + (2 * 1) + 1];

				// 3 -> 1
				new_verts[block * 8 + (2 * 1)] = new_verts[block * 8 + (2 * 3)];
				new_verts[block * 8 + (2 * 1) + 1] = new_verts[block * 8 + (2 * 3) + 1];

				// 2 -> 3
				new_verts[block * 8 + (2 * 3)] = new_verts[block * 8 + (2 * 2)];
				new_verts[block * 8 + (2 * 3) + 1] = new_verts[block * 8 + (2 * 2) + 1];

				// 0 -> 2
				new_verts[block * 8 + (2 * 2)] = temp[0];
				new_verts[block * 8 + (2 * 2) + 1] = temp[1];
			}

			// Assing the new vertices to the vertex buffer.
			j = 0;
			for (int i = m_currentPieceIndex; i < (m_vertices.size()); i += c_NUM_ELEMENTS_PER_VERT)
			{
				m_vertices[i] = new_verts[j];
				m_vertices[i + 1] = new_verts[j + 1];
				j += 2;
			}
		}

		m_FlipPiece = false;
	}


	// Check if there is a full line to erase
	for (int i = 0; i < m_numRows; i++)
	{
		bool deleteRow = true;
		for (int j = 0; j < m_numCols; j++)
		{
			if (!m_occupiedBlocks[i][j])
			{
				deleteRow = false;
				break;
			}
		}

		if (deleteRow)
		{
			// Delete the row and shift the rest of the rows down.
		}
	}

	glNamedBufferData(m_bufferHandle, sizeof(float) * numVertices(), getVertexPointer(), GL_STREAM_DRAW);
}


void Board::PrintOccupied()
{
	
	for (int i = 0; i < m_numRows; i++)
	{
		for (int j = 0; j < m_numCols; j++)
		{
			std::cout << (m_occupiedBlocks[i][j] ? "   " : " x ");
		}
		std::cout << std::endl;
	}

	std::cout << "\n================================\n";
	m_ActivePiece = false;
	m_currentPieceIndex = 0;
}


void Board::createSides(float xPos)
{
	for (int i = 0; i < (m_numRows + 1); i++)
	{
		CreateBlock(xPos, 1.0f - (i * m_block_length), 0.5f, 0.5f, 0.5f);
	}
}

void Board::CreateBlock(float xPos, float yPos, float r, float g, float b)
{
	unsigned int vert_idx = m_vertices.size() / c_NUM_ELEMENTS_PER_VERT;

	// Update the Vertex Buffer
	// Top left
	m_vertices.push_back(xPos);								// x-pos
	m_vertices.push_back(yPos);								// y-pos
	m_vertices.push_back(0.0f);								// s-pos
	m_vertices.push_back(1.0f);								// t-pos
	m_vertices.push_back(r);
	m_vertices.push_back(g);
	m_vertices.push_back(b);

	// Top right
	m_vertices.push_back(xPos + m_block_length);			// x-pos
	m_vertices.push_back(yPos);								// y-pos
	m_vertices.push_back(1.0f);								// s-pos
	m_vertices.push_back(1.0f);								// t-pos
	m_vertices.push_back(r);
	m_vertices.push_back(g);
	m_vertices.push_back(b);

	// Bottom Left
	m_vertices.push_back(xPos);								// x-pos
	m_vertices.push_back(yPos - m_block_length);			// y-pos
	m_vertices.push_back(0.0f);								// s-pos
	m_vertices.push_back(0.0f);								// t-pos
	m_vertices.push_back(r);
	m_vertices.push_back(g);
	m_vertices.push_back(b);

	// Bottom right
	m_vertices.push_back(xPos + m_block_length);			// x-pos
	m_vertices.push_back(yPos - m_block_length);			// y-pos
	m_vertices.push_back(1.0f);								// s-pos
	m_vertices.push_back(0.0f);								// t-pos
	m_vertices.push_back(r);
	m_vertices.push_back(g);
	m_vertices.push_back(b);


	// Update the Index buffer
	// First triangle
	m_indices.push_back(vert_idx);
	m_indices.push_back(vert_idx + 1);
	m_indices.push_back(vert_idx + 2);

	// Second Triangle
	m_indices.push_back(vert_idx + 1);
	m_indices.push_back(vert_idx + 2);
	m_indices.push_back(vert_idx + 3);
}

float Board::GetXPosition(int x)
{
	return (m_LeftXCord + m_block_length) + (x * m_block_length);
}

float Board::GetYPosition(int y)
{
	return 1.0f - (y * m_block_length);
}

unsigned int Board::GetXIndex(float x)
{
	return static_cast<unsigned int>(round((x - m_LeftXCord -  m_block_length) / m_block_length));
}

unsigned int Board::GetYIndex(float y)
{
	// Need to round to not loose data from converting to unsinged int occasionally.
	return  static_cast<unsigned>(round((y - 1.0f) / (-m_block_length)));
}

void Board::CreatePiece(const char piece_type)
{

	switch (piece_type)
	{
	case 'O':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(0);

		CreateBlock(spawnStartX, spawnStartY, 1.0f, 1.0f, 0.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY, 1.0f, 1.0f, 0.0f);
		CreateBlock(spawnStartX, spawnStartY - m_block_length, 1.0f, 1.0f, 0.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY - m_block_length, 1.0f, 1.0f, 0.0f);
		break;
	}
	case 'I':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(0);
		
		CreateBlock(spawnStartX - m_block_length, spawnStartY, 0.0f, 1.0f, 1.0f);
		CreateBlock(spawnStartX, spawnStartY, 0.0f, 1.0f, 1.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY, 0.0f, 1.0f, 1.0f);
		CreateBlock(spawnStartX + (2 * m_block_length), spawnStartY, 0.0f, 1.0f, 1.0f);
		break;
	}
	case 'J':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);
		
		CreateBlock(spawnStartX - m_block_length, spawnStartY + m_block_length, 0.0f, 0.0f, 1.0f);
		CreateBlock(spawnStartX - m_block_length, spawnStartY, 0.0f, 0.0f, 1.0f);
		CreateBlock(spawnStartX, spawnStartY, 0.0f, 0.0f, 1.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY, 0.0f, 0.0f, 1.0f);
		break;
	}
	case 'L':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY, 1.0f, 0.5f, 0.0f);
		CreateBlock(spawnStartX, spawnStartY, 1.0f, 0.5f, 0.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY, 1.0f, 0.5f, 0.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY + m_block_length, 1.0f, 0.5f, 0.0f);
		break;
	}
	case 'S':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY, 0.0f, 1.0f, 0.0f);
		CreateBlock(spawnStartX, spawnStartY, 0.0f, 1.0f, 0.0f);
		CreateBlock(spawnStartX, spawnStartY + m_block_length, 0.0f, 1.0f, 0.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY + m_block_length, 0.0f, 1.0f, 0.0f);
		break;
	}
	case 'T':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY, 0.5f, 0.0f, 0.5f);
		CreateBlock(spawnStartX, spawnStartY, 0.5f, 0.0f, 0.5f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY, 0.5f, 0.0f, 0.5f);
		CreateBlock(spawnStartX, spawnStartY + m_block_length, 0.5f, 0.0f, 0.5f);
		break;
	}
	case 'Z':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY + m_block_length, 1.0f, 0.0f, 0.0f);
		CreateBlock(spawnStartX, spawnStartY + m_block_length, 1.0f, 0.0f, 0.0f);
		CreateBlock(spawnStartX, spawnStartY, 1.0f, 0.0f, 0.0f);
		CreateBlock(spawnStartX + m_block_length, spawnStartY, 1.0f, 0.0f, 0.0f);
		break;
	}
	}


}

void Board::SpawnPiece()
{
	int numBlocks = 4;
	int blockVerts = 4;
	int numInsert = numBlocks * blockVerts * 4;

	m_currentPieceIndex = m_vertices.size();

	char pieces[] = "OIJLSTZ";
	srand(time(NULL));
	CreatePiece(pieces[rand() % 7]);
	
	// Swap out the new data.
	glNamedBufferData(m_bufferHandle, sizeof(float) * numVertices(), getVertexPointer(), GL_STREAM_DRAW);
	glNamedBufferData(m_ebo, sizeof(unsigned int) * numIndices(), getIndexPointer(), GL_STREAM_DRAW);

	m_ActivePiece = true;

	static auto m_time = std::chrono::system_clock::now();
}

size_t Board::numVertices()
{
	return m_vertices.size();
}

size_t Board::numIndices()
{
	return m_indices.size();
}

void Board::SetMoveDirection(int x, int y)
{
	m_moveX = x;
	m_moveY = y;
}

void Board::Flip()
{
	m_FlipPiece = true;
}

float* Board::getVertexPointer()
{
	return &m_vertices[0];
}

unsigned int* Board::getIndexPointer()
{
	return &m_indices[0];
}