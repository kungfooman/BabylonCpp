#include <babylon/sprites/sprite.h>

#include <babylon/materials/textures/texture.h>
#include <babylon/math/color4.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

Sprite::Sprite(const string_t& iName, SpriteManager* manager)
    : name{iName}
    , position{Vector3::Zero()}
    , color{::std::make_unique<Color4>(1.f, 1.f, 1.f, 1.f)}
    , width{1}
    , height{1}
    , angle{0}
    , cellIndex{0}
    , invertU{0}
    , invertV{0}
    , isPickable{false}
    , actionManager{nullptr}
    , isVisible{true}
    , size{this, &Sprite::get_size, &Sprite::set_size}
    , _animationStarted{false}
    , _loopAnimation{false}
    , _fromIndex{0}
    , _toIndex{0}
    , _delay{0}
    , _direction{1}
    , _manager{manager}
    , _time{0}
    , _onAnimationEnd{nullptr}
{
}

Sprite::~Sprite()
{
}

void Sprite::addToSpriteManager(unique_ptr_t<Sprite>&& newSprite)
{
  _manager->sprites.emplace_back(::std::move(newSprite));
}

int Sprite::get_size() const
{
  return width;
}

void Sprite::set_size(int value)
{
  width  = value;
  height = value;
}

void Sprite::playAnimation(int from, int to, bool loop, float delay,
                           const ::std::function<void()>& onAnimationEnd)
{
  _fromIndex        = from;
  _toIndex          = to;
  _loopAnimation    = loop;
  _delay            = delay;
  _animationStarted = true;

  _direction = from < to ? 1 : -1;

  cellIndex = from;
  _time     = 0.f;

  _onAnimationEnd = onAnimationEnd;
}

void Sprite::stopAnimation()
{
  _animationStarted = false;
}

void Sprite::_animate(float deltaTime)
{
  if (!_animationStarted)
    return;

  _time += deltaTime;
  if (_time > _delay) {
    _time = ::std::fmod(_time, _delay);
    cellIndex += _direction;
    if (cellIndex > _toIndex) {
      if (_loopAnimation) {
        cellIndex = _fromIndex;
      }
      else {
        cellIndex         = _toIndex;
        _animationStarted = false;
        if (_onAnimationEnd) {
          _onAnimationEnd();
        }
        if (disposeWhenFinishedAnimating) {
          dispose();
        }
      }
    }
  }
}

void Sprite::dispose()
{
  // Remove from scene
  _manager->sprites.erase(
    ::std::remove_if(_manager->sprites.begin(), _manager->sprites.end(),
                     [this](const unique_ptr_t<Sprite>& sprite) {
                       return sprite.get() == this;
                     }),
    _manager->sprites.end());
}

} // end of namespace BABYLON
