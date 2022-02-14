#define GLEW_STATIC
#include "mesh.h"
#include "vectornd.h"
#include "geometry.h"
#include "importstl.h"
#include "exportobj.h"
#include <GL/glew.h>
#include <map>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glew32sd.lib")

glm::vec3 calNormal(std::vector<glm::vec3> points) {
    glm::vec3 n = glm::cross(points[1] - points[0], points[2] - points[0]);
    if (n != glm::vec3(0, 0, 0)) {
        return glm::normalize(n);
    }
    return n;
}

Mesh::Mesh(const std::string& fileName, const char* outPath, int simplifyFlag, double percent) {
    if (fileName.find(".obj") < fileName.length()) {
        OBJModel objModel = OBJModel(fileName, simplifyFlag, percent);
        IndexedModel model = objModel.ToIndexedModel();
        saveStl(objModel, model, outPath);

    }
    else if (fileName.find(".stl") < fileName.length() || fileName.find(".STL") < fileName.length()) {
        /*
                OBJModel objModel = OBJModel(fileName, percent);
                IndexedModel model = objModel.ToIndexedModel();
                saveStl(objModel, model);
        */
        Geometry tessel;
        //  fill up the tesselation object with STL data (load STL)
        tessel.visit(ImportSTL(fileName));
        //  write down the tesselation object into OBJ file (save OBJ)
        tessel.visit(ExportOBJ(".temp.obj"));

        OBJModel objModel = OBJModel(".temp.obj", simplifyFlag, percent);
        IndexedModel model = objModel.ToIndexedModel();
        saveStl(objModel, model, outPath);



    }
}

//@Edited
// simplifyFlag = 0 -> load Mesh as is
// simplifyFlag = 1 -> load Mesh and reduce the triangles
//					   to MAX_FACES 
Mesh::Mesh(const std::string& fileName, int simplifyFlag, double percent)
{
    if (fileName.find(".obj") < fileName.length()) {
        OBJModel objModel = OBJModel(fileName, simplifyFlag, percent);
        IndexedModel model = objModel.ToIndexedModel();
        saveStl(objModel, model);

    }
    else if(fileName.find(".stl") < fileName.length() || fileName.find(".STL") < fileName.length()){
/*
        OBJModel objModel = OBJModel(fileName, percent);
        IndexedModel model = objModel.ToIndexedModel();
        saveStl(objModel, model);
*/
        Geometry tessel;
        //  fill up the tesselation object with STL data (load STL)
        tessel.visit(ImportSTL(fileName));
        //  write down the tesselation object into OBJ file (save OBJ)
        tessel.visit(ExportOBJ(".temp.obj"));
        
        OBJModel objModel = OBJModel(".temp.obj", simplifyFlag, percent);
        IndexedModel model = objModel.ToIndexedModel();
        saveStl(objModel, model);
        


    }

//	InitMesh(model);

}



void Mesh::InitMesh(const IndexedModel& model)
{
    m_numIndices = model.indices.size();

    glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.texCoords[0]) * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.colors[0]) * model.colors.size(), &model.colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
   

	for(unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(*vertices[i].GetPos());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
		model.normals.push_back(*vertices[i].GetNormal());
		model.colors.push_back(*vertices[i].GetColor());
	}
	
	for(unsigned int i = 0; i < numIndices; i++)
        model.indices.push_back(indices[i]);

    InitMesh(model);
}

Mesh::~Mesh()
{
//	glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
//	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::Draw(int mode)
{
	glBindVertexArray(m_vertexArrayObject);
	glDrawElementsBaseVertex(mode, m_numIndices, GL_UNSIGNED_INT, 0, 0);
	glBindVertexArray(0);
}

void Mesh::saveStl(OBJModel& objModel, IndexedModel& indexedModel,const char*  outPath) {
    char partName[80];

    FILE* fp;
    int err = fopen_s(&fp, outPath, "wb");
    if (err != 0) {
        return;
    }
    float* data = new float[12];
    int indexNum = indexedModel.indices.size();
    int faceNum = indexNum / 3;

    fwrite(partName, sizeof(char), 80, fp);
    fwrite(&faceNum, sizeof(int), 1, fp);
    char buf[2];

    for (int i = 0; i < indexNum; i += 3) {
        std::vector<glm::vec3>position;
        position.push_back(indexedModel.positions[indexedModel.indices[i]]);
        position.push_back(indexedModel.positions[indexedModel.indices[i + 1]]);
        position.push_back(indexedModel.positions[indexedModel.indices[i + 2]]);

        glm::vec3 normal = calNormal(position);
        data[0] = normal.x;
        data[1] = normal.y;
        data[2] = normal.z;


        data[3] = position[0].x;
        data[4] = position[0].y;
        data[5] = position[0].z;
        data[6] = position[1].x;
        data[7] = position[1].y;
        data[8] = position[1].z;
        data[9] = position[2].x;
        data[10] = position[2].y;
        data[11] = position[2].z;

        fwrite(data, sizeof(float), 12, fp);
        fwrite(buf, sizeof(char), 2, fp);
    }
    fclose(fp);

    delete[]data;

}

void Mesh::saveStl(OBJModel& objModel, IndexedModel& indexedModel) {
    char partName[80];

    FILE* fp;
    int err = fopen_s(&fp,"test.stl", "wb");
    if (err != 0) {
        return;
    }
    float* data = new float[12];
    int indexNum = indexedModel.indices.size();
    int faceNum = indexNum / 3;

    fwrite(partName, sizeof(char), 80, fp);
    fwrite(&faceNum, sizeof(int), 1, fp);
    char buf[2];
    
    for (int i = 0; i < indexNum; i+=3) {
        std::vector<glm::vec3>position;
        position.push_back(indexedModel.positions[indexedModel.indices[i]]);
        position.push_back(indexedModel.positions[indexedModel.indices[i+1]]);
        position.push_back(indexedModel.positions[indexedModel.indices[i+2]]);

        glm::vec3 normal= calNormal(position);
        data[0] = normal.x;
        data[1] = normal.y;
        data[2] = normal.z;


        data[3] = position[0].x;
        data[4] = position[0].y;
        data[5] = position[0].z;
        data[6] = position[1].x;
        data[7] = position[1].y;
        data[8] = position[1].z;
        data[9] = position[2].x;
        data[10] = position[2].y;
        data[11] = position[2].z;

        fwrite(data, sizeof(float), 12, fp);
        fwrite(buf, sizeof(char), 2, fp);
    }
    fclose(fp);

    delete[]data;

}

