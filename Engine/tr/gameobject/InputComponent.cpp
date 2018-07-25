#include "InputComponent.h"
#include "../core/Engine.h"
#include "../graphics/GraphicsHandler.h"

void tr::InputComponent::SetupInputListening()
{
    connection = tr::Engine::Get().sGraphicsHandler->InputRecieved.connect(
        [&](const InputEvent &e) { this->OnEvent(e); });
}

void tr::InputComponent::OnEvent(const InputEvent &e) { InputRecieved(e); }
