#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a new torus geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#torus
 */
class BABYLON_SHARED_EXPORT TorusGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static TorusGeometryPtr New(Ts&&... args)
  {
    auto mesh = shared_ptr_t<TorusGeometry>(
      new TorusGeometry(::std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~TorusGeometry() override;

  /**
   * @brief Hidden
   * @return
   */
  unique_ptr_t<VertexData> _regenerateVertexData() override;

  GeometryPtr copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static TorusGeometryPtr Parse(const Json::value& parsedTorus, Scene* scene);

protected:
  /**
   * @brief Creates a new torus geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param diameter defines the diameter of the torus
   * @param thickness defines the thickness of the torus (ie. internal diameter)
   * @param tessellation defines the tesselation factor to apply to the torus
   * (number of segments along the circle)
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  TorusGeometry(const string_t& id, Scene* scene, float diameter,
                float thickness, unsigned int tessellation,
                bool canBeRegenerated, Mesh* mesh = nullptr,
                unsigned int side = Mesh::DEFAULTSIDE());

private:
  /**
   * Defines the diameter of the torus
   */
  float diameter;

  /**
   * Defines the thickness of the torus (ie. internal diameter)
   */
  float thickness;

  /**
   * Defines the tesselation factor to apply to the torus
   */
  unsigned int tessellation;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class TorusGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_GEOMETRY_H
