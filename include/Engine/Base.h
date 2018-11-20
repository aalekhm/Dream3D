#ifndef BASE_H
#define BASE_H

//C/C++
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include <gl\glew.h>
//#include <gl\gl.h>						// Header File For The OpenGL32 Library
//#include <gl\glu.h>						// Header File For The GLu32 Library

// Common
#ifndef NULL
#define NULL     0
#endif

typedef GLuint VBOHandle;
typedef GLuint VAOHandle;

/** Global variable to hold GL errors
 * @script{ignore} */
static GLenum __gl_error_code;

// Assert macros.
#ifdef _DEBUG
#define GP_ASSERT(expression) assert(expression)
#else
#define GP_ASSERT(expression)
#endif


#ifdef NDEBUG
#define GL_ASSERT( gl_code ) gl_code
#else
#define GL_ASSERT( gl_code ) do	{ gl_code; __gl_error_code = glGetError(); GP_ASSERT(__gl_error_code == GL_NO_ERROR); } while(0)
#endif

// Object deletion macro
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

// Array deletion macro
#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x = NULL; }

// Ref cleanup macro
#define SAFE_RELEASE(x)	if(x) { (x)->release(); x = NULL; }

typedef GLuint TextureHandle;

#endif