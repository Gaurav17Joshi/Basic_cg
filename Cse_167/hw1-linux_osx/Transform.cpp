// // Transform.cpp: implementation of the Transform class.


// #include "Transform.h"

// //Please implement the following functions:

// // Helper rotation function.  
// mat3 Transform::rotate(const float degrees, const vec3& axis) {
// 	// YOUR CODE FOR HW1 HERE

// 	// You will change this return call
// 	return mat3();
// }

// // Transforms the camera left around the "crystal ball" interface
// void Transform::left(float degrees, vec3& eye, vec3& up) {
// 	// YOUR CODE FOR HW1 HERE
// }

// // Transforms the camera up around the "crystal ball" interface
// void Transform::up(float degrees, vec3& eye, vec3& up) {
// 	// YOUR CODE FOR HW1 HERE 
// }

// // Your implementation of the glm::lookAt matrix
// mat4 Transform::lookAt(vec3 eye, vec3 up) {
// 	// YOUR CODE FOR HW1 HERE

// 	// You will change this return call
// 	return mat4();
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
mat4 Transform::lookAt(vec3 eye, vec3 up) {
    vec3 w = normalize(eye);
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

Transform::Transform() {}
Transform::~Transform() {}