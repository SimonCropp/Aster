#pragma once

#include <ecs/BaseSystems.h>

#include <string>

namespace YAML
{
  class Node;
}

class Context;

class LevelInitializationSystem : public InitializationSystem
{
public:
  LevelInitializationSystem(Context* pContext, const std::string& levelYaml);

  virtual void Initialize() override;

private:
  void LoadMeshes(const YAML::Node& config);
  void CreateObjects(const YAML::Node& config);

private:
  std::string levelYaml;
};