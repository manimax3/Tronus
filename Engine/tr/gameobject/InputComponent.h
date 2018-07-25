#pragma once
#include <tr.h>

#include "../event/CommonEvents.h"
#include "../event/Signal.h"
#include "Component.h"

#include <memory>

namespace tr {
/**
 * (Special) component to manage input.
 * For further information see GameObjects InputComponent attribut.
 */
class InputComponent final : public GameObjectComponent,
                             std::enable_shared_from_this<InputComponent> {
public:
    /**
     * Connects to the required input signals
     */
    void SetupInputListening();

    /**
     * Signal to listen for events.
     */
    Signal<void(const InputEvent &)> InputRecieved;

private:
    /**
     * Handles a recieved input event.
     * Fires the the InputRecieved Signal.
     */
    void OnEvent(const InputEvent &e);

    /**
     * We use this to disconnect our slot
     */
    sig::scoped_connection connection;
};
}
