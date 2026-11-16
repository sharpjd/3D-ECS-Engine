#version 430 core

//layout (location = 0) in vec3 aNormal; //can be inferred. No VAOs at all, wow

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 transposeInverseModel;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 chunkWorldPos; //lower-left-front corner of chunk in worldspace
uniform int[6] faceCounts; //forward, backward, upward, downward, right, left

//SSBO REFERENCE https://voxel.wiki/wiki/vertex-pulling/
layout(std430, binding = 0) readonly buffer vertexPullBuffer
{
	uint packedMeshData[];
};

// OpenGL depth testing treats CCW drawing as the "front" of a triangle

// for front/back/top/bottom, we assume we are facing forward

// Offsets for our vertices drawing this face
const vec3 faceVertPositions_front[4] = vec3[4] //the face staring at us
(
	vec3(-0.5f, -0.5f, 0.5f), 
	vec3( 0.5f, -0.5f, 0.5f), 
	vec3( 0.5f,  0.5f, 0.5f), 
	vec3(-0.5f,  0.5f, 0.5f)
);

const vec3 faceVertPositions_back[4] = vec3[4] //the face behind the cube
(
	vec3( 0.5f, -0.5f, -0.5f),
	vec3(-0.5f, -0.5f, -0.5f),
	vec3(-0.5f,  0.5f, -0.5f),
	vec3( 0.5f,  0.5f, -0.5f)
);

const vec3 faceVertPositions_top[4] = vec3[4]
(
	vec3(-0.5f,  0.5f,  0.5f),
	vec3( 0.5f,  0.5f,  0.5f),
	vec3( 0.5f,  0.5f, -0.5f),
	vec3(-0.5f,  0.5f, -0.5f)
);

const vec3 faceVertPositions_bottom[4] = vec3[4]
(
	vec3(-0.5f, -0.5f, -0.5f),
	vec3( 0.5f, -0.5f, -0.5f),
	vec3( 0.5f, -0.5f,  0.5f),
	vec3(-0.5f, -0.5f,  0.5f)
);

const vec3 faceVertPositions_left[4] = vec3[4]
(
	vec3(-0.5f, -0.5f, -0.5f),
	vec3(-0.5f, -0.5f,  0.5f),
	vec3(-0.5f,  0.5f,  0.5f),
	vec3(-0.5f,  0.5f, -0.5f)
);

const vec3 faceVertPositions_right[4] = vec3[4]
(
	vec3( 0.5f, -0.5f,  0.5f),
	vec3( 0.5f, -0.5f, -0.5f),
	vec3( 0.5f,  0.5f, -0.5f),
	vec3( 0.5f,  0.5f,  0.5f)
);

//FACE ORDER: Right, Left, Top, Bottom, Front, Back
const vec3 faceNormals[6] = vec3[6]
(
	vec3( 1.0f,  0.0f,  0.0f),
	vec3(-1.0f,  0.0f,  0.0f),
	vec3( 0.0f,  1.0f,  0.0f),
	vec3( 0.0f, -1.0f,  0.0f),
	vec3( 0.0f,  0.0f,  1.0f),
	vec3( 0.0f,  0.0f, -1.0f)
);

// Winding order to access the face positions
const int vertexOrder[6] = {0, 1, 2, 2, 3, 0};

void main() {
	
	const int index = gl_VertexID / 6; //increases by 1 every 6 steps
	const int currVertexID = gl_VertexID % 6; //cycles between 0-5

	const uint packedData = packedMeshData[index];
	// Unpack; we have 6 bits per position (2^6 = 64)
	// xyz is in CHUNKSPACE
	const uint x = (packedData)       & 63;
	const uint y = (packedData >> 6)  & 63;
	const uint z = (packedData >> 12) & 63;

	vec3 blockPosition = vec3(x, y, z) + chunkWorldPos; //convert to worldspace

	int chunkSide = 0;
	if(index >= faceCounts[4] + faceCounts[3] + faceCounts[2] + faceCounts[1] + faceCounts[0]){
		chunkSide = 5;
	} else if(index >= faceCounts[3] + faceCounts[2] + faceCounts[1] + faceCounts[0]) {
		chunkSide = 4;
	} else if(index >= faceCounts[2] + faceCounts[1] + faceCounts[0]) {
		chunkSide = 3;
	} else if(index >= faceCounts[1] + faceCounts[0]) {
		chunkSide = 2;
	} else if(index >= faceCounts[0]) {
		chunkSide = 1;
	} else {
		chunkSide = 0;
	}

	vec3 faceVertPositions[4];
	switch(chunkSide) { //TODO hopefully compiler optimizes it, but this switch is not needed
		case 0: faceVertPositions = faceVertPositions_right; break;
		case 1: faceVertPositions = faceVertPositions_left; break;
		case 2: faceVertPositions = faceVertPositions_top; break;
		case 3: faceVertPositions = faceVertPositions_bottom; break;
		case 4: faceVertPositions = faceVertPositions_front; break;
		case 5: faceVertPositions = faceVertPositions_back; break;
	}

	vec3 faceVertPosition = faceVertPositions[vertexOrder[currVertexID]];

	vec3 worldSpacePos = blockPosition + faceVertPosition;

	gl_Position = projection * view * model * vec4(worldSpacePos, 1.0);

	FragPos = vec3(model * vec4(worldSpacePos, 1.0));
	Normal = mat3(transposeInverseModel) * faceNormals[chunkSide];
	TexCoords = faceVertPosition.xz; //it's just a cube, this works out

}