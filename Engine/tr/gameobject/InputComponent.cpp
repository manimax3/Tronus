#include "InputComponent.h"
#include "../core/Engine.h"
#include "../graphics/GraphicsHandler.h"

void tr::InputComponent::SetupInputListening()
{
    connection = tr::Engine::Get().sGraphicsHandler->InputRecieved.connect(
        InputLayer::COMPONENT, [&](const InputEvent &e) {
            if (e.canceled)
                return;
            this->OnEvent(e);
        });
}

void tr::InputComponent::OnEvent(const InputEvent &e) { InputRecieved(e); }
