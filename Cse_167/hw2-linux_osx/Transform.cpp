// // Transform.cpp: implementation of the Transform class.

// // Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// // Keep this in mind in readfile.cpp and display.cpp
// // See FAQ for more details or if you're having problems.

// #include "Transform.h"

// // Helper rotation function.  Please implement this.  
// mat3 Transform::rotate(const float degrees, const vec3& axis) 
// {
//   mat3 ret;
//   // YOUR CODE FOR HW2 HERE
//   // Please implement this.  Likely the same as in HW 1.  
//   return ret;
// }

// void Transform::left(float degrees, vec3& eye, vec3& up) 
// {
//   // YOUR CODE FOR HW2 HERE
//   // Likely the same as in HW 1.  
// }

// void Transform::up(float degrees, vec3& eye, vec3& up) 
// {
//   // YOUR CODE FOR HW2 HERE 
//   // Likely the same as in HW 1.  
// }

// mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
// {
//   mat4 ret;
//   // YOUR CODE FOR HW2 HERE
//   // Likely the same as in HW 1.  
//   return ret;
// }

// mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
// {
//   mat4 ret;
//   // YOUR CODE FOR HW2 HERE
//   // New, to implement the perspective transform as well.  
//   return ret;
// }

// mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
// {
//   mat4 ret;
//   // YOUR CODE FOR HW2 HERE
//   // Implement scaling 
//   return ret;
// }

// mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
// {
//   mat4 ret;
//   // YOUR CODE FOR HW2 HERE
//   // Implement translation 
//   return ret;
// }

// // To normalize the up direction and construct a coordinate frame.  
// // As discussed in the lecture.  May be relevant to create a properly 
// // orthogonal and normalized up. 
// // This function is provided as a helper, in case you want to use it. 
// // Using this function (in readfile.cpp or display.cpp) is optional.  

// vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
// {
//   vec3 x = glm::cross(up,zvec); 
//   vec3 y = glm::cross(zvec,x); 
//   vec3 ret = glm::normalize(y); 
//   return ret; 
// }


// Transform::Transform()
// {

// }

// Transform::~Transform()
// {

// }

// Transform.cpp: implementation of the Transform class.

#include "Transform.h"
#include <cmath>

using namespace glm;

// Helper rotation function 
mat3 Transform::rotate(const float degrees, const vec3& axis) {
    float radians = glm::radians(degrees);
    vec3 a = normalize(axis);

    float c = cos(radians);
    float s = sin(radians);

    mat3 I(1.0f);
    mat3 A(  0,   -a.z,  a.y,
           a.z,     0, -a.x,
          -a.y,  a.x,    0);

    // R = I*c + (1-c)(a a^T) + s[A]
    mat3 R = c * I + (1 - c) * outerProduct(a, a) + s * A;
    return R;
}

// Rotate camera LEFT/RIGHT around the crystal ball
void Transform::left(float degrees, vec3& eye, vec3& up) {
    mat3 R = rotate(degrees, up);
    eye = R * eye;
}

// Rotate camera UP/DOWN around the crystal ball
void Transform::up(float degrees, vec3& eye, vec3& up) {
    vec3 right = normalize(cross(up, eye));
    mat3 R = rotate(degrees, right);

    eye = R * eye;
    up  = normalize(R * up);
}

// Custom implementation of glm::lookAt
mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
    vec3 w = normalize(eye - center);
    vec3 u = normalize(cross(up, w));
    vec3 v = cross(w, u);

    mat4 M(1.0f);

    M[0][0] = u.x;  M[1][0] = u.y;  M[2][0] = u.z;
    M[0][1] = v.x;  M[1][1] = v.y;  M[2][1] = v.z;
    M[0][2] = w.x;  M[1][2] = w.y;  M[2][2] = w.z;

    M[3][0] = -dot(u, eye);
    M[3][1] = -dot(v, eye);
    M[3][2] = -dot(w, eye);

    return M;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    // Convert fovy from degrees to radians
    float radians = glm::radians(fovy);
    float tanHalfFovy = tan(radians / 2.0f);
    
    mat4 ret(0.0f); // Initialize to zero
    
    ret[0][0] = 1.0f / (aspect * tanHalfFovy);
    ret[1][1] = 1.0f / tanHalfFovy;
    ret[2][2] = -(zFar + zNear) / (zFar - zNear);
    ret[2][3] = -1.0f;
    ret[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    
    return ret;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    mat4 ret(1.0f); // Start with identity
    ret[0][0] = sx;
    ret[1][1] = sy;
    ret[2][2] = sz;
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    mat4 ret(1.0f); // Start with identity
    ret[3][0] = tx;
    ret[3][1] = ty;
    ret[3][2] = tz;
    return ret;
}

// To normalize the up direction and construct a coordinate frame.  
vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}

Transform::Transform() {}
Transform::~Transform() {}