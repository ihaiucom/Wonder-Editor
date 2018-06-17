open AssetNodeType;

open AssetTreeNodeType;

module Method = {
  let _isSelected = (currentNodeId, id) =>
    switch (currentNodeId) {
    | None => false
    | Some(nodeId) => AssetUtils.isIdEqual(id, nodeId)
    };

  let showSpecificTreeNodeChildren =
      (
        (store, dispatchFunc),
        (dragImg, debounceTime, nodeMap, currentNodeId),
        assetTreeNodeChildrenArr,
      ) =>
    assetTreeNodeChildrenArr
    |> Js.Array.map(({id}: assetTreeNodeType) => {
         let {name, type_, result} =
           nodeMap |> WonderCommonlib.SparseMapService.unsafeGet(id);
         switch (type_) {
         | Folder =>
           <FolderBox
             key=(DomHelper.getRandomKey())
             store
             dispatchFunc
             attributeTuple=(
               dragImg,
               "./public/img/11.jpg",
               id,
               name,
               _isSelected(currentNodeId, id),
               AssetTreeUtils.getFlag(),
               debounceTime,
             )
             funcTuple=(
               AssetTreeUtils.onDrop(dispatchFunc),
               AssetTreeUtils.handleFlag,
               AssetUtils.isTreeNodeRelationError,
             )
           />
         | Image =>
           <FileBox
             key=(DomHelper.getRandomKey())
             store
             dispatchFunc
             attributeTuple=(
               dragImg,
               result |> OptionService.unsafeGet,
               id,
               name,
               AssetTreeUtils.getFlag(),
               _isSelected(currentNodeId, id),
             )
           />
         | Json =>
           <FileBox
             key=(DomHelper.getRandomKey())
             store
             dispatchFunc
             attributeTuple=(
               dragImg,
               "./public/img/12.jpg",
               id,
               name,
               AssetTreeUtils.getFlag(),
               _isSelected(currentNodeId, id),
             )
           />
         | _ =>
           /* WonderLog.Log.fatal(
             WonderLog.Log.buildFatalMessage(
               ~title="showSpecificTreeNodeChildren",
               ~description={j|unknown type_: $type_|j},
               ~reason="",
               ~solution={j||j},
               ~params={j||j},
             ),
           ) */
           WonderLog.Log.warn({j|the type texture not exist|j});
           ReasonReact.nullElement
         };
       });

  let buildContent = ((store, dispatchFunc), dragImg, debounceTime) => {
    let editorState = StateEditorService.getState();

    editorState
    |> AssetTreeRootEditorService.unsafeGetAssetTreeRoot
    |> AssetUtils.getSpecificTreeNodeById(
         editorState |> AssetUtils.getTargetTreeNodeId,
       )
    |> OptionService.unsafeGet
    |> (currentParentNode => currentParentNode.children)
    |> showSpecificTreeNodeChildren(
         (store, dispatchFunc),
         (
           dragImg,
           debounceTime,
           editorState |> AssetNodeMapEditorService.unsafeGetNodeMap,
           editorState |> AssetCurrentNodeIdEditorService.getCurrentNodeId,
         ),
       );
  };
};

let component = ReasonReact.statelessComponent("MainEditorAssetHeader");

let render = ((store, dispatchFunc), dragImg, debounceTime, _self) =>
  <article key="assetChildrenNode" className="wonder-asset-assetChildren">
    (
      ReasonReact.arrayToElement(
        Method.buildContent((store, dispatchFunc), dragImg, debounceTime),
      )
    )
  </article>;

let make = (~store, ~dispatchFunc, ~dragImg, ~debounceTime, _children) => {
  ...component,
  render: self => render((store, dispatchFunc), dragImg, debounceTime, self),
};