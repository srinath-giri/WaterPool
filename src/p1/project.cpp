/**
 * @file project.cpp
 * @brief OpenGL project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "p1/project.hpp"
#include "math/vector.hpp"
#include <stdio.h>

// use this header to include the OpenGL headers
// DO NOT include gl.h or glu.h directly; it will not compile correctly.
#include "application/opengl.hpp"

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

	GLfloat purple[] = { 0.5f, 0.0f, 0.5f, 1.0f };
	GLfloat red[] = { 0.7f, 0.0f, 0.0f, 1.0f };
	GLfloat blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat lblue[] = { 0.0f, 0.0f, 0.7f, 1.0f };
	GLfloat lred[] = { 1.0f, 0.1f, 0.0f, 1.0f };
	GLfloat white[] = {1.0f, 1.0f, 1.0f};
	GLfloat grey[] = {0.5f, 0.5f, 0.5f};

// definitions of functions for the OpenglProject class

// constructor, invoked when object is created
OpenglProject::OpenglProject() {
	// any basic construction or initialization of members
	// Warning: Although members' constructors are automatically called,
	// ints, floats, pointers, and classes with empty contructors all
	// will have uninitialized data!
	this->water_mesh = new Vector3[WATER_MESH_MAX_VERTICES2D];
	this->water_mesh_quads = new Quad[WATER_MESH_MAX_QUADS2D];
	this->water_mesh_normals = new Vector3[WATER_MESH_MAX_VERTICES2D];
	this->nTriangles_of_vertices = NULL;
	this->nQuads_of_vertices = new unsigned int[WATER_MESH_MAX_VERTICES2D];
}

// destructor, invoked when object is destroyed
OpenglProject::~OpenglProject() {
	// any final cleanup of members
	// Warning: Do not throw exceptions or call virtual functions from deconstructors!
	// They will cause undefined behavior (probably a crash, but perhaps worse).
	delete [] water_mesh;
	delete [] water_mesh_quads;
	delete [] water_mesh_normals;
	delete [] nQuads_of_vertices;
}

/**
 * Initialize the project, doing any necessary opengl initialization.
 * @param camera An already-initialized camera.
 * @param scene The scene to render.
 * @return true on success, false on error.
 */
bool OpenglProject::initialize(Camera* camera, Scene* scene) {
	// copy scene
	this->scene = *scene;
	MeshData &mesh = scene->mesh;

	// opengl initialization code and precomputation of mesh/heightmap
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute Vertex Normals of pool
	Triangle* triangle = mesh.triangles;
	unsigned int nVertices = mesh.num_vertices;
	unsigned int nTriangles = mesh.num_triangles;
	nTriangles_of_vertices = new unsigned int[nVertices];

	unsigned int idx0;
	unsigned int idx1;
	unsigned int idx2;
	for (unsigned int i = 0; i < nTriangles; i++, triangle++) {
		unsigned int * indices = triangle->vertices;
		idx0 = indices[0];
		idx1 = indices[1];
		idx2 = indices[2];
		Vector3 v1 = mesh.vertices[idx0];
		Vector3 v2 = mesh.vertices[idx1];
		Vector3 v3 = mesh.vertices[idx2];
		// Normal is the cross product of 2 edges of the triangle
		Vector3 normal = normalize(cross(v3 - v1, v2 - v1));
		mesh.vertexNormals[idx0] += normal;
		nTriangles_of_vertices[idx0] += 1;
		mesh.vertexNormals[idx1] += normal;
		nTriangles_of_vertices[idx1] += 1;
		mesh.vertexNormals[idx2] += normal;
		nTriangles_of_vertices[idx2] += 1;
	}

	for (unsigned int i = 0; i < nVertices; i++) {
		mesh.vertexNormals[i] /= nTriangles_of_vertices[i];
	}

	// Pre-initialize water mesh and its indices
	real_t x = -1.0;
	real_t z = -1.0;
	real_t step = 2.0 / (WATER_MESH_MAX_VERTICES1D - 1);

	// Creating vertices of water mesh (x,z coordinates only)
	unsigned int i = 0, j = 0;
	for (; j < WATER_MESH_MAX_VERTICES2D;) {
		Vector3 &v = water_mesh[j];
		v.x = x;v.z = z;
		z = z + step;
		j++;
		if (j % WATER_MESH_MAX_VERTICES1D == 0) {
			z = -1.0;
			x = x + step;
		}
	}

	// Creating indices of water mesh
	for (i = 0; i < WATER_MESH_MAX_QUADS1D; i++) {
		for (j = 0; j < WATER_MESH_MAX_QUADS1D; j++) {
			Quad &quad = water_mesh_quads[i * WATER_MESH_MAX_QUADS1D + j];
			quad.vertices[0] = i * WATER_MESH_MAX_VERTICES1D + j;
			quad.vertices[1] = i * WATER_MESH_MAX_VERTICES1D + j + 1;
			quad.vertices[2] = (i + 1) * WATER_MESH_MAX_VERTICES1D + j + 1;
			quad.vertices[3] = (i + 1) * WATER_MESH_MAX_VERTICES1D + j;
		}
	}

	// Load Identity Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set Camera Parameters
	glMatrixMode(GL_PROJECTION);
	gluPerspective(camera->get_fov_degrees(), camera->aspect, camera->near_clip,
			camera->far_clip);
	Vector3 camera_position = camera->get_position();
	Vector3 camera_direction = camera->get_direction();
	Vector3 camera_up = camera->get_up();
	gluLookAt(camera_position.x, camera_position.y, camera_position.z,
			camera_position.x + camera_direction.x,
			camera_position.y + camera_direction.y,
			camera_position.z + camera_direction.z, camera_up.x, camera_up.y,
			camera_up.z);


	// Set Lighting parameters
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, grey);

	// Enable Depth Test
	glEnable(GL_DEPTH_TEST);

	return true;
}

real_t OpenglProject::radians_to_degrees(real_t angle) {
	return angle * 180.0 / PI;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void OpenglProject::destroy() {
	// any cleanup code, e.g., freeing memory
	delete [] nTriangles_of_vertices;
}

/**
 * Perform an update step. This happens on a regular interval.
 * @param dt The time difference from the previous frame to the current.
 */
void OpenglProject::update(real_t dt) {
	// update our heightmap
	scene.heightmap->update(dt);

	// Compute heights of water mesh
	Vector2 v(0.0, 0.0);
	for (unsigned int i = 0; i < WATER_MESH_MAX_VERTICES2D; i++) {
		v.x = water_mesh[i].x;
		v.y = water_mesh[i].z;
		water_mesh[i].y = scene.heightmap->compute_height(v);
	}

	// Compute Vertex Normals of water mesh
	Quad* quad = water_mesh_quads;
	unsigned int idx0, idx1,idx2, idx3;
	for (unsigned int i = 0; i < WATER_MESH_MAX_QUADS2D; i++, quad++) {
		unsigned int * indices = quad->vertices;
		idx0 = indices[0];
		idx1 = indices[1];
		idx2 = indices[2];
		idx3 = indices[3];
		Vector3 v1 = water_mesh[idx0];
		Vector3 v2 = water_mesh[idx1];
		Vector3 v3 = water_mesh[idx2];
		Vector3 v4 = water_mesh[idx3];
		// Normal is cross-product of diagonals of quadrilateral
		Vector3 normal = normalize(cross(v1 - v3, v2 - v4));
		water_mesh_normals[idx0] += normal;
		nQuads_of_vertices[idx0] += 1;
		water_mesh_normals[idx1] += normal;
		nQuads_of_vertices[idx1] += 1;
		water_mesh_normals[idx2] += normal;
		nQuads_of_vertices[idx2] += 1;
		water_mesh_normals[idx3] += normal;
		nQuads_of_vertices[idx3] += 1;
	}

	// Average of normals is the vertex normal
	for (unsigned int i = 0; i < WATER_MESH_MAX_VERTICES2D; i++) {
		water_mesh_normals[i] /= nQuads_of_vertices[i];
	}

}

/**
 * Clear the screen, then render the mesh using the given camera.
 * @param camera The logical camera to use.
 * @see math/camera.hpp
 */
void OpenglProject::render(const Camera* camera) {
	// render code
	MeshData &mesh = scene.mesh;
	Vector3 position;
	Vector3 axes;
	real_t angle;
	Vector3 scale;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	const Vector3 camera_position = camera->get_position();
	const Vector3 camera_direction = camera->get_direction();
	const Vector3 camera_up = camera->get_up();
	const Vector3 target_point = camera_position + camera_direction;

	gluLookAt(camera_position.x, camera_position.y, camera_position.z,
			target_point.x, target_point.y, target_point.z,
			camera_up.x, camera_up.y, camera_up.z);

	glMatrixMode(GL_MODELVIEW);

	/* -------- Draw Pool --------------*/

	// Load vertices of pool
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, mesh.vertices);

	// Load vertexNormals of pool
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, mesh.vertexNormals);

	// Transform and draw pool
	glPushMatrix();
		position = scene.mesh_position.position;
		glTranslated(position.x, position.y, position.z);

		scene.mesh_position.orientation.to_axis_angle(&axes, &angle);
		glRotated(radians_to_degrees(angle), axes.x, axes.y, axes.z);

		scale = scene.mesh_position.scale;
		glScaled(scale.x, scale.y, scale.z);

		// Set Material parameters
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, lred);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, lred);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);

		glDrawElements( GL_TRIANGLES, this->scene.mesh.num_triangles * 3,
		GL_UNSIGNED_INT, this->scene.mesh.triangles);
	glPopMatrix();
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	/* -------- Draw Water Mesh --------------*/

	// Load vertices of water mesh
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, water_mesh);

	// Load vertexNormals of water mesh
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, water_mesh_normals);

	//  Transform and draw pool
	glPushMatrix();
		position = scene.heightmap_position.position;
		glTranslated(position.x, position.y, position.z);

		scene.heightmap_position.orientation.to_axis_angle(&axes, &angle);
		glRotated(radians_to_degrees(angle), axes.x, axes.y, axes.z);

		scale = scene.heightmap_position.scale;
		glScaled(scale.x, scale.y, scale.z);

		// Set Material parameters
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, lblue);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, grey);

		glDrawElements( GL_QUADS, WATER_MESH_MAX_QUADS2D * 4,
		GL_UNSIGNED_INT, this->water_mesh_quads);
	glPopMatrix();
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}

} /* _462 */
