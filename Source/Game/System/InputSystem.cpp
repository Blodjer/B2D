#include "B2D_pch.h"
#include "InputSystem.h"

#include "Game/Component/InputComponent.h"

void InputSystem::Update(float deltaTime)
{
    for (ComponentSlice slice : ComponentIterator(mWorld))
    {
        /*

        level persistent data can be saved in a playerstate

        controller is controlling ui
        controller only exists in a loaded level

        PlayerController/AiController system dispatches event to its entity

        Window->GameInstance->LocalPlayer->PlayerController?

        Player
            PlayerState playerState
                name
                score

        LocalPlayer
            InputMapping inputMapping
            OnInput()

        System::Update()
            for (PlayerControllerItr)
                mWorld->GetOwningGameInstance()->GetLocalPlayer(playerControllerId)
                
        PlayerEntity::OnUpdate()
            InputComponent i = Get<InputComponent>()
            CharacterMovement cm = Get<CharacterMovement>()
            if (i.Get("Jump", EKeyEvent::Press))
                cm.jump = true;
            if (i.Get("Jump", EKeyEvent::Release))
                cm.jump = false;
            if (i.Get("Move") != 0)
                cm.addInputVector(i.Get("Move"))

-----------
        EditoEngine 
            vector<Window> windows

        GameEngine
            Window window

        Window (Transform determined by Window/EditorWindow)
            vector<Viewport> viewports [min 1]
            OnInput()
                Input::OnInput()
                GetOwningGameInstance()::OnInput(inputId)
                    GetTargetPlayer(inputId)->OnInput()
            
        Viewport (Splitscreen)
            Origin
            Size
            Camera* currentCamera

        GameInstance (ClientGame) only one per game or pie
            GameWorld world
            Tick()
                world->Tick()
            OnInput()
                InputToPlayer()
            GameWorld* world
            AddLocalPlayer()
                AddPlayerEntity()
            vector<LocalPlayer> players

        GameWorld (ECS)(EntityAdmin)(representation of the world)
            Copy(world&)
            GameInstance* owningGameInstance
            Entities
            Components
            Systems

        EditorInstance
            Tick()
                RenderWorldInViewport(world, viewport)
            GameWorld world
            PIE(world)
                new GameInstance(world)
                    world = world
           
        RenderSystem
            Update()
                mWorld.gameInstance.window.Draw(mWorld)

----------- InGame Input
        Input
            ControllerId [Keyboard&Mouse, Gamepad1, Gamepad2]
            GetInputFor(ControllerId id)

        Player
            int inputId
            Viewport* viewport

        PossesableComponent
            ControllerID possesedBy
            EntityID possesedBy
            vector<commands> commands

        ControllerComponent
            PossesableComponent* possesses

        PlayerController : ControllerComponent
            Player* player

        AiController : ControllerComponent
            BT* bt

        InputComponent : Component
            ButtonStates
            AxisStates
            bool anyButtonPressed
            float lastInput

        InputSystem : System<InputComponent>
            for (i = InputComponent)
                i.Flush()
                GETBUTTONSTATEFROMSOMEWHERE
                i.playerController

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

