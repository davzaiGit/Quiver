#include "Render_Utils.h"

#include <algorithm>

#include "glew.h"
#include "freeglut.h"
#include <ext.hpp>


void Core::RenderContext::initFromOBJ(obj::Model& model)
{
    vertexArray = 0;
    vertexBuffer = 0;
    vertexIndexBuffer = 0;
    unsigned int vertexDataBufferSize = sizeof(float) * model.vertex.size();
    unsigned int vertexNormalBufferSize = sizeof(float) * model.normal.size();
    unsigned int vertexTexBufferSize = sizeof(float) * model.texCoord.size();

    size = model.faces["default"].size();
    unsigned int vertexElementBufferSize = sizeof(unsigned short) * size;


    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);


    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexElementBufferSize, &model.faces["default"][0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBufferData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataBufferSize, &model.vertex[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize, vertexNormalBufferSize, &model.normal[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize, vertexTexBufferSize, &model.texCoord[0]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexNormalBufferSize + vertexDataBufferSize));
}




physx::PxVec3 Core::GlmToPxVec3(glm::vec3 in)
{
    return physx::PxVec3(in.x, in.y, in.z);
}

glm::vec3 Core::PxVec3ToGlm(physx::PxVec3 in)
{
	return glm::vec3(in.x, in.y, in.z);
}


void Core::DrawVertexArray(const float * vertexArray, int numVertices, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Core::DrawVertexArrayIndexed( const float * vertexArray, const int * indexArray, int numIndexes, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, indexArray);
}


void Core::DrawVertexArray( const VertexData & data )
{
	int numAttribs = std::min(VertexData::MAX_ATTRIBS, data.NumActiveAttribs);
	for(int i = 0; i < numAttribs; i++)
	{
		glVertexAttribPointer(i, data.Attribs[i].Size, GL_FLOAT, false, 0, data.Attribs[i].Pointer);
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLES, 0, data.NumVertices);
}

void Core::DrawContext(Core::RenderContext& context)
{

	glBindVertexArray(context.vertexArray);
	glDrawElements(
		GL_TRIANGLES,      // mode
		context.size,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
}



void Core::RenderText(Core::TextContext& textContext, GLuint program, std::string text, float x, float y, float scale, glm::vec3 color)
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program);
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 temp = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&temp);
	glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textContext.vao);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Core::Character ch = textContext.Characters[*c];
		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, textContext.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Core::RenderHud(Core::TextContext& textContext, GLuint program, GLuint tex, float x, float y, float scale, glm::vec3 color)
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program);
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 temp = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&temp);
	glUniform3f(glGetUniformLocation(program, "textColor"), color.x,color.y,color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textContext.vao);
	float vertices[6][4] = {
			{ x,     y + 1 * scale,   0.0f, 0.0f },
			{ x,     y,       0.0f, 1.0f },
			{ x + 1 * scale, y,       1.0f, 1.0f },

			{ x,     y + 1 * scale,   0.0f, 0.0f },
			{ x + 1 * scale, y,       1.0f, 1.0f },
			{ x + 1 * scale, y + 1 * scale,   1.0f, 0.0f }
	};
	glBindTexture(GL_TEXTURE_2D, tex);
	glBindBuffer(GL_ARRAY_BUFFER, textContext.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}


void Core::TextContext::initText()
{
	FT_Init_FreeType(&lib);
	FT_New_Face(lib, "fonts/font.ttf", 0, &fontFace);
	FT_Set_Pixel_Sizes(fontFace, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			fontFace->glyph->bitmap.width,
			fontFace->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			fontFace->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Core::Character character = {
			texture,
			glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
			glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
			fontFace->glyph->advance.x
		};
		Characters.insert(std::pair<char, Core::Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(fontFace);
	FT_Done_FreeType(lib);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
