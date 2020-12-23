#pragma once

#include <unordered_map>
#include <string>

#define CUBE_STATIC_MESH std::string("__cube_mesh")

class Core;
struct StaticMesh;

class AssetStorage
{
public:
  AssetStorage(Core& vkCore);

private:
  void InitializeBasicMeshes();

private:
  Core& vkCore;

  std::unordered_map<std::string, StaticMesh> staticMeshes;
};