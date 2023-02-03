#include "board.h"
#include <iostream>

#define GLEW_STATIC // Need to define to be able to statically link.
#include <glew.h>
#include <glfw3.h>

#include "Texture.h"
#include "shader.h"

Board::Board(int width, int height)
{
	// Playable area is 10 x 20.
	m_numRows = 20;
	m_numCols = 10;

	m_block_length = 2.f / (m_numRows + 1);
	m_LeftXCord = -5.0f * m_block_length;
	m_RightXCord = 6.0f * m_block_length;

	// Set up the vectors that will be used with the opengl buffers
	m_vertices = std::vector<float>();
	m_indices = std::vector<unsigned int>();

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

	
	// Enable the position data for the screen position and the texture position.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	// Generate the texture for the current context.
	unsigned int texture = Texture::generate2DTexture("resources/tetris_block.png");


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
	m_shaderProg.use();
	glUniform1i(glGetUniformLocation(m_shaderProg.program(), "blockTexture"), 0);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, numVertices(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Board::createBottom(float xPos)
{
	for (int i = 0; i < m_numCols; i++)
	{
		CreateBlock(xPos + (i * m_block_length), -1.0f + m_block_length);
	}
}

void Board::createSides(float xPos)
{
	for (int i = 0; i < (m_numRows + 1); i++)
	{
		CreateBlock(xPos, 1.0f - (i * m_block_length));
	}
}

void Board::CreateBlock(float xPos, float yPos)
{
	unsigned int vert_idx = m_vertices.size() / 4;

	// Update the Vertex Buffer
	// Top left
	m_vertices.push_back(xPos);								// x-pos
	m_vertices.push_back(yPos);								// y-pos
	m_vertices.push_back(0.0f);								// s-pos
	m_vertices.push_back(1.0f);								// t-pos

	// Top right
	m_vertices.push_back(xPos + m_block_length);				// x-pos
	m_vertices.push_back(yPos);								// y-pos
	m_vertices.push_back(1.0f);								// s-pos
	m_vertices.push_back(1.0f);								// t-pos

	// Bottom Left
	m_vertices.push_back(xPos);								// x-pos
	m_vertices.push_back(yPos - m_block_length);			// y-pos
	m_vertices.push_back(0.0f);								// s-pos
	m_vertices.push_back(0.0f);								// t-pos

	// Bottom right
	m_vertices.push_back(xPos + m_block_length);			// x-pos
	m_vertices.push_back(yPos - m_block_length);			// y-pos
	m_vertices.push_back(1.0f);								// s-pos
	m_vertices.push_back(0.0f);								// t-pos


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

void Board::CreatePiece(const char piece_type)
{
	switch (piece_type)
	{
	case 'O':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(0);

		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY - m_block_length);
		CreateBlock(spawnStartX + m_block_length, spawnStartY - m_block_length);
		break;
	}
	case 'I':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(0);
		
		CreateBlock(spawnStartX - m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY);
		CreateBlock(spawnStartX + (2 * m_block_length), spawnStartY);
		break;
	}
	case 'J':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);
		
		CreateBlock(spawnStartX - m_block_length, spawnStartY + m_block_length);
		CreateBlock(spawnStartX - m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY);
		break;
	}
	case 'L':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY + m_block_length);
		break;
	}
	case 'S':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY + m_block_length);
		CreateBlock(spawnStartX + m_block_length, spawnStartY + m_block_length);
		break;
	}
	case 'T':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY);
		CreateBlock(spawnStartX, spawnStartY + m_block_length);
		break;
	}
	case 'Z':
	{
		float spawnStartX = GetXPosition(4);
		float spawnStartY = GetYPosition(1);

		CreateBlock(spawnStartX - m_block_length, spawnStartY + m_block_length);
		CreateBlock(spawnStartX, spawnStartY + m_block_length);
		CreateBlock(spawnStartX, spawnStartY);
		CreateBlock(spawnStartX + m_block_length, spawnStartY);
		break;
	}
	}


}

void Board::SpawnPiece()
{
	int numBlocks = 4;
	int blockVerts = 4;
	int numInsert = numBlocks * blockVerts * 4;

	std::vector<float> newBlock(numInsert);
	std::vector<unsigned int> newIdxs(numInsert);
	
	CreatePiece('Z');
	//CreateBlock(0.f, 0.0f);

	m_vertices.insert(m_vertices.end(), newBlock.begin(), newBlock.end());
	//m_indices.insert(m_indices.end(), newIdxs.begin(), newIdxs.end());
	glNamedBufferData(m_bufferHandle, sizeof(float) * numVertices(), getVertexPointer(), GL_STREAM_DRAW);
	glNamedBufferData(m_ebo, sizeof(unsigned int) * numIndices(), getIndexPointer(), GL_STREAM_DRAW);

}

size_t Board::numVertices()
{
	return m_vertices.size();
}

size_t Board::numIndices()
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