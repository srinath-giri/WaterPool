/**
 * @file project.cpp
 * @brief OpenGL project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "p1/project.hpp"

// use this header to include the OpenGL headers
// DO NOT include gl.h or glu.h directly; it will not compile correctly.
#include "application/opengl.hpp"

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

// definitions of functions for the OpenglProject class

// constructor, invoked when object is created
OpenglProject::OpenglProject()
{
    // TODO any basic construction or initialization of members
    // Warning: Although members' constructors are automatically called,
    // ints, floats, pointers, and classes with empty contructors all
    // will have uninitialized data!
}

// destructor, invoked when object is destroyed
OpenglProject::~OpenglProject()
{
    // TODO any final cleanup of members
    // Warning: Do not throw exceptions or call virtual functions from deconstructors!
    // They will cause undefined behavior (probably a crash, but perhaps worse).
}

/**
 * Initialize the project, doing any necessary opengl initialization.
 * @param camera An already-initialized camera.
 * @param scene The scene to render.
 * @return true on success, false on error.
 */
bool OpenglProject::initialize( Camera* camera, Scene* scene )
{
    // copy scene
    this->scene = *scene;

    // TODO opengl initialization code and precomputation of mesh/heightmap
    return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void OpenglProject::destroy()
{
    // TODO any cleanup code, e.g., freeing memory
}

/**
 * Perform an update step. This happens on a regular interval.
 * @param dt The time difference from the previous frame to the current.
 */
void OpenglProject::update( real_t dt )
{
    // update our heightmap
    scene.heightmap->update( dt );

    // TODO any update code, e.g. commputing heightmap mesh positions and normals
}

/**
 * Clear the screen, then render the mesh using the given camera.
 * @param camera The logical camera to use.
 * @see math/camera.hpp
 */
void OpenglProject::render( const Camera* camera )
{
    // TODO render code
}

} /* _462 */
