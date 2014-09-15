/*********************************
 * TestcompyComponent.h
 * Troy
 * Created 2014/09/15
 *********************************/

#include "Common.h"
#include "Testcompy.h"
#include "Engine/TransformComponent.h"

// ----------------------------------------------------------------------------

TestcompyComponentFactory TestcompyComponent::factory;

// ----------------------------------------------------------------------------

TestcompyComponent::TestcompyComponent()
{
  
}

// ----------------------------------------------------------------------------

void TestcompyComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);
  Owner->AddEvent(this, Events::EventId{"update"}, &TestcompyComponent::OnUpdate);

  _transform = Owner->GetComponent<TransformComponent>("TransformComponent");
}

// ----------------------------------------------------------------------------

void TestcompyComponent::OnUpdate(Events::EventMessage& e)
{
  _transform->Rotation.y += (float)e.GetData<Events::UpdateEvent>()->gameTime.Dt;
}

// ----------------------------------------------------------------------------

mrb_value TestcompyComponent::GetRubyWrapper()
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

TestcompyComponentFactory::TestcompyComponentFactory()
  : allocator(sizeof(TestcompyComponent))
{
  
}

// ----------------------------------------------------------------------------

Component *TestcompyComponentFactory::CreateObject(void *memory, component_factory_data&)
{
  return new (memory) TestcompyComponent;
}

// ----------------------------------------------------------------------------
