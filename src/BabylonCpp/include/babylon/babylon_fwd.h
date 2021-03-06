#ifndef BABYLON_FWD_H
#define BABYLON_FWD_H

#include <babylon/babylon_stl.h>

// --- Forward Declarations --- //

namespace BABYLON {
// --- Actions ---
class Action;
class ActionEvent;
class ActionManager;
class Condition;
// --- Actions aliases ---
using ActionManagerPtr = shared_ptr_t<ActionManager>;
// - Conditions
class PredicateCondition;
class StateCondition;
class ValueCondition;
// - DirectActions
class CombineAction;
class DoNothingAction;
class ExecuteCodeAction;
class IncrementValueAction;
class PlayAnimationAction;
class PlaySoundAction;
class SetParentAction;
class SetStateAction;
class SetValueAction;
class StopAnimationAction;
class StopSoundAction;
class SwitchBooleanAction;
// - ValueActions
class InterpolateValueAction;
// --- Animations ---
class Animatable;
class Animation;
class AnimationEvent;
class AnimationGroup;
struct AnimationPropertiesOverride;
class AnimationRange;
class AnimationValue;
class IAnimatable;
struct IAnimationKey;
class PathCursor;
class RuntimeAnimation;
struct TargetedAnimation;
// --- Animations aliases ---
using AnimatablePtr       = shared_ptr_t<Animatable>;
using AnimationPtr        = shared_ptr_t<Animation>;
using AnimationGroupPtr   = shared_ptr_t<AnimationGroup>;
using IAnimatablePtr      = shared_ptr_t<IAnimatable>;
using RuntimeAnimationPtr = shared_ptr_t<RuntimeAnimation>;
// - Easing
class BackEase;
class BezierCurveEase;
class BounceEase;
class CircleEase;
class CubicEase;
class EasingFunction;
class ElasticEase;
class ExponentialEase;
class IEasingFunction;
class PowerEase;
class QuadraticEase;
class QuarticEase;
class QuinticEase;
class SineEase;
// - Easing aliases
using BackEasePtr        = shared_ptr_t<BackEase>;
using BezierCurveEasePtr = shared_ptr_t<BezierCurveEase>;
using BounceEasePtr      = shared_ptr_t<BounceEase>;
using CircleEasePtr      = shared_ptr_t<CircleEase>;
using CubicEasePtr       = shared_ptr_t<CubicEase>;
using EasingFunctionPtr  = shared_ptr_t<EasingFunction>;
using ElasticEasePtr     = shared_ptr_t<ElasticEase>;
using ExponentialEasePtr = shared_ptr_t<ExponentialEase>;
using IEasingFunctionPtr = shared_ptr_t<IEasingFunction>;
using PowerEasePtr       = shared_ptr_t<PowerEase>;
using QuadraticEasePtr   = shared_ptr_t<QuadraticEase>;
using QuarticEasePtr     = shared_ptr_t<QuarticEase>;
using QuinticEasePtr     = shared_ptr_t<QuinticEase>;
using SineEasePtr        = shared_ptr_t<SineEase>;
// --- Audio ---
class Analyser;
class AudioEngine;
class Sound;
class SoundTrack;
class WeightedSound;
// --- Audio aliases ---
using SoundPtr = shared_ptr_t<Sound>;
// --- Behaviors ---
template <class T>
struct Behavior;
template <class T>
struct IBehaviorAware;
// - Cameras
class AutoRotationBehavior;
class BouncingBehavior;
class FramingBehavior;
// - Mesh
class AttachToBoxBehavior;
struct FaceDirectionInfo;
class FadeInOutBehavior;
class MultiPointerScaleBehavior;
class PointerDragBehavior;
struct PointerDragBehaviorOptions;
class SixDofDragBehavior;
// --- Bones ---
class Bone;
class BoneIKController;
class BoneLookController;
class Skeleton;
// --- Bones aliases ---
using BonePtr     = shared_ptr_t<Bone>;
using SkeletonPtr = shared_ptr_t<Skeleton>;
// --- Cameras ---
class ArcFollowCamera;
class ArcRotateCamera;
struct ArcRotateCameraInputsManager;
class Camera;
template <class TCamera>
class CameraInputsManager;
class DeviceOrientationCamera;
class FollowCamera;
class FreeCamera;
struct FreeCameraInputsManager;
class GamepadCamera;
template <class TCamera>
struct ICameraInput;
class TargetCamera;
class TouchCamera;
class UniversalCamera;
class VirtualJoysticksCamera;
// --- Cameras aliases ---
using ArcFollowCameraPtr         = shared_ptr_t<ArcFollowCamera>;
using ArcRotateCameraPtr         = shared_ptr_t<ArcRotateCamera>;
using CameraPtr                  = shared_ptr_t<Camera>;
using DeviceOrientationCameraPtr = shared_ptr_t<DeviceOrientationCamera>;
using FollowCameraPtr            = shared_ptr_t<FollowCamera>;
using FreeCameraPtr              = shared_ptr_t<FreeCamera>;
using GamepadCameraPtr           = shared_ptr_t<GamepadCamera>;
using TargetCameraPtr            = shared_ptr_t<TargetCamera>;
using TouchCameraPtr             = shared_ptr_t<TouchCamera>;
using UniversalCameraPtr         = shared_ptr_t<UniversalCamera>;
using VirtualJoysticksCameraPtr  = shared_ptr_t<VirtualJoysticksCamera>;
// - Inputs
class ArcRotateCameraKeyboardMoveInput;
class ArcRotateCameraMouseWheelInput;
class ArcRotateCameraPointersInput;
class FreeCameraKeyboardMoveInput;
class FreeCameraGamepadInput;
class FreeCameraMouseInput;
class FreeCameraTouchInput;
// - Stereoscopic
class AnaglyphArcRotateCamera;
class AnaglyphFreeCamera;
class AnaglyphGamepadCamera;
class AnaglyphUniversalCamera;
class StereoscopicArcRotateCamera;
class StereoscopicFreeCamera;
class StereoscopicGamepadCamera;
class StereoscopicUniversalCamera;
// --- Stereoscopic aliases ---
using AnaglyphArcRotateCameraPtr = shared_ptr_t<AnaglyphArcRotateCamera>;
using AnaglyphFreeCameraPtr      = shared_ptr_t<AnaglyphFreeCamera>;
using AnaglyphGamepadCameraPtr   = shared_ptr_t<AnaglyphGamepadCamera>;
using AnaglyphUniversalCameraPtr = shared_ptr_t<AnaglyphUniversalCamera>;
using StereoscopicArcRotateCameraPtr
  = shared_ptr_t<StereoscopicArcRotateCamera>;
using StereoscopicFreeCameraPtr    = shared_ptr_t<StereoscopicFreeCamera>;
using StereoscopicGamepadCameraPtr = shared_ptr_t<StereoscopicGamepadCamera>;
using StereoscopicUniversalCameraPtr
  = shared_ptr_t<StereoscopicUniversalCamera>;
// - VR
struct DevicePose;
struct PoseControlled;
class VRCameraMetrics;
struct VRFov;
// --- Collisions ---
struct BabylonMessage;
struct CollidePayload;
class CollideWorker;
class Collider;
class CollisionCache;
class CollisionCoordinatorLegacy;
class CollisionCoordinatorWorker;
class CollisionDetectorTransferable;
struct CollisionReplyPayload;
struct ICollisionCoordinator;
struct ICollisionDetector;
struct InitPayload;
class IntersectionInfo;
class PickingInfo;
struct SerializedColliderToWorker;
struct SerializedGeometry;
struct SerializedMesh;
struct SerializedSubMesh;
struct UpdatePayload;
class Worker;
struct WorkerReply;
// --- Core ---
class DataView;
struct Image;
struct NodeCache;
// - Logging
class LogChannel;
class LogMessage;
class Logger;
// --- Culling ---
class BoundingBox;
class BoundingInfo;
class BoundingSphere;
struct ICullable;
class Ray;
// - Octrees
template <class T>
struct IOctreeContainer;
template <class T>
class Octree;
template <class T>
class OctreeBlock;
// --- Debug ---
class DebugLayer;
class RayHelper;
namespace Debug {
class AxesViewer;
class BoneAxesViewer;
class PhysicsViewer;
class SkeletonViewer;
} // end of namespace Debug
// --- Engine ---
class AbstractScene;
class AssetContainer;
class ClickInfo;
struct DepthTextureCreationOptions;
class Engine;
struct EngineCapabilities;
struct EngineOptions;
struct IActiveMeshCandidateProvider;
struct InstancingAttributeInfo;
struct ISceneSerializableComponent;
struct ISceneComponent;
class Node;
class Scene;
// --- Engine aliases ---
using AbstractScenePtr  = shared_ptr_t<AbstractScene>;
using AssetContainerPtr = shared_ptr_t<AssetContainer>;
using ISceneSerializableComponentPtr
  = shared_ptr_t<ISceneSerializableComponent>;
using ISceneComponentPtr = shared_ptr_t<ISceneComponent>;
using NodePtr            = shared_ptr_t<Node>;
// --- Events ---
struct KeyboardEventTypes;
class KeyboardInfo;
class KeyboardInfoPre;
class PointerInfo;
class PointerInfoBase;
class PointerInfoPre;
struct RenderingGroupInfo;
// --- Gamepad ---
class Gamepad;
struct GamepadButtonChanges;
class GamepadManager;
struct GamepadPose;
class GamepadSystemSceneComponent;
using GamepadSystemSceneComponentPtr
  = shared_ptr_t<GamepadSystemSceneComponent>;
class GenericPad;
struct StickValues;
class Xbox360Pad;
// - Controllers
class DaydreamController;
class ExtendedGamepadButton;
class GearVRController;
class GenericController;
class MutableGamepadButton;
class OculusTouchController;
class PoseEnabledController;
struct PoseEnabledControllerHelper;
class ViveController;
class WebVRController;
class WindowsMotionController;
// --- Gizmos ---
class AxisDragGizmo;
class AxisScaleGizmo;
class BoundingBoxGizmo;
class Gizmo;
class GizmoManager;
class PlaneRotationGizmo;
class PositionGizmo;
class RotationGizmo;
class ScaleGizmo;
// --- Helpers ---
class EnvironmentHelper;
struct IEnvironmentHelperOptions;
struct ISceneSize;
struct PhotoDomeOptions;
class PhotoDome;
// --- Instrumentation ---
struct _TimeToken;
class EngineInstrumentation;
class SceneInstrumentation;
// --- Interfaces ---
class IBrowserGamepad;
class ICanvas;
class ICanvasRenderingContext2D;
struct IDisposable;
class IReflect;
namespace GL {
struct GLInfo;
class IGLFramebuffer;
class IGLBuffer;
class IGLProgram;
class IGLQuery;
class IGLRenderBuffer;
class IGLRenderingContext;
class IGLShader;
class IGLShaderPrecisionFormat;
class IGLTexture;
class IGLUniformLocation;
class IGLVertexArrayObject;
} // end of namespace GL
class ILoadingScreen;
class IRenderableScene;
// --- Layer ---
class EffectLayerSceneComponent;
class EffectLayer;
class GlowLayer;
class HighlightLayer;
struct IEffectLayerOptions;
struct IGlowLayerOptions;
struct IHighlightLayerExcludedMesh;
struct IHighlightLayerMesh;
struct IHighlightLayerOptions;
class Layer;
class LayerSceneComponent;
// --- Layer aliases ---
using EffectLayerSceneComponentPtr = shared_ptr_t<EffectLayerSceneComponent>;
using EffectLayerPtr               = shared_ptr_t<EffectLayer>;
using GlowLayerPtr                 = shared_ptr_t<GlowLayer>;
using HighlightLayerPtr            = shared_ptr_t<HighlightLayer>;
using LayerPtr                     = shared_ptr_t<Layer>;
using LayerSceneComponentPtr       = shared_ptr_t<LayerSceneComponent>;
// --- Lensflare ---
class LensFlare;
class LensFlareSystem;
class LensFlareSystemSceneComponent;
// --- Lensflare aliases ---
using LensFlareSystemPtr = shared_ptr_t<LensFlareSystem>;
using LensFlareSystemSceneComponentPtr
  = shared_ptr_t<LensFlareSystemSceneComponent>;
// --- Lights ---
class DirectionalLight;
class HemisphericLight;
class IShadowLight;
class Light;
class PointLight;
class ShadowLight;
class SpotLight;
// --- Lights aliases ---
using DirectionalLightPtr = shared_ptr_t<DirectionalLight>;
using HemisphericLightPtr = shared_ptr_t<HemisphericLight>;
using IShadowLightPtr     = shared_ptr_t<IShadowLight>;
using LightPtr            = shared_ptr_t<Light>;
using PointLightPtr       = shared_ptr_t<PointLight>;
using SpotLightPtr        = shared_ptr_t<SpotLight>;
// - Shadows
class ShadowGenerator;
// --- Loading ---
struct IRegisteredPlugin;
struct ISceneLoaderPlugin;
struct ISceneLoaderPluginExtensions;
class ProgressEvent;
class SceneLoader;
class SceneLoaderProgressEvent;
// - Plugins / babylon
struct BabylonFileLoader;
// --- Materials ---
// - Common
class ColorCurves;
class Effect;
struct EffectCreationOptions;
class EffectFallbacks;
class EffectIncludesShadersStore;
class EffectShadersStore;
class FresnelParameters;
struct IImageProcessingConfigurationDefines;
class ImageProcessingConfiguration;
struct IMaterialDefines;
class Material;
struct MaterialDefines;
struct MaterialHelper;
class MultiMaterial;
class PushMaterial;
class ShaderMaterial;
struct ShaderMaterialOptions;
class StandardMaterial;
struct StandardMaterialDefines;
class UniformBuffer;
// - Common aliases
using ColorCurvesPtr      = shared_ptr_t<ColorCurves>;
using MaterialPtr         = shared_ptr_t<Material>;
using MultiMaterialPtr    = shared_ptr_t<MultiMaterial>;
using PushMaterialPtr     = shared_ptr_t<PushMaterial>;
using ShaderMaterialPtr   = shared_ptr_t<ShaderMaterial>;
using StandardMaterialPtr = shared_ptr_t<StandardMaterial>;
// - Background
class BackgroundMaterial;
struct BackgroundMaterialDefines;
// - Background aliases
using BackgroundMaterialPtr = shared_ptr_t<BackgroundMaterial>;
// - PBR
class PBRBaseMaterial;
class PBRBaseSimpleMaterial;
class PBRMaterial;
struct PBRMaterialDefines;
class PBRMetallicRoughnessMaterial;
class PBRSpecularGlossinessMaterial;
// PBR aliases
using PBRMaterialPtr = shared_ptr_t<PBRMaterial>;
// - Textures
class BaseTexture;
class ColorGradingTexture;
class CubeTexture;
struct DummyInternalTextureTracker;
class DynamicTexture;
class HDRCubeTexture;
struct IInternalTextureTracker;
struct IMultiRenderTargetOptions;
class InternalTexture;
struct IRenderTargetOptions;
using RenderTargetCreationOptions = IRenderTargetOptions;
class MirrorTexture;
class MultiRenderTarget;
struct RawCubeTexture;
class RawTexture;
class RawTexture3D;
class RefractionTexture;
class RenderTargetTexture;
class Texture;
// -- Textures aliase
using BaseTexturePtr         = shared_ptr_t<BaseTexture>;
using CubeTexturePtr         = shared_ptr_t<CubeTexture>;
using DynamicTexturePtr      = shared_ptr_t<DynamicTexture>;
using MirrorTexturePtr       = shared_ptr_t<MirrorTexture>;
using MultiRenderTargetPtr   = shared_ptr_t<MultiRenderTarget>;
using RenderTargetTexturePtr = shared_ptr_t<RenderTargetTexture>;
using TexturePtr             = shared_ptr_t<Texture>;
using RawTexturePtr          = shared_ptr_t<RawTexture>;
// - Textures / Procedurals
class CustomProceduralTexture;
class NoiseProceduralTexture;
class ProceduralTexture;
// --- Math ---
class Angle;
class Arc2;
struct Axis;
struct BezierCurve;
class Color3;
class Color4;
class Curve3;
class Frustum;
struct ISize;
class Matrix;
class Path2;
class Path3D;
class PathCursor;
class Plane;
class PositionNormalTextureVertex;
class PositionNormalVertex;
class Quaternion;
class Size;
class SphericalHarmonics;
class SphericalPolynomial;
struct Tmp;
class Vector2;
class Vector3;
class Vector4;
class Viewport;
namespace SIMD {
struct Float32x4;
struct SIMDMatrix;
struct SIMDVector3;
} // end of namespace SIMD
// --- Mesh ---
class _InstancesBatch;
class _VisibleInstances;
class AbstractMesh;
class BaseSubMesh;
class Buffer;
struct BufferPointer;
struct FacetParameters;
class Geometry;
class GroundMesh;
struct IGetSetVerticesData;
class InstancedMesh;
class LinesMesh;
class Mesh;
class MeshBuilder;
class MeshLODLevel;
class SubMesh;
class TransformNode;
class VertexBuffer;
class VertexData;
// -- Mesh aliases --
using AbstractMeshPtr  = shared_ptr_t<AbstractMesh>;
using GroundMeshPtr    = shared_ptr_t<GroundMesh>;
using GeometryPtr      = shared_ptr_t<Geometry>;
using InstancedMeshPtr = shared_ptr_t<InstancedMesh>;
using LinesMeshPtr     = shared_ptr_t<LinesMesh>;
using MeshPtr          = shared_ptr_t<Mesh>;
using SubMeshPtr       = shared_ptr_t<SubMesh>;
using TransformNodePtr = shared_ptr_t<TransformNode>;
// - Options
class BoxOptions;
class CylinderOptions;
class DashedLinesOptions;
class DecalOptions;
class DiscOptions;
class ExtrudeShapeOptions;
class ExtrudeShapeCustomOptions;
class GroundFromHeightMapOptions;
class GroundOptions;
class IcoSphereOptions;
class LatheOptions;
class LinesOptions;
class LineSystemOptions;
class PlaneOptions;
class PolygonOptions;
class PolyhedronOptions;
class RibbonOptions;
class SphereOptions;
class TiledGroundOptions;
class TorusKnotOptions;
class TorusOptions;
class TubeOptions;
// - CSG
namespace CSG {
class CSG;
// class Node;
class Plane;
class Polygon;
class Vertex;
} // end of namespace CSG
// - Primitive Geometries
class _PrimitiveGeometry;
class BoxGeometry;
class CylinderGeometry;
class DiscGeometry;
class GroundGeometry;
class PlaneGeometry;
class RibbonGeometry;
class SphereGeometry;
class TiledGroundGeometry;
class TorusGeometry;
class TorusKnotGeometry;
// -- Primitive Geometries aliases --
using BoxGeometryPtr         = shared_ptr_t<BoxGeometry>;
using CylinderGeometryPtr    = shared_ptr_t<CylinderGeometry>;
using DiscGeometryPtr        = shared_ptr_t<DiscGeometry>;
using GroundGeometryPtr      = shared_ptr_t<GroundGeometry>;
using PlaneGeometryPtr       = shared_ptr_t<PlaneGeometry>;
using RibbonGeometryPtr      = shared_ptr_t<RibbonGeometry>;
using SphereGeometryPtr      = shared_ptr_t<SphereGeometry>;
using TiledGroundGeometryPtr = shared_ptr_t<TiledGroundGeometry>;
using TorusGeometryPtr       = shared_ptr_t<TorusGeometry>;
using TorusKnotGeometryPtr   = shared_ptr_t<TorusKnotGeometry>;
// - Polygon Mesh
class IndexedVector2;
struct Polygon;
class PolygonMeshBuilder;
struct PolygonPoints;
// - Simplification
class DecimationTriangle;
class DecimationVertex;
struct ISimplificationSettings;
struct ISimplificationTask;
class ISimplifier;
class QuadraticErrorSimplification;
class QuadraticMatrix;
class Reference;
class SimplificationQueue;
class SimplicationQueueSceneComponent;
class SimplificationSettings;
// -- Simplification aliases
using SimplificationQueuePtr = shared_ptr_t<SimplificationQueue>;
using SimplicationQueueSceneComponentPtr
  = shared_ptr_t<SimplicationQueueSceneComponent>;
// --- Morph ---
class MorphTarget;
class MorphTargetManager;
// -- Morph aliases --
using MorphTargetManagerPtr = shared_ptr_t<MorphTargetManager>;
// --- Particles ---
struct DepthSortedParticle;
struct EmitterCreationOptions;
class GPUParticleSystem;
struct IParticleSystem;
class ModelShape;
class Particle;
struct ParticleHelper;
class ParticleSystem;
class ParticleSystemSet;
struct ParticleSystemSetEmitterCreationOptions;
class SolidParticle;
class SolidParticleSystem;
// -- Particles aliases --
using IParticleSystemPtr = shared_ptr_t<IParticleSystem>;
// - EmitterTypes
class BoxParticleEmitter;
class ConeParticleEmitter;
class CylinderParticleEmitter;
class HemisphericParticleEmitter;
struct IParticleEmitterType;
class PointParticleEmitter;
class SphereDirectedParticleEmitter;
class SphereParticleEmitter;
// --- Physics ---
struct IPhysicsBody;
struct IPhysicsEnabledObject;
struct IPhysicsEnginePlugin;
struct IWorld;
class PhysicsEngine;
class PhysicsImpostor;
struct PhysicsImpostorJoint;
struct PhysicsImpostorParameters;
struct PhysicsParams;
// - Helper
struct PhysicsForceAndContactPoint;
class PhysicsGravitationalFieldEvent;
struct PhysicsGravitationalFieldEventData;
class PhysicsHelper;
class PhysicsRadialExplosionEvent;
struct PhysicsRadialExplosionEventData;
class PhysicsUpdraftEvent;
struct PhysicsUpdraftEventData;
class PhysicsVortexEvent;
struct PhysicsVortexEventData;
// - Joint
class DistanceJoint;
struct DistanceJointData;
class Hinge2Joint;
class HingeJoint;
struct IMotorEnabledJoint;
class MotorEnabledJoint;
class PhysicsJoint;
struct PhysicsJointData;
struct SpringJointData;
// - Plugins
class OimoPhysicsEnginePlugin;
// --- Post Process ---
class AnaglyphPostProcess;
class BlackAndWhitePostProcess;
class BloomEffect;
class BloomMergePostProcess;
class BlurPostProcess;
class ChromaticAberrationPostProcess;
class CircleOfConfusionPostProcess;
class ColorCorrectionPostProcess;
class ConvolutionPostProcess;
struct DepthOfFieldBlurPostProcess;
class DepthOfFieldEffect;
class DepthOfFieldMergePostProcess;
struct DepthOfFieldMergePostProcessOptions;
class DisplayPassPostProcess;
class ExtractHighlightsPostProcess;
class FilterPostProcess;
class FxaaPostProcess;
class GlowBlurPostProcess;
class GrainPostProcess;
class HighlightsPostProcess;
class ImageProcessingPostProcess;
class PassPostProcess;
class PostProcess;
class PostProcessManager;
class RefractionPostProcess;
class SharpenPostProcess;
class StereoscopicInterlacePostProcess;
class TonemapPostProcess;
class VolumetricLightScatteringPostProcess;
class VRDistortionCorrectionPostProcess;
// - Render Pipeline
class PostProcessRenderEffect;
class PostProcessRenderPipeline;
class PostProcessRenderPipelineManager;
class PostProcessRenderPipelineManagerSceneComponent;
// - Render Pipeline aliases --
using PostProcessRenderPipelineManagerPtr
  = shared_ptr_t<PostProcessRenderPipelineManager>;
using PostProcessRenderPipelineManagerSceneComponentPtr
  = shared_ptr_t<PostProcessRenderPipelineManagerSceneComponent>;
// -- Pipelines
class DefaultRenderingPipeline;
class LensRenderingPipeline;
class SSAO2RenderingPipeline;
class SSAORenderingPipeline;
class StandardRenderingPipeline;
// --- Probes ---
class ReflectionProbe;
// --- Rendering ---
class BoundingBoxRenderer;
class DepthRenderer;
class DepthRendererSceneComponent;
class EdgesRenderer;
class FaceAdjacencies;
class GeometryBufferRenderer;
class GeometryBufferRendererSceneComponent;
struct IRenderingManagerAutoClearSetup;
class OutlineRenderer;
class LineEdgesRenderer;
class RenderingGroup;
class RenderingManager;
class UtilityLayerRenderer;
// -- Rendering aliases --
using BoundingBoxRendererPtr = shared_ptr_t<BoundingBoxRenderer>;
using DepthRendererSceneComponentPtr
  = shared_ptr_t<DepthRendererSceneComponent>;
using GeometryBufferRendererPtr = shared_ptr_t<GeometryBufferRenderer>;
using GeometryBufferRendererSceneComponentPtr
  = shared_ptr_t<GeometryBufferRendererSceneComponent>;
// --- Sprites ---
class Sprite;
class SpriteManager;
// --- States ---
class _AlphaState;
class _DepthCullingState;
class _StencilState;
} // end of namespace BABYLON
// --- Tools ---
class ColorGradient;
struct DDS;
struct DDSInfo;
class DDSTools;
class EventState;
struct FactorGradient;
struct IFileRequest;
struct IValueGradient;
class PerfCounter;
class PerformanceMonitor;
class RollingAverage;
struct SerializationHelper;
// - HDR
struct CMGBoundinBox;
struct CubeMapInfo;
class CubeMapToSphericalPolynomialTools;
struct FileFaceOrientation;
struct HDRInfo;
class HDRTools;
class PanoramaToCubeMapTools;
template <typename ArrayBufferView>
class PMREMGenerator;
// - Optimization
class HardwareScalingOptimization;
class LensFlaresOptimization;
class MergeMeshesOptimization;
class ParticlesOptimization;
class PostProcessesOptimization;
class RenderTargetsOptimization;
class SceneOptimization;
struct SceneOptimizer;
class SceneOptimizerOptions;
class ShadowsOptimization;
class TextureOptimization;
namespace picojson {
class value;
typedef ::std::vector<value> array;
typedef ::std::map<::std::string, value> object;
} // end of namespace picojson

namespace BABYLON {
namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json
} // end of namespace BABYLON

#endif // BABYLON_FWD_H
