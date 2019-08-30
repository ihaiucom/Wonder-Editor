module CustomEventHandler = {
  include EmptyEventHandler.EmptyEventHandler;
  type prepareTuple = unit;
  type dataTuple = unit;
  type return = unit;

  let handleSelfLogic = ((uiState, dispatchFunc), (), ()) => {
    (
      editorState => {
        let engineState = StateEngineService.unsafeGetState();

        let (editorState, nodeId) =
          IdAssetEditorService.generateNodeId(editorState);

        let targetTreeNode =
          editorState
          |> OperateTreeAssetEditorService.unsafeGetSelectedFolderNodeInAssetTree;

        let editorState =
          JsonNodeAssetEditorService.addJsonNodeToAssetTree(
            targetTreeNode,
            JsonNodeAssetService.buildNode(
              ~nodeId,
              ~jsonStr="",
              ~name=
                JsonNodeAssetService.getNewName()
                ->(
                    OperateTreeAssetLogicService.getUniqueNodeName(
                      targetTreeNode,
                      engineState,
                    )
                  ),
            ),
            editorState,
          );

        editorState;
      }
    )
    |> StateLogicService.getAndSetEditorState;

    dispatchFunc(AppStore.UpdateAction(Update([|UpdateStore.Project|])))
    |> ignore;
  };
};

module MakeEventHandler = EventHandler.MakeEventHandler(CustomEventHandler);