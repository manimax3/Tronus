#pragma once
#include <tr.h>

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
};
}
