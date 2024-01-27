#ifndef PREFAB_H
#define PREFAB_H

enum PrefabType
{
	SQUARE, CUBE
};

template<PrefabType type>
struct Vertex;

template<PrefabType type>
struct Prefab;

template<PrefabType type>
void setVAOFormat(unsigned int vaoID);
template<PrefabType type>
void attachVBO(unsigned int vaoID, unsigned int vboID, size_t start);
template<PrefabType type>
void attachEBO(unsigned int vaoID, unsigned int eboID);

template<PrefabType type>
void createPrefab(Prefab<type>& prefab);

#endif