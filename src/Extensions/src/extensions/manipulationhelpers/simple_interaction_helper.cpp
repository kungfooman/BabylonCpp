#include <babylon/extensions/manipulationhelpers/simple_interaction_helper.h>

#include <babylon/engine/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/extensions/manipulationhelpers/manipulator_interaction_helper.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

float SimpleInteractionHelper::CameratorSwitchThreshold = 4.f;

SimpleInteractionHelper::SimpleInteractionHelper(Scene* scene)
    : _actionStack{}, _scene{scene}, _manipulator{nullptr}
{
  _pointerObserver = _scene->onPointerObservable.add(
    [this](PointerInfo* p, EventState& s) { pointerCallback(*p, s); }, -1,
    true);
}

SimpleInteractionHelper::~SimpleInteractionHelper()
{
}

SimpleInteractionHelper::SIHCurrentAction
SimpleInteractionHelper::currentAction() const
{
  if (_actionStack.empty()) {
    return SIHCurrentAction::Selector;
  }

  return _actionStack.back();
}

ManipulatorInteractionHelper* SimpleInteractionHelper::manipulator()
{
  if (!_manipulator) {
    _manipulator = std::make_unique<ManipulatorInteractionHelper>(_scene);
  }

  return _manipulator.get();
}

void SimpleInteractionHelper::pointerCallback(const PointerInfo& p,
                                              EventState& s)
{
  detectActionChanged(p, s);

  switch (currentAction()) {
    case SIHCurrentAction::Selector:
      doSelectorInteraction(p, s);
      break;
    case SIHCurrentAction::Camerator:
      if (static_cast<int>(p.type)
          & (static_cast<int>(PointerEventTypes::POINTERUP)
             | (static_cast<int>(PointerEventTypes::POINTERWHEEL)))) {
        if (!_actionStack.empty()) {
          _actionStack.pop_back();
        }
      }
      break;
    case SIHCurrentAction::None:
      break;
  }
}

void SimpleInteractionHelper::doSelectorInteraction(const PointerInfo& p,
                                                    EventState& s)
{
  s.skipNextObservers = true;

  // We want left button up.
  if (p.type != PointerEventTypes::POINTERUP
      || p.pointerEvent.button != MouseButtonType::LEFT) {
    return;
  }

  AbstractMeshPtr selectedMesh = nullptr;

  if (p.pickInfo.hit) {
    selectedMesh = p.pickInfo.pickedMesh;
  }

  // We selected the same mesh? nothing to do
  auto pickedMesh = ::std::static_pointer_cast<AbstractMesh>(_pickedNode);
  if (pickedMesh == selectedMesh) {
#if 0
    selectedMesh->showBoundingBox = !selectedMesh->showBoundingBox;

    if (selectedMesh->showBoundingBox == false) {
      manipulator()->detachManipulatedNode(_pickedNode);
      _pickedNode = nullptr;
    }
#endif
    return;
  }

  // Detach the manipulator to the current selected mesh
  if (_pickedNode) {
    auto mesh = ::std::static_pointer_cast<AbstractMesh>(_pickedNode);
    if (mesh) {
#if 0
      mesh->showBoundingBox = false;
#endif
    }
    manipulator()->detachManipulatedNode(_pickedNode.get());
    _pickedNode = nullptr;
  }

  // Nothing selected, our job's done
  if (!selectedMesh) {
    return;
  }

  _pickedNode = selectedMesh;
#if 0
  selectedMesh->showBoundingBox = true;
#endif

  manipulator()->attachManipulatedNode(_pickedNode.get());
}

void SimpleInteractionHelper::detectActionChanged(const PointerInfo& p,
                                                  EventState& /*s*/)
{
  // Detect switch from selection to camerator
  if (currentAction() == SIHCurrentAction::Selector) {

    if (p.type == PointerEventTypes::POINTERDOWN) {
      if (!p.pickInfo.hit) {
        _actionStack.emplace_back(SIHCurrentAction::Camerator);
        return;
      }
    }

    if (p.type == PointerEventTypes::POINTERWHEEL) {
      _actionStack.emplace_back(SIHCurrentAction::Camerator);
      return;
    }
  }
}

} // end of namespace Extensions
} // end of namespace BABYLON
