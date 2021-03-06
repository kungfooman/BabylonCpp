#ifndef BABYLON_CAMERAS_CAMERA_H
#define BABYLON_CAMERAS_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>
#include <babylon/engine/node.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/math/vector3.h>
#include <babylon/math/viewport.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class Camera;
struct ICullable;
class FreeCamera;
class PostProcess;
class Ray;
class RenderTargetTexture;
using CameraPtr              = std::shared_ptr<Camera>;
using FreeCameraPtr          = std::shared_ptr<FreeCamera>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

class BABYLON_SHARED_EXPORT Camera : public Node {

private:
  /** Statics **/
  static constexpr unsigned int _PERSPECTIVE_CAMERA  = 0;
  static constexpr unsigned int _ORTHOGRAPHIC_CAMERA = 1;

  static constexpr unsigned int _FOVMODE_VERTICAL_FIXED   = 0;
  static constexpr unsigned int _FOVMODE_HORIZONTAL_FIXED = 1;

  static constexpr unsigned int _RIG_MODE_NONE                             = 0;
  static constexpr unsigned int _RIG_MODE_STEREOSCOPIC_ANAGLYPH            = 10;
  static constexpr unsigned int _RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL = 11;
  static constexpr unsigned int _RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED
    = 12;
  static constexpr unsigned int _RIG_MODE_STEREOSCOPIC_OVERUNDER = 13;
  static constexpr unsigned int _RIG_MODE_VR                     = 20;
  static constexpr unsigned int _RIG_MODE_WEBVR                  = 21;

public:
  static constexpr unsigned int PERSPECTIVE_CAMERA()
  {
    return Camera::_PERSPECTIVE_CAMERA;
  }

  static constexpr unsigned int ORTHOGRAPHIC_CAMERA()
  {
    return Camera::_ORTHOGRAPHIC_CAMERA;
  }

  /**
   * @brief This is the default FOV mode for perspective cameras.
   * This setting aligns the upper and lower bounds of the viewport to the upper
   * and lower bounds of the camera frustum.
   *
   */
  static constexpr unsigned int FOVMODE_VERTICAL_FIXED()
  {
    return Camera::_FOVMODE_VERTICAL_FIXED;
  }

  /**
   * @brief This setting aligns the left and right bounds of the viewport to the
   * left and right bounds of the camera frustum.
   *
   */
  static constexpr unsigned int FOVMODE_HORIZONTAL_FIXED()
  {
    return Camera::_FOVMODE_HORIZONTAL_FIXED;
  }

  static constexpr unsigned int RIG_MODE_NONE()
  {
    return Camera::_RIG_MODE_NONE;
  }

  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_ANAGLYPH()
  {
    return Camera::_RIG_MODE_STEREOSCOPIC_ANAGLYPH;
  }

  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL()
  {
    return Camera::_RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL;
  }

  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED()
  {
    return Camera::_RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED;
  }

  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_OVERUNDER()
  {
    return Camera::_RIG_MODE_STEREOSCOPIC_OVERUNDER;
  }

  static constexpr unsigned int RIG_MODE_VR()
  {
    return Camera::_RIG_MODE_VR;
  }

  static constexpr unsigned int RIG_MODE_WEBVR()
  {
    return Camera::_RIG_MODE_WEBVR;
  }

  static bool ForceAttachControlToAlwaysPreventDefault;
  static bool UseAlternateWebVRRendering;

public:
  virtual ~Camera() override;

  virtual IReflect::Type type() const override;
  void addToScene(const CameraPtr& newCamera);

  /**
   * @brief Store current camera state (fov, position, etc..).
   */
  virtual Camera& storeState();

  /**
   * @brief Restored camera state. You must call storeState() first.
   */
  bool restoreState();

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  std::string toString(bool fullDetails = false) const;

  std::vector<AbstractMeshPtr>& getActiveMeshes();
  bool isActiveMesh(const AbstractMeshPtr& mesh);

  /**
   * @brief Is this camera ready to be used/rendered.
   * @param completeCheck defines if a complete check (including post processes)
   * has to be done (false by default)
   * @return true if the camera is ready
   */
  bool isReady(bool completeCheck        = false,
               bool forceInstanceSupport = false) override;

  /** Cache **/
  /** Hidden */
  virtual void _initCache() override;
  /** Hidden */
  void _updateCache(bool ignoreParentClass = false) override;

  /** Synchronized **/
  /** Hidden */
  bool _isSynchronized() override;
  /** Hidden */
  virtual bool _isSynchronizedViewMatrix();
  bool _isSynchronizedProjectionMatrix();

  /** Controls **/
  virtual void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                             bool useCtrlForPanning = true,
                             MouseButtonType panningMouseButton
                             = MouseButtonType::RIGHT);
  virtual void detachControl(ICanvas* canvas);
  void update();
  /** Hidden */
  virtual void _checkInputs();
  std::vector<CameraPtr>& rigCameras();
  const std::vector<CameraPtr>& rigCameras() const;
  PostProcess* rigPostProcess();

  /**
   * @brief Internal, gets the first post proces.
   * @returns the first post process to be run on this camera.
   */
  PostProcess* _getFirstPostProcess() const;

  int attachPostProcess(PostProcess* postProcess, int insertAt = -1);
  void detachPostProcess(PostProcess* postProcess);
  Matrix* getWorldMatrix() override;
  /** Hidden */
  virtual Matrix _getViewMatrix();
  Matrix& getViewMatrix(bool force = false);
  void freezeProjectionMatrix();
  void freezeProjectionMatrix(const Matrix& projection);
  void unfreezeProjectionMatrix();
  Matrix& getProjectionMatrix(bool force = false);

  /**
   * @brief Gets the transformation matrix (ie. the multiplication of view by
   * projection matrices).
   * @returns a Matrix
   */
  Matrix& getTransformationMatrix();

  bool isInFrustum(ICullable* target);
  bool isCompletelyInFrustum(ICullable* target);
  Ray getForwardRay(float length = 100.f);
  Ray getForwardRay(float length, const Matrix& transform);
  Ray getForwardRay(float length, const Matrix& transform,
                    const Vector3& origin);

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

  /** Camera rigs section **/
  FreeCameraPtr leftCamera();
  FreeCameraPtr rightCamera();
  Vector3* getLeftTarget();
  Vector3* getRightTarget();
  void setCameraRigMode(int mode, const Json::value& rigParams);
  void setCameraRigParameter(const std::string& name, float value);
  virtual CameraPtr createRigCamera(const std::string& name, int cameraIndex);
  /** Hidden */
  virtual void _updateRigCameras();
  /** Hidden */
  virtual void _setupInputs();
  virtual Json::object serialize() const;
  virtual const std::string getClassName() const override;
  Camera* clone(const std::string& name);
  Vector3 getDirection(const Vector3& localAxis);
  void getDirectionToRef(const Vector3& localAxis, Vector3& result);
  Matrix& computeWorldMatrix(bool force = false) override;

  // Statics
  static CameraPtr GetConstructorFromName(const std::string& type,
                                          const std::string& name, Scene* scene,
                                          float interaxial_distance     = 0.f,
                                          bool isStereoscopicSideBySide = true);
  static CameraPtr Parse(const Json::value& parsedCamera, Scene* scene);

protected:
  Camera(const std::string& name, const Vector3& position, Scene* scene,
         bool setActiveOnSceneIfNoneActive = true);

  virtual bool _restoreStateValues();

private:
  Vector3& get_globalPosition();
  void _cascadePostProcessesToRigCams();
  void updateFrustumPlanes();
  Matrix& _getVRProjectionMatrix();
  virtual void _updateCameraRotationMatrix();
  virtual void _updateWebVRCameraRotationMatrix();
  virtual Matrix& _getWebVRProjectionMatrix();
  virtual Matrix& _getWebVRViewMatrix();

public:
  /** Members **/
  CameraInputsManager<Camera> inputs;

  Vector3 position;

  /**
   * The vector the camera should consider as up.
   * (default is Vector3(0, 1, 0) aka Vector3.Up())
   */
  Vector3 upVector;

  float orthoLeft;
  float orthoRight;
  float orthoBottom;
  float orthoTop;

  /**
   * FOV is set in Radians. (default is 0.8)
   */
  float fov;

  float minZ;
  float maxZ;
  float inertia;
  unsigned int mode;
  bool isIntermediate;
  Viewport viewport;

  /**
   * Restricts the camera to viewing objects with the same layerMask.
   * A camera with a layerMask of 1 will render mesh.layerMask &
   * camera.layerMask!== 0
   */
  unsigned int layerMask;

  /**
   * fovMode sets the camera frustum bounds to the viewport bounds. (default is
   * FOVMODE_VERTICAL_FIXED)
   */
  unsigned int fovMode;

  /** Camera rig members **/
  unsigned int cameraRigMode;
  float interaxialDistance;
  bool isStereoscopicSideBySide;
  /** Hidden */
  CameraRigParams _cameraRigParams;
  /** Hidden */
  std::vector<CameraPtr> _rigCameras;
  /** Hidden */
  PostProcess* _rigPostProcess;
  /** Hidden */
  bool _skipRendering;
  /** Hidden */
  Camera* _alternateCamera;
  std::vector<RenderTargetTexturePtr> customRenderTargets;

  /** Observables **/
  Observable<Camera> onViewMatrixChangedObservable;
  Observable<Camera> onProjectionMatrixChangedObservable;
  Observable<Camera> onAfterCheckInputsObservable;
  Observable<Camera> onRestoreStateObservable;

  /** Cache **/
  Matrix _projectionMatrix;
  std::vector<PostProcess*> _postProcesses;
  Uint32Array _postProcessesTakenIndices;
  std::vector<AbstractMeshPtr> _activeMeshes;

  ReadOnlyProperty<Camera, Vector3> globalPosition;

protected:
  Matrix _webvrViewMatrix;
  Vector3 _globalPosition;

private:
  Matrix _computedViewMatrix;
  bool _doNotComputeProjectionMatrix;
  std::unique_ptr<Matrix> _worldMatrix;
  Matrix _transformMatrix;
  Matrix _webvrProjectionMatrix;
  std::array<Plane, 6> _frustumPlanes;
  bool _refreshFrustumPlanes;
  float _storedFov;
  bool _stateStored;
  bool _setActiveOnSceneIfNoneActive;

}; // end of class Camera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_H
