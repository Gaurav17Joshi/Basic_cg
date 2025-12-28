/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
// OSX systems require different headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include "Transform.h"
#include "Geometry.h"

using namespace std ; 
#include "variables.h"
#include "readfile.h"

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  
void transformvec (const GLfloat input[4], GLfloat output[4]) 
{
  glm::vec4 inputvec(input[0], input[1], input[2], input[3]);
  glm::vec4 outputvec = modelview * inputvec;
  output[0] = outputvec[0];
  output[1] = outputvec[1];
  output[2] = outputvec[2];
  output[3] = outputvec[3];
}


void display() 
{
  glClearColor(0, 0, 1, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Either use the built-in lookAt function or the lookAt implemented by the user.
  if (useGlu) {
    modelview = glm::lookAt(eye,center,up); 
  } else {
    modelview = Transform::lookAt(eye,center,up); 
  }
  glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);

  // Lights are transformed by current modelview matrix. 
  if (numused) {
    glUniform1i(enablelighting,true);

    // YOUR CODE FOR HW 2 HERE - SOLUTION:
    // Transform light positions by modelview matrix
    for (int i = 0; i < numused; i++) {
      GLfloat light_input[4] = {
        lightposn[4*i], 
        lightposn[4*i+1], 
        lightposn[4*i+2], 
        lightposn[4*i+3]
      };
      transformvec(light_input, &lightransf[4*i]);
    }
    
    // Pass transformed light positions and colors to shader
    glUniform4fv(lightpos, numused, lightransf);
    glUniform4fv(lightcol, numused, lightcolor);
    glUniform1i(numusedcol, numused);

  } else {
    glUniform1i(enablelighting,false); 
  }

  // Transformations for objects, involving translation and scaling 
  mat4 sc(1.0) , tr(1.0), transf(1.0); 
  sc = Transform::scale(sx,sy,1.0); 
  tr = Transform::translate(tx,ty,0.0); 

  // YOUR CODE FOR HW 2 HERE - SOLUTION:
  // Combine transformations: modelview * translate * scale
  // Order matters! We apply scale first, then translate, then camera view
  transf = modelview * tr * sc;

  // The object draw functions will need to further modify the top of the stack
  modelview = transf;

  for (int i = 0 ; i < numobjects ; i++) {
    object* obj = &(objects[i]);

    // YOUR CODE FOR HW 2 HERE - SOLUTION:
    // Set modelview to include object's transformation
    // glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &(transf * obj->transform)[0][0]);
    modelview = transf * obj->transform;
    
    // Pass material properties to shader
    glUniform4fv(ambientcol, 1, obj->ambient);
    glUniform4fv(diffusecol, 1, obj->diffuse);
    glUniform4fv(specularcol, 1, obj->specular);
    glUniform4fv(emissioncol, 1, obj->emission);
    glUniform1f(shininesscol, obj->shininess);

    // Actually draw the object
    if (obj->type == cube) {
      solidCube(obj->size); 
    }
    else if (obj->type == sphere) {
      const int tessel = 20; 
      solidSphere(obj->size, tessel, tessel); 
    }
    else if (obj->type == teapot) {
      solidTeapot(obj->size); 
    }
  }

  glutSwapBuffers();
}
