#ifndef BABYLON_PARTICLES_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_PARTICLE_SYSTEM_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/particles/emittertypes/iparticle_emitter_Type.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief This represents a particle system in Babylon.
 * Particles are often small sprites used to simulate hard-to-reproduce
 * phenomena like fire, smoke, water, or abstract visual effects like magic
 * glitter and faery dust. Particles can take different shapes while emitted
 * like box, sphere, cone or you can write your custom function.
 * Example: https://doc.babylonjs.com/babylon101/particles
 */
class BABYLON_SHARED_EXPORT ParticleSystem : public IAnimatable,
                                             public IParticleSystem {

public:
  /** Statics **/
  /**
   * Source color is added to the destination color without alpha affecting the
   * result.
   */
  static constexpr int BLENDMODE_ONEONE = 0;
  /**
   * Blend current color and particle color using particle’s alpha.
   */
  static constexpr int BLENDMODE_STANDARD = 1;

  /**
   * Add current color and particle color multiplied by particle’s alpha.
   */
  static constexpr int BLENDMODE_ADD = 2;

public:
  /**
   * @brief Instantiates a particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce
   * phenomena like fire, smoke, water, or abstract visual effects like magic
   * glitter and faery dust.
   * @param name The name of the particle system
   * @param capacity The max number of particles alive at the same time
   * @param scene The scene the particle system belongs to
   * @param customEffect a custom effect used to change the way particles are
   * rendered by default
   * @param isAnimationSheetEnabled Must be true if using a spritesheet to
   * animate the particles texture
   * @param epsilon Offset used to render the particles
   */
  ParticleSystem(const string_t& name, size_t capacity, Scene* scene,
                 Effect* customEffect         = nullptr,
                 bool isAnimationSheetEnabled = false, float epsilon = 0.01f);
  virtual ~ParticleSystem() override;

  virtual IReflect::Type type() const override;

  /**
   * @brief Get hosting scene.
   * @returns the scene
   */
  Scene* getScene() const;

  /**
   * @brief Gets the current list of color gradients.
   * You must use addColorGradient and removeColorGradient to udpate this list
   * @returns the list of color gradients
   */
  vector_t<ColorGradient>& getColorGradients() override;

  /**
   * @brief Gets the current list of size gradients.
   * You must use addSizeGradient and removeSizeGradient to udpate this list
   * @returns the list of size gradients
   */
  vector_t<FactorGradient>& getSizeGradients() override;

  /**
   * @brief Gets the current list of life time gradients.
   * You must use addLifeTimeGradient and removeLifeTimeGradient to udpate this
   * list
   * @returns the list of life time gradients
   */
  vector_t<FactorGradient>& getLifeTimeGradients();

  /**
   * @brief Gets the current list of angular speed gradients.
   * You must use addAngularSpeedGradient and removeAngularSpeedGradient to
   * udpate this list
   * @returns the list of angular speed gradients
   */
  vector_t<FactorGradient>& getAngularSpeedGradients() override;

  /**
   * @brief Gets the current list of velocity gradients.
   * You must use addVelocityGradient and removeVelocityGradient to udpate this
   * list
   * @returns the list of velocity gradients
   */
  vector_t<FactorGradient>& getVelocityGradients() override;

  /**
   * @brief Adds a new life time gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the life time factor to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  ParticleSystem& addLifeTimeGradient(float gradient, float factor,
                                      const nullable_t<float>& factor2
                                      = nullopt_t);

  /**
   * @brief Remove a specific life time gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  ParticleSystem& removeLifeTimeGradient(float gradient);

  /**
   * @brief Adds a new size gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  ParticleSystem& addSizeGradient(float gradient, float factor,
                                  const nullable_t<float>& factor2
                                  = nullopt_t) override;

  /**
   * @brief Remove a specific size gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeSizeGradient(float gradient) override;

  /**
   * @brief Adds a new angular speed gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addAngularSpeedGradient(float gradient, float factor,
                                           const nullable_t<float>& factor2
                                           = nullopt_t) override;

  /**
   * @brief Remove a specific angular speed gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeAngularSpeedGradient(float gradient) override;

  /**
   * @brief Adds a new velocity gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addVelocityGradient(float gradient, float factor,
                                       const nullable_t<float>& factor2
                                       = nullopt_t) override;

  /**
   * @brief Remove a specific velocity gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeVelocityGradient(float gradient) override;

  /**
   * @brief Adds a new color gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color defines the color to affect to the specified gradient
   * @param color2 defines an additional color used to define a range ([color,
   * color2]) with main color to pick the final color from
   */
  IParticleSystem& addColorGradient(float gradient, const Color4& color,
                                    const nullable_t<Color4>& color2
                                    = nullopt_t) override;

  /**
   * @brief Remove a specific color gradient.
   * @param gradient defines the gradient to remove
   */
  IParticleSystem& removeColorGradient(float gradient) override;

  /**
   * @brief Gets the current list of active particles.
   */
  vector_t<Particle*>& particles();

  /**
   * @brief Returns the string "ParticleSystem".
   * @returns a string containing the class name
   */
  const char* getClassName() const;

  /**
   * @brief Gets the maximum number of particles active at the same time.
   * @returns The max number of active particles.
   */
  size_t getCapacity() const override;

  /**
   * @brief Gets Wether there are still active particles in the system.
   * @returns True if it is alive, otherwise false.
   */
  bool isAlive() const;

  /**
   * @brief Gets Wether the system has been started.
   * @returns True if it has been started, otherwise false.
   */
  bool isStarted() const override;

  /**
   * @brief Starts the particle system and begins to emit.
   * @param delay defines the delay in milliseconds before starting the system
   * (0 by default)
   */
  void start(size_t delay = 0) override;

  /**
   * @brief Stops the particle system.
   */
  void stop() override;

  /**
   * @brief Stops the particle system.
   * @param stopSubEmitters if true it will stop the current system and all
   * created sub-Systems if false it will stop the current root system only,
   * this param is used by the root particle system only. the default value is
   * true.
   */
  void stop(bool stopSubEmitters);

  /**
   * @brief Remove all active particles
   */
  void reset() override;

  /**
   * @brief For internal use only.
   */
  void _appendParticleVertex(unsigned int index, Particle* particle,
                             int offsetX, int offsetY);

  /**
   * @brief "Recycles" one of the particle by copying it back to the "stock" of
   * particles and removing it from the active list. Its lifetime will start
   * back at 0.
   */
  void recycleParticle(Particle* particle);

  /**
   * @brief Hidden
   */
  static vector_t<string_t>
  _GetAttributeNamesOrOptions(bool isAnimationSheetEnabled = false,
                              bool isBillboardBased        = false);

  /**
   * @brief Hidden
   */
  static vector_t<string_t>
  _GetEffectCreationOptions(bool isAnimationSheetEnabled = false);

  /**
   * @brief For internal use only.
   */
  void _appendParticleVertexWithAnimation(unsigned int index,
                                          Particle* particle, int offsetX,
                                          int offsetY);

  /**
   * @brief Animates the particle system for the current frame by emitting new
   * particles and or animating the living ones.
   * @param preWarmOnly will prevent the system from updating the vertex buffer
   * (default is false)
   */
  void animate(bool preWarmOnly = false) override;

  /**
   * @brief Rebuilds the particle system.
   */
  void rebuild() override;

  /**
   * @brief Is this system ready to be used/rendered.
   * @return true if the system is ready
   */
  bool isReady() override;

  /**
   * @brief Renders the particle system in its current state.
   * @returns the current number of particles
   */
  size_t render(bool preWarm = false) override;

  /**
   * @brief Disposes the particle system and free the associated resources.
   * @param disposeTexture defines if the particule texture must be disposed as
   * well (true by default)
   */
  void dispose(bool disposeTexture             = true,
               bool disposeMaterialAndTextures = false) override;

  vector_t<AnimationPtr> getAnimations() override;

  /**
   * @brief Creates a Point Emitter for the particle system (emits directly from
   * the emitter position).
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @returns the emitter
   */
  PointParticleEmitter* createPointEmitter(const Vector3& direction1,
                                           const Vector3& direction2);

  /**
   * @brief Creates a Hemisphere Emitter for the particle system (emits along
   * the hemisphere radius).
   * @param radius The radius of the hemisphere to emit from
   * @param radiusRange The range of the hemisphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  HemisphericParticleEmitter* createHemisphericEmitter(float radius      = 1.f,
                                                       float radiusRange = 1.f);

  /**
   * @brief Creates a Sphere Emitter for the particle system (emits along the
   * sphere radius).
   * @param radius The radius of the sphere to emit from
   * @param radiusRange The range of the sphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  SphereParticleEmitter* createSphereEmitter(float radius      = 1.f,
                                             float radiusRange = 1.f);

  /**
   * @brief Creates a Directed Sphere Emitter for the particle system (emits
   * between direction1 and direction2).
   * @param radius The radius of the sphere to emit from
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the sphere
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the sphere
   * @returns the emitter
   */
  SphereDirectedParticleEmitter* createDirectedSphereEmitter(
    float radius = 1.f, const Vector3& direction1 = Vector3(0.f, 1.f, 0.f),
    const Vector3& direction2 = Vector3(0.f, 1.f, 0.f));

  /**
   * @brief Creates a Cone Emitter for the particle system (emits from the cone
   * to the particle position).
   * @param radius The radius of the cone to emit from
   * @param angle The base angle of the cone
   * @returns the emitter
   */
  ConeParticleEmitter* createConeEmitter(float radius = 1.f,
                                         float angle  = Math::PI_4);

  // this method needs to be changed when breaking changes will be allowed
  // to match the sphere and cone methods and properties direction1,2 and
  // minEmitBox,maxEmitBox to be removed from the system.
  /**
   * @brief Creates a Box Emitter for the particle system. (emits between
   * direction1 and direction2 from withing the box defined by minEmitBox
   * and maxEmitBox)
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param minEmitBox Particles are emitted from the box between minEmitBox
   * and maxEmitBox
   * @param maxEmitBox  Particles are emitted from the box between
   * minEmitBox and maxEmitBox
   * @returns the emitter
   */
  BoxParticleEmitter* createBoxEmitter(const Vector3& direction1,
                                       const Vector3& direction2,
                                       const Vector3& minEmitBox,
                                       const Vector3& maxEmitBox);

  /**
   * @brief Clones the particle system.
   * @param name The name of the cloned object
   * @param newEmitter The new emitter to use
   * @returns the cloned particle system
   */
  IParticleSystem* clone(const string_t& name, Mesh* newEmitter) override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  Json::object serialize() const override;

  /**
   * @brief Hidden
   */
  static void _Serialize(Json::object& serializationObject,
                         IParticleSystem* particleSystem);

  /**
   * @brief Hidden
   */
  static ParticleSystem* _Parse(const Json::value& parsedParticleSystem,
                                IParticleSystem* particleSystem, Scene* scene,
                                const string_t& url);

  /**
   * @brief Parses a JSON object to create a particle system.
   * @param parsedParticleSystem The JSON object to parse
   * @param scene The scene to create the particle system in
   * @param rootUrl The root url to use to load external dependencies like
   * texture
   * @returns the Parsed particle system
   */
  static ParticleSystem* Parse(const Json::value& parsedParticleSystem,
                               Scene* scene, const string_t& url);

protected:
  Vector3& get_direction1();
  void set_direction1(const Vector3& value);
  Vector3& get_direction2();
  void set_direction2(const Vector3& value);
  Vector3& get_minEmitBox();
  void set_minEmitBox(const Vector3& value);
  Vector3& get_maxEmitBox();
  void set_maxEmitBox(const Vector3& value);

  /**
   * @brief Sets a callback that will be triggered when the system is disposed.
   */
  void set_onDispose(
    const ::std::function<void(ParticleSystem*, EventState&)>& callback);

  /**
   * @brief Gets whether an animation sprite sheet is enabled or not on the
   * particle system
   */
  bool get_isAnimationSheetEnabled() const;

  /**
   * @brief Sets whether an animation sprite sheet is enabled or not on the
   * particle system
   */
  void set_isAnimationSheetEnabled(bool value);

  /**
   * @brief Gets or sets a boolean indicating if the particles must be rendered
   * as billboard or aligned with the direction.
   */
  bool get_isBillboardBased() const override;

  /**
   * @brief Sets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction.
   */
  void set_isBillboardBased(bool value) override;

private:
  float _fetchR(float u, float v, float width, float height,
                const Uint8Array& pixels);
  void _addFactorGradient(vector_t<FactorGradient>& factorGradients,
                          float gradient, float factor,
                          const nullable_t<float>& factor2 = nullopt_t);
  void _removeFactorGradient(vector_t<FactorGradient>& factorGradients,
                             float gradient);
  void _resetEffect();
  void _createVertexBuffers();
  void _createIndexBuffer();
  // Start of sub system methods
  void _stopSubEmitters();
  Particle* _createParticle();
  void _removeFromRoot();
  void _emitFromParticle(Particle* particle);
  // End of sub system methods
  void _update(int newParticles);
  Effect* _getEffect();
  void _appendParticleVertices(unsigned int offset, Particle* particle);

public:
  /**
   * If you want to launch only a few particles at once, that can be done, as
   * well.
   */
  int manualEmitCount;

  /**
   * Specifies whether the particle system will be disposed once it reaches
   * the end of the animation.
   */
  bool disposeOnStop;

  /**
   * This can help using your own shader to render the particle system.
   * The according effect will be created
   */
  string_t customShader;

  /**
   * By default particle system starts as soon as they are created. This
   * prevents the automatic start to happen and let you decide when to start
   * emitting particles.
   */
  bool preventAutoStart;

  /**
   * This function can be defined to provide custom update for active
   * particles. This function will be called instead of regular update (age,
   * position, color, etc.). Do not forget that this function will be called
   * on every frame so try to keep it simple and fast :)
   */
  ::std::function<void(vector_t<Particle*>& particles)> updateFunction;

  /**
   * Callback triggered when the particle animation is ending.
   */
  ::std::function<void()> onAnimationEnd;

  /**
   * Forces the particle to write their depth information to the depth buffer.
   * This can help preventing other draw calls to override the particles.
   */
  bool forceDepthWrite;

  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when
   * particleEmitterTyps is a BoxParticleEmitter
   */
  Property<ParticleSystem, Vector3> direction1;

  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when
   * particleEmitterTyps is a BoxParticleEmitter
   */
  Property<ParticleSystem, Vector3> direction2;

  /**
   * Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than
   * one point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter
   */
  Property<ParticleSystem, Vector3> minEmitBox;

  /**
   * Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than
   * one point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter
   */
  Property<ParticleSystem, Vector3> maxEmitBox;

  /**
   * An optional mask to filter some colors out of the texture, or filter a
   * part of the alpha channel.
   */
  Color4 textureMask;

  /**
   * This function can be defined to specify initial direction for every new
   * particle. It by default use the emitterType defined function
   */
  ::std::function<void(const Matrix& worldMatrix, Vector3& directionToUpdate,
                       Particle* particle)>
    startDirectionFunction;

  /**
   * This function can be defined to specify initial position for every new
   * particle. It by default use the emitterType defined function
   */
  ::std::function<void(const Matrix& worldMatrix, Vector3& positionToUpdate,
                       Particle* particle)>
    startPositionFunction;

  /**
   * An event triggered when the system is disposed
   */
  Observable<ParticleSystem> onDisposeObservable;

  /**
   * Sets a callback that will be triggered when the system is disposed
   */
  WriteOnlyProperty<ParticleSystem,
                    ::std::function<void(ParticleSystem*, EventState&)>>
    onDispose;

  /**
   * Gets or sets whether an animation sprite sheet is enabled or not on the
   * particle system
   */
  Property<ParticleSystem, bool> isAnimationSheetEnabled;

  unsigned int _vertexBufferSize;

  // Sub-emitters
  /**
   * this is the Sub-emitters templates that will be used to generate particle
   * system when the particle dies, this property is used by the root particle
   * system only.
   */
  vector_t<ParticleSystem*> subEmitters;
  /**
   * The current active Sub-systems, this property is used by the root
   * particle system only.
   */
  vector_t<ParticleSystem*> activeSubSystems;

private:
  Observer<ParticleSystem>::Ptr _onDisposeObserver;
  vector_t<Particle*> _particles;
  float _epsilon;
  size_t _capacity;
  Scene* _scene;
  vector_t<Particle*> _stockParticles;
  int _newPartsExcess;
  Float32Array _vertexData;
  unique_ptr_t<Buffer> _vertexBuffer;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unique_ptr_t<Buffer> _spriteBuffer;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  Effect* _customEffect;
  string_t _cachedDefines;

  Color4 _scaledColorStep;
  Color4 _colorDiff;
  Vector3 _scaledDirection;
  Vector3 _scaledGravity;
  int _currentRenderId;
  bool _alive;
  bool _useInstancing;

  bool _started;
  bool _stopped;
  int _actualFrame;
  int _scaledUpdateSpeed;
  bool _isAnimationSheetEnabled;
  bool _isBillboardBased;

  ::std::function<void(unsigned int offset, Particle* particle)>
    _appendParticleVertexes;

  ParticleSystem* _rootParticleSystem;
  Vector3 _zeroVector3;

  Matrix _emitterWorldMatrix;

  vector_t<ColorGradient> _colorGradients;
  vector_t<FactorGradient> _sizeGradients;
  vector_t<FactorGradient> _lifeTimeGradients;
  vector_t<FactorGradient> _angularSpeedGradients;
  vector_t<FactorGradient> _velocityGradients;

}; // end of class ParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_SYSTEM_H
