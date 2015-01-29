#include "Mesh.h"

using namespace std;

Mesh :: Mesh()
{
	filename = "";
	vao = 0;
	point_count = 0;
}

Mesh :: Mesh(const char* _filename)
{
	filename = _filename;
	vao = 0;
	point_count = 0;
}

bool Mesh :: load_mesh (std::vector<glm::vec3> &v)
{
	
	v = getVertices();
	
  return load_mesh();
}



bool Mesh :: load_mesh ()
{
	

	
  const aiScene* scene = aiImportFile (filename, aiProcess_Triangulate); // TRIANGLES!

  
  if (!scene)
  {
    fprintf (stderr, "ERROR: reading mesh %s\n", filename);
    return false;
  }
  printf ("  %i animations\n", scene->mNumAnimations);
  printf ("  %i cameras\n", scene->mNumCameras);
  printf ("  %i lights\n", scene->mNumLights);
  printf ("  %i materials\n", scene->mNumMaterials);
  printf ("  %i meshes\n", scene->mNumMeshes);
  printf ("  %i textures\n", scene->mNumTextures);
  
  

  for(int i=0; i<scene->mNumMeshes; i++)
  {
	assign_vao(scene);
  }
 
  aiReleaseImport (scene);
  printf("mesh loaded\n");
  
  return true;
}


//loads assimp mesh data into the vao
void Mesh :: assign_vao(const aiScene* scene)
{

	//get materials
	if (scene->mNumMaterials > 0)
	{
		const aiMaterial* a_mat = scene->mMaterials[0];
		
		aiColor4D ambient;
		aiGetMaterialColor(a_mat, AI_MATKEY_COLOR_AMBIENT, &ambient);

		aiColor4D diffuse;
		aiGetMaterialColor(a_mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

		aiColor4D specular;
		aiGetMaterialColor(a_mat, AI_MATKEY_COLOR_SPECULAR, &specular);

		float shininess = 0.0f;
		aiGetMaterialFloat(a_mat, AI_MATKEY_SHININESS, &shininess);


		
		material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
		material.specular = glm::vec3(specular.r, specular.g, specular.b);
		material.shininess = shininess;

		material.ambient = (material.diffuse) * (0.1f);

		material.cool = glm::vec3(0.0, 0.0, 1.0);
		material.warm = glm::vec3(1.0, 1.0, 0.4);
	}




	 const aiMesh* mesh = scene->mMeshes[0]; 


	 printf("   %i vertices in mesh[%i]\n", mesh->mNumVertices, 0);

	  point_count = mesh->mNumVertices;


	  glGenVertexArrays(1, &vao);
	  glBindVertexArray(vao);

		GLfloat* points = NULL;
		GLfloat* normals = NULL;
		GLfloat* texcoords = NULL;


		GLint* bone_ids_lbs = NULL;
		GLfloat* bone_weights = NULL;

		//get vertex positions
		  if (mesh->HasPositions ())
		  {

			points = (GLfloat*)malloc (point_count * 3 * sizeof (GLfloat));

			for (int i=0; i< point_count; i++)
			{
				const aiVector3D* vp = &(mesh->mVertices[i]);
				points[i*3] = (GLfloat)vp->x;
				points[i*3+1] = (GLfloat)vp->y;
				points[i*3+2] = (GLfloat)vp->z;
			}


		  }

		  //get normals
		  if (mesh->HasNormals ()) {
			normals = (GLfloat*)malloc(point_count * 3 * sizeof (GLfloat));

			for (int i=0; i<point_count; i++)
			{
				const aiVector3D* vn = &(mesh->mNormals[i]);
				normals[i*3] = (GLfloat)vn->x;
				normals[i*3+1] = (GLfloat)vn->y;
				normals[i*3+2] = (GLfloat)vn->z;
			}
		  }

		  //get texture coordinates
		  if (mesh->HasTextureCoords (0)) {
			texcoords = (GLfloat*)malloc(point_count * 2 * sizeof (GLfloat));

			for (int i=0; i<point_count; i++)
			{
				const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
				texcoords[i*2] = (GLfloat)vt->x;
				texcoords[i*2+1] = (GLfloat)vt->y;
		
			}
		  }



	
		  // copy mesh data in VBOs


		  //load in vertex positions
		  if(mesh->HasPositions())
		  {
			  GLuint vbo;
			  glGenBuffers (1, &vbo);
			  glBindBuffer(GL_ARRAY_BUFFER, vbo);
			  glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), points, GL_STATIC_DRAW);

			  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			  glEnableVertexAttribArray(0);
			  free (points);
		  }

		  //load in normals
		  if(mesh->HasNormals())
		  {
			  GLuint vbo;
			  glGenBuffers (1, &vbo);
			  glBindBuffer(GL_ARRAY_BUFFER, vbo);
			  glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);

			  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			  glEnableVertexAttribArray(1);
			  free (normals);
		  }

		  //load in texture coordinates
		  if(mesh->HasTextureCoords(0))
		  {
			  GLuint vbo;
			  glGenBuffers (1, &vbo);
			  glBindBuffer(GL_ARRAY_BUFFER, vbo);
			  glBufferData(GL_ARRAY_BUFFER, 2 * point_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);

			  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			  glEnableVertexAttribArray(2);
			  free (texcoords);
		  }


}

void Mesh :: draw(GLuint spID, glm::mat4 model_mat)
{
	int ka_location = glGetUniformLocation (spID, "ka");
	int kd_location = glGetUniformLocation (spID, "kd");
	int ks_location = glGetUniformLocation (spID, "ks");
	int shine_location = glGetUniformLocation (spID, "spec_ex");
	int kcool_location = glGetUniformLocation (spID, "kcool");
	int kwarm_location = glGetUniformLocation (spID, "kwarm");

	glUniform3fv(ka_location, 1, glm::value_ptr(material.ambient));
	glUniform3fv(kd_location, 1, glm::value_ptr(material.diffuse));
	glUniform3fv(ks_location, 1, glm::value_ptr(material.specular));
	glUniform1fv(shine_location, 1, &material.shininess);
	glUniform3fv(kcool_location, 1, glm::value_ptr(material.cool));
	glUniform3fv(kwarm_location, 1, glm::value_ptr(material.warm));

	int matrix_location = glGetUniformLocation (spID, "model");
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, point_count);
}

std::vector<glm::vec3> Mesh :: getVertices()
{
	const aiScene* scene = aiImportFile (filename, aiProcess_Triangulate);

	const aiMesh* mesh = scene->mMeshes[0]; 

	int vertex_count = mesh->mNumVertices;
	std::vector<glm::vec3> v (vertex_count);

	//get vertex positions
	if (mesh->HasPositions ())
	{

		for (int i=0; i< vertex_count; i++)
		{
			const aiVector3D* vp = &(mesh->mVertices[i]);
			v[i].x = (GLfloat)vp->x;
			v[i].y = (GLfloat)vp->y;
			v[i].z = (GLfloat)vp->z;
		}
	}
	return v;
}


void Mesh :: addTBar(TwBar *bar)
{


	TwAddVarRW(bar, vao+"    Ambient", TW_TYPE_COLOR3F, &material.ambient, "");
	TwAddVarRW(bar, vao+"    Diffuse", TW_TYPE_COLOR3F, &material.diffuse, "");
	TwAddVarRW(bar, vao+"    Speculular", TW_TYPE_COLOR3F, &material.specular, "");
	TwAddVarRW(bar, vao+"    Shininess", TW_TYPE_FLOAT, &material.shininess, "");
	TwAddVarRW(bar, vao+"    Cool", TW_TYPE_COLOR3F, &material.cool, "");
	TwAddVarRW(bar, vao+"    Warm", TW_TYPE_COLOR3F, &material.warm, "");
}
