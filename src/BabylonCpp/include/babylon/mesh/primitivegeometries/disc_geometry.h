#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_DISC_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_DISC_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a disc geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#disc-or-regular-polygon
 */
class BABYLON_SHARED_EXPORT DiscGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static DiscGeometryPtr New(Ts&&... args)
  {
    auto mesh = shared_ptr_t<DiscGeometry>(
      new DiscGeometry(::std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~DiscGeometry() override;

  /**
   * @brief Hidden
   * @return
   */
  unique_ptr_t<VertexData> _regenerateVertexData() override;

  GeometryPtr copy(const string_t& id) override;

protected:
  /**
   * @brief Creates a new disc geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param radius defines the radius of the disc
   * @param tessellation defines the tesselation factor to apply to the disc
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  DiscGeometry(const string_t& id, Scene* scene, float radius,
               unsigned int tessellation, bool canBeRegenerated,
               Mesh* mesh = nullptr, unsigned int side = Mesh::DEFAULTSIDE());

public:
  /**
   * Defines the radius of the disc
   */
  float radius;

  /**
   * Defines the tesselation factor to apply to the disc
   */
  unsigned int tessellation;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class DiscGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_DISC_GEOMETRY_H
