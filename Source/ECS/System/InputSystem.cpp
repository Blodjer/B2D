#include "B2D_pch.h"
#include "InputSystem.h"

#include "ECS/Component/InputComponent.h"
#include "ECS/Component/PawnComponent.h"

void InputSystem::Update(float deltaTime)
{
    for (InputComponent& input : ComponentItr<InputComponent, PawnComponent>(mWorld))
    {
        PawnComponent const& pawn = input.Sibling<PawnComponent>();
        /*

        level persistent data can be saved in a playerstate

        controller is controlling ui
        controller only exists in a loaded level


        PlayerController/AiController system dispatches event to its entity
            OnMove(controlinput)
                Get<CharacterMovement>()->Move(controlinput)

-----------

        Window
            OnInput()
                GetMyGameInstance()->OnInput()
            
        GameInstance
            OnInput()
                EvaluteWhereToDispatchInput
                LocalPlayer
           
        Viewport
            vector<LocalPlayer> players

        Player
            int inputId
            Viewport* viewport
            PlayerController* controller

        PossesableComponent : Component
            Controller* possesedBy
            vector<commands> c

        ControllerComponent
            PossesableComponent* possesses

        PlayerController : ControllerComponent
            Player* player

        AiController : ControllerComponent
            BT* bt

        InputComponent : Component
            

        InputSystem : System<InputComponent>
            inputstream = playercontroller.player.GetInputStream()
            inputComponent.stream = inputstream
            inputComponent.anyButtonPressed = inputstream.isEmpty()
            inputComponent.lastInput = Time.now()

        CommandSystem : System<PossesableComponent>
            cmc = GetWeakSibling<CharacterMovementComponent>()
            if (cmc.move

        AiSystem : System<AiController>
            doBT()
            aicontroller.possesses
            if (Get<Sensor>().triggered)
                Move()

        InputSingeltonComponent : SingletonComponent
            vector<Keys> keys
            vector<Axis> axis
        */
    }
}

