#include "InputComponent.h"
#include "../core/Engine.h"
#include "../graphics/GraphicsHandler.h"

void tr::InputComponent::SetupInputListening()
{
    tr::Engine::Get().sGraphicsHandler->InputRecieved.connect(
        Signal<void(const InputEvent &)>::slot_type(
            &InputComponent::OnEvent, this, boost::placeholders::_1)
            .track_foreign(this->weak_from_this()));
}

void tr::InputComponent::OnEvent(const InputEvent &e) { InputRecieved(e); }
