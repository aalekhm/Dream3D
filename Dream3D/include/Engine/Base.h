#pragma once
//#define USE_VAO

#ifndef USE_VAO
	//#define USE_VERTEX_POINTERS
#endif

//C/C++
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <map>

#include <gl\glew.h>
//#include <gl\gl.h>						// Header File For The OpenGL32 Library
//#include <gl\glu.h>						// Header File For The GLu32 Library

// Common
#ifndef NULL
#define NULL     0
#endif

typedef GLuint VertexAttribute;				// Vertex Attribute
typedef GLuint VBOHandle;					// Vertex Buffer Object Handle
typedef GLuint VAOHandle;					// Vertex Array Object Handle
typedef GLuint IBOHandle;					// Indexed Buffer Object Handle
typedef GLuint FrameBufferHandle;	/** Frame buffer handle. */
typedef GLuint RenderBufferHandle;	/** Render buffer handle. */

#define VERTEX_ATTRIBUTE_POSITION_NAME              "a_position"
#define VERTEX_ATTRIBUTE_NORMAL_NAME                "a_normal"
#define VERTEX_ATTRIBUTE_COLOR_NAME                 "a_color"
#define VERTEX_ATTRIBUTE_TANGENT_NAME               "a_tangent"
#define VERTEX_ATTRIBUTE_BINORMAL_NAME              "a_binormal"
#define VERTEX_ATTRIBUTE_BLENDWEIGHTS_NAME          "a_blendWeights"
#define VERTEX_ATTRIBUTE_BLENDINDICES_NAME          "a_blendIndices"
#define VERTEX_ATTRIBUTE_TEXCOORD_PREFIX_NAME       "a_texCoord"

#define gl_Vertex 			0
#define gl_Normal 			1
#define gl_TexCoord 		8
#define gl_Color 			3
#define gl_SecondaryColor 	4
#define gl_FogCoord 		5
#define gl_MultiTexCoord0 	8
#define gl_MultiTexCoord1 	9
#define gl_MultiTexCoord2 	10
#define gl_MultiTexCoord3 	11
#define gl_MultiTexCoord4 	12
#define gl_MultiTexCoord5 	13
#define gl_MultiTexCoord6 	14
#define gl_MultiTexCoord7 	15

/** Global variable to hold GL errors
 * @script{ignore} */
static GLenum __gl_error_code;

// Assert macros.
#ifdef NDEBUG
#define GP_ASSERT(expression)
#else
#define GP_ASSERT(expression) assert(expression)
#endif


#ifdef NDEBUG
#define GL_ASSERT( gl_code ) gl_code
#else
#define GL_ASSERT( gl_code ) do	{ gl_code; __gl_error_code = glGetError(); GP_ASSERT(__gl_error_code == GL_NO_ERROR); } while(0)
#endif

#ifdef GP_ERRORS_AS_WARNINGS
#define GP_ERROR GP_WARN
#else
#define GP_ERROR(...) do { /*LOG HERE...*/ assert(0); std::exit(-1); } while(0)
#endif

#define END_OF_FILE 0

// Object deletion macro
#define SAFE_DELETE(x) if(x != NULL) { delete x; x = NULL; }

// Array deletion macro
#define SAFE_DELETE_ARRAY(x) if(x != NULL) { delete[] x; x = NULL; }

// Ref cleanup macro
#define SAFE_RELEASE(x)	if(x != NULL) { (x)->release(); x = NULL; }

typedef GLuint TextureHandle;

#define MATH_RANDOM_0_1()           ((float)rand()/RAND_MAX)                    // Returns a random float between 0 and 1.
#define MATH_DEG_TO_RAD(x)          ((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)          ((x)* 57.29577951f)

static void trimString(std::string& sInStr)
{
	// Left Trim
	sInStr.erase(	sInStr.begin(),
					std::find_if_not(sInStr.begin(), sInStr.end(),
					[](unsigned char ch) 
					{
						return std::isspace(ch);
					}));

	// Right Trim
	sInStr.erase(	std::find_if_not(sInStr.rbegin(), sInStr.rend(),
					[](unsigned char ch) 
					{
						return std::isspace(ch);
					}).base(),
					sInStr.end());
}

static void stdReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
	std::string::size_type iPos = 0;
	while ((iPos = str.find(oldStr, iPos)) != std::string::npos)
	{
		str.replace(iPos, oldStr.length(), newStr);
		iPos += newStr.length();
	}
}

static std::string toLowerCase(std::string sStr)
{
	std::transform(	sStr.begin(),
					sStr.end(),
					sStr.begin(),
					[](unsigned char ch) 
					{
						return std::tolower(ch);
					});
	
	return sStr;
}

static std::string toUpperCase(std::string sStr)
{
	std::transform(	sStr.begin(),
					sStr.end(),
					sStr.begin(),
					[](unsigned char ch) 
					{
						return std::toupper(ch);
					});

	return sStr;
}

static bool isLowerCase(char c)
{
	return (c >= 97 && c <= 122);
}

static bool isUpperCase(char c)
{
	return (c >= 65 && c <= 90);
}

static bool isAlphabet(char c)
{
	return (isLowerCase(c) || isUpperCase(c));
}