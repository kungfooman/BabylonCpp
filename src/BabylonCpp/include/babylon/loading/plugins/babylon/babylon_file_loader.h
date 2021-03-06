#ifndef BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BabylonFileLoader : public ISceneLoaderPlugin {

  BabylonFileLoader();
  virtual ~BabylonFileLoader();

  MaterialPtr parseMaterialById(const string_t& id,
                                const Json::value& parsedData, Scene* scene,
                                const string_t& rootUrl) const;
  bool isDescendantOf(const Json::value& mesh, const vector_t<string_t>& names,
                      vector_t<string_t>& hierarchyIds) const;
  string_t logOperation(const string_t& operation) const;
  string_t logOperation(const string_t& operation,
                        const Json::value& producer) const;

  bool
  importMesh(const vector_t<string_t>& meshesNames, Scene* scene,
             const string_t& data, const string_t& rootUrl,
             vector_t<AbstractMeshPtr>& meshes,
             vector_t<IParticleSystemPtr>& particleSystems,
             vector_t<SkeletonPtr>& skeletons,
             const ::std::function<void(const string_t& message,
                                        const string_t& exception)>& onError
             = nullptr) const override;
  bool load(Scene* scene, const string_t& data, const string_t& rootUrl,
            const ::std::function<void(const string_t& message,
                                       const string_t& exception)>& onError
            = nullptr) const override;

}; // end of struct BabylonFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_BABYLON_BABYLON_FILE_LOADER_H
