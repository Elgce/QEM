#include "obj_loader.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>

static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end);
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> SplitString(const std::string &s, char delim);

// OBJModel ctor
OBJModel::OBJModel(const std::string& fileName, int simplifyFlag, double percent){

	hasUVs = false;
	hasNormals = false;
    std::ifstream file;
    file.open(fileName.c_str());
	int bla1;
    std::string line;

    if(file.is_open()){
        while(file.good())
		{
            getline(file, line);
            unsigned int lineLength = line.length();
            if(lineLength < 2)
                continue;

            const char* lineCStr = line.c_str();
            
            switch(lineCStr[0])            
			{
                case 'v':
					if (lineCStr[1] == 't')
					{
						this->uvs.push_back(ParseOBJVec2(line));
					}
                    else if(lineCStr[1] == 'n')
					{
                        this->normals.push_back(ParseOBJVec3(line));
						this->colors.push_back((normals.back() + glm::vec3(1)) * 0.5f);
					}
					else if(lineCStr[1] == ' ' || lineCStr[1] == '\t')
                        this->vertices.push_back(ParseOBJVec3(line));
                break;
                case 'f':
                    CreateOBJFace(line);
                break;
                default: break;
            };
        }

		if (simplifyFlag == 1)
		{
			MeshSimplification meshSimplification(OBJIndices, vertices, percent);
			this->OBJIndices = meshSimplification.getIndices();
			this->vertices = meshSimplification.getVertices();
		}


	}
    else
        std::cerr << "Unable to load mesh: " << fileName << std::endl;
}

OBJModel::OBJModel(const std::string& stlFileName, double percent){
    hasUVs = false;
    hasNormals = false;

    FILE* fStl = nullptr;
    int err = fopen_s(&fStl, stlFileName.c_str(), "r");
    if (fStl == nullptr || err != 0) {
        std::cerr << "Unable to load mesh: " << stlFileName << std::endl;
        return;
    }

    char buf[6] = {};
    if (fread(buf, 5, 1, fStl) != 1) {
        fclose(fStl);
        std::cerr << "Unable to load mesh: " << stlFileName << std::endl;
        return;
    }
    fclose(fStl);

    if (strcmp(buf, "solid") == 0) { //是ASCII STL
        readSTLASCII(stlFileName);
    }
    else {
        readSTLBinary(stlFileName);
    }
        
    MeshSimplification meshSimplification(OBJIndices, vertices, percent);
    this->OBJIndices = meshSimplification.getIndices();
    this->vertices = meshSimplification.getVertices();
}

void OBJModel::readSTLASCII(const std::string& fileName){
}

void OBJModel::readSTLBinary(const std::string& fileName){
    FILE* fStl = nullptr;
    int err = fopen_s(&fStl, fileName.c_str(), "rb");
    char partName[80];
    unsigned int faceNum;
    if (fStl == nullptr || err != 0 || fread(partName, 80, 1, fStl) != 1 || fread(&faceNum, 4, 1, fStl) != 1) {
        return;
    }

    int vertexId = 0;
    int faceId = 0;
    float v1, v2, v3;
    char buf[2];
    for (int i = 0; i < faceNum; i++) {

        fread(&v1, 4, 1, fStl);//读取法线数据
        fread(&v2, 4, 1, fStl);
        fread(&v3, 4, 1, fStl);

/*

        for (int j = 0; j < 3; j++) {
            fread(&v1, 4, 1, fStl);//读取顶点的数据
            fread(&v2, 4, 1, fStl);
            fread(&v3, 4, 1, fStl);
            glm::vec3 p = glm::vec3((float)v1, (float)v2, (float)v3);

            int id = findVertexByPoint(p);
            if (id == -1) {			//没找到，该point是新的
                this->vertices.push_back(p);
            }
        }

        fread(buf, 2, 1, fStl);//读取保留项数据，这一项一般没什么用，这里选择读取是为了移动文件指针
        generateEdge(face);


        faces.push_back(face);
        faceId++;
*/
    }
}

void OBJModel::CalcNormals(){
	float *count = new float[normals.size()];
	for (int i = 0; i < normals.size(); i++){
		count[i] = 0;
	}
	for (std::list<OBJIndex>::iterator it = OBJIndices.begin(); it !=OBJIndices.end(); it++){
/*		int i0 = (*it).vertexIndex;
		(*it).normalIndex = i0;
		it++;
		int i1 = (*it).vertexIndex;
		(*it).normalIndex = i1;
		it++;
        int i2 = (*it).vertexIndex;
		(*it).normalIndex = i2;
*/      int i0 = (*it).vertexIndex;
        int ni0 = (*it).normalIndex;
        it++;
        int i1 = (*it).vertexIndex;
        int ni1 = (*it).normalIndex;
        it++;
        int i2 = (*it).vertexIndex;
        int ni2 = (*it).normalIndex;
		glm::vec3 v1 = vertices[i1] - vertices[i0];
		glm::vec3 v2 = vertices[i2] - vertices[i0];
        glm::vec3 normal = glm::cross(v2, v1);
        if(normal != glm::vec3(0,0,0))
		    normal = glm::normalize(normal);
/*
		if (count[i0] == 0)
			count[i0] = 1.0f;
		else
			count[i0] = count[i0] / (count[i0] + 1);

		if (count[i1] == 0)
			count[i1] = 1.0f;
		else
			count[i1] = count[i1] / (count[i1] + 1);

		if (count[i2] == 0)
			count[i2] = 1.0f;
		else
			count[i2] = count[i2] / (count[i2] + 1);
		
		normals[i0] += normal;
		normals[i1] += normal;
		normals[i2] += normal;
*/        if (count[ni0] == 0)
            count[ni0] = 1.0f;
        else
            count[ni0] = count[ni0] / (count[ni0] + 1);

        if (count[ni1] == 0)
            count[ni1] = 1.0f;
        else
            count[ni1] = count[ni1] / (count[ni1] + 1);

        if (count[ni2] == 0)
            count[ni2] = 1.0f;
        else
            count[ni2] = count[ni2] / (count[ni2] + 1);

        normals[ni0] += normal;
        normals[ni1] += normal;
        normals[ni2] += normal;
	}
	for (int i = 0; i < normals.size(); i++){
		normals[i] = normals[i] * count[i];
	}
	delete count;
}
 
IndexedModel OBJModel::ToIndexedModel(){
    IndexedModel result;
    IndexedModel normalModel;
	IndexedModel simpleResult;
    
    unsigned int numIndices = OBJIndices.size();
    std::vector<OBJIndex*> indexLookup;
 
	if(!hasNormals){
		for (int i = 0; i < vertices.size(); i++){
		    normals.push_back(glm::vec3(0,0,0));
		}
		hasNormals = true;
	}
    if (normals.size() < maxNormal) {
        for (int i = normals.size(); i < maxNormal; i++) {
            normals.push_back(glm::vec3(0, 0, 0));
        }
    }
	CalcNormals();

 	for(OBJIndex &it1 : OBJIndices){
       indexLookup.push_back(&it1);
	}
    
	std::sort(indexLookup.begin(), indexLookup.end(), CompareOBJIndexPtr);
    std::map<OBJIndex, unsigned int> normalModelIndexMap;
    std::map<unsigned int, unsigned int> indexMap;

    for(OBJIndex &it1 : OBJIndices){
        OBJIndex* currentIndex = &it1;
  
        glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
        glm::vec2 currentTexCoord;
        glm::vec3 currentNormal;
        glm::vec3 currentColor;

        if(hasUVs && currentIndex->uvIndex != -1)
            currentTexCoord = uvs[currentIndex->uvIndex];
        else
            currentTexCoord = glm::vec2(0,0);
            
		currentNormal = normals[currentIndex->normalIndex];
		currentColor = normals[currentIndex->normalIndex];
        
		unsigned int normalModelIndex;
        unsigned int resultModelIndex;
        
        //Create model to properly generate normals on
        std::map<OBJIndex, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
        if(it == normalModelIndexMap.end()){
            normalModelIndex = normalModel.positions.size();
            normalModelIndexMap.insert(std::pair<OBJIndex, unsigned int>(*currentIndex, normalModelIndex));
            normalModel.positions.push_back(currentPosition);
            normalModel.texCoords.push_back(currentTexCoord);
            normalModel.normals.push_back(currentNormal);
			normalModel.colors.push_back(currentColor);
        }
        else
            normalModelIndex = it->second;
        
        //Create model which properly separates texture coordinates
        unsigned int previousVertexLocation = FindLastVertexIndex(indexLookup, currentIndex, result);
        
        if(previousVertexLocation == (unsigned int)-1){
            resultModelIndex = result.positions.size();
            result.positions.push_back(currentPosition);
            result.texCoords.push_back(currentTexCoord);
            result.normals.push_back(currentNormal);
			result.colors.push_back(currentColor);
        }
        else
            resultModelIndex = previousVertexLocation;
        
        normalModel.indices.push_back(normalModelIndex);
        result.indices.push_back(resultModelIndex);
        indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
    }
    return result;
}


int OBJModel::findVertexByPoint(glm::vec3 p){
    auto it = m_hash_point.find(p);
    if (it != m_hash_point.end()) {
        return it->second;
    }
    else {
        return -1;
    }
}

unsigned int OBJModel::FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result)
{
    unsigned int start = 0;
    unsigned int end = indexLookup.size();
    unsigned int current = (end - start) / 2 + start;
    unsigned int previous = start;
    
    while(current != previous)
    {
        OBJIndex* testIndex = indexLookup[current];
        
        if(testIndex->vertexIndex == currentIndex->vertexIndex)
        {
            unsigned int countStart = current;
        
            for(unsigned int i = 0; i < current; i++)
            {
                OBJIndex* possibleIndex = indexLookup[current - i];
                
                if(possibleIndex == currentIndex)
                    continue;
                    
                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                    
                countStart--;
            }
            
            for(unsigned int i = countStart; i < indexLookup.size() - countStart; i++)
            {
                OBJIndex* possibleIndex = indexLookup[current + i];
                
                if(possibleIndex == currentIndex)
                    continue;
                    
                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                else if((!hasUVs || possibleIndex->uvIndex == currentIndex->uvIndex) 
                    && (!hasNormals || possibleIndex->normalIndex == currentIndex->normalIndex))
                {
                    glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
                    glm::vec2 currentTexCoord;
                    glm::vec3 currentNormal;
                    glm::vec3 currentColor;

                    if(hasUVs && currentIndex->uvIndex != -1)
                        currentTexCoord = uvs[currentIndex->uvIndex];
                    else
                        currentTexCoord = glm::vec2(0,0);
                        
                    if(hasNormals)
					{
                        currentNormal = normals[currentIndex->normalIndex];
						currentColor =  normals[currentIndex->normalIndex];
					}
					else
                    {
						currentNormal = glm::vec3(0,0,0);
						currentColor = glm::normalize(glm::vec3(1,1,1));
					}
                    for(unsigned int j = 0; j < result.positions.size(); j++)
                    {
                        if(currentPosition == result.positions[j] 
                            && ((!hasUVs || currentTexCoord == result.texCoords[j])
                            && (!hasNormals || currentNormal == result.normals[j])))
                        {
                            return j;
                        }
                    }
                }
            }       
            return -1;
        }
        else
        {
            if(testIndex->vertexIndex < currentIndex->vertexIndex)
                start = current;
            else
                end = current;
        }
    
        previous = current;
        current = (end - start) / 2 + start;
    }
    
    return -1;
}

void OBJModel::CreateOBJFace(const std::string& line)
{
    std::vector<std::string> tokens = SplitString(line, ' ');
	unsigned int tmpIndex = OBJIndices.size();

	OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals));

	OBJIndices.push_back(ParseOBJIndex(tokens[2], &this->hasUVs, &this->hasNormals));

	OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals));

	if((int)tokens.size() > 4 && tokens[4] != "" && tokens[4] != " " )//triangulation
    {	
		OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals));
		
		OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals));
		
		OBJIndices.push_back(ParseOBJIndex(tokens[4], &this->hasUVs, &this->hasNormals));
    }
}

OBJIndex OBJModel::ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals)
{
    unsigned int tokenLength = token.length();
    const char* tokenString = token.c_str();
    
    unsigned int vertIndexStart = 0;
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    OBJIndex result;
    result.vertexIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    result.uvIndex = 0;
    result.normalIndex = 0;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart-1, tokenString, tokenLength, '/');
    
    result.uvIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    *hasUVs = true;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart-1, tokenString, tokenLength, '/');
    
    result.normalIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    if (result.normalIndex > maxNormal) {
        maxNormal = result.normalIndex;
    }
    *hasNormals = true;
    
    return result;
}

glm::vec3 OBJModel::ParseOBJVec3(const std::string& line) 
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 2;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec3(x,y,z);	
}

glm::vec2 OBJModel::ParseOBJVec2(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 3;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec2(x,y);
}

static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b)
{
    return a->vertexIndex < b->vertexIndex;
}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
    unsigned int result = start;
    while(result < length)
    {
        result++;
        if(str[result] == token)
            break;
    }
    
    return result;
}

static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atoi(token.substr(start, end - start).c_str()) - 1;
}

static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atof(token.substr(start, end - start).c_str());
}

static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;
        
    const char* cstr = s.c_str();
    unsigned int strLength = s.length();
    unsigned int start = 0;
    unsigned int end = 0;
        
    while(end <= strLength)
    {
        while(end <= strLength)
        {
            if(cstr[end] == delim)
                break;
            end++;
        }
            
        elems.push_back(s.substr(start, end - start));
        start = end + 1;
        end = start;
    }
        
    return elems;
}

