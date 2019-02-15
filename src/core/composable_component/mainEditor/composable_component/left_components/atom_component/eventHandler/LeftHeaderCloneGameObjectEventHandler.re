open UpdateStore;

module CustomEventHandler = {
  include EmptyEventHandler.EmptyEventHandler;
  type prepareTuple = unit;
  type dataTuple = unit;
  type return = unit;

  let handleSelfLogic = ((uiState, dispatchFunc), (), ()) => {
    let editorState = StateEditorService.getState();
    let engineState = StateEngineService.unsafeGetState();

    let engineState =
      LeftHeaderGameObjectResultUtils.getTargetGameObject()
      |> Result.Result.either(
           targetGameObject => {
             let isNeedReInitAllLightMaterials =
               HierarchyGameObjectEngineService.getAllGameObjects(
                 targetGameObject,
                 engineState,
               )
               |> SceneEngineService.isNeedReInitAllLightMaterials(
                    _,
                    engineState,
                  );

             let (clonedGameObjectArr, engineState) =
               engineState
               |> GameObjectEngineService.cloneGameObject(
                    targetGameObject,
                    1,
                    true,
                  );

             let clonedGameObject =
               CloneGameObjectLogicService.getClonedGameObject(
                 clonedGameObjectArr,
               );

             let engineState =
               engineState
               |> HierarchyGameObjectEngineService.addChild(
                    engineState
                    |> HierarchyGameObjectEngineService.getParentGameObject(
                         targetGameObject,
                       )
                    |> OptionService.unsafeGet,
                    clonedGameObject,
                  );

             let editorState =
               editorState
               |> GameObjectComponentLogicService.setGameObjectArrComponentTypeMap(
                    [|clonedGameObject|],
                    GameObjectComponentLogicService.buildAllComponentArray(),
                    engineState,
                  )
               |> SceneTreeEditorService.setCurrentSceneTreeNode(
                    clonedGameObject,
                  );

             let engineState =
               isNeedReInitAllLightMaterials ?
                 engineState
                 |> SceneEngineService.clearShaderCacheAndReInitAllLightMaterials :
                 engineState;

             editorState |> StateEditorService.setState |> ignore;

             let engineState =
               StateLogicService.refreshEngineStateAndReturnEngineState(
                 engineState,
               );

             engineState;
           },
           errorMsg => {
             ConsoleUtils.error(errorMsg, editorState);

             engineState;
           },
         );

    engineState |> StateEngineService.setState |> ignore;

    dispatchFunc(AppStore.UpdateAction(Update([|Inspector, SceneTree|])))
    |> ignore;
  };
};

module MakeEventHandler = EventHandler.MakeEventHandler(CustomEventHandler);