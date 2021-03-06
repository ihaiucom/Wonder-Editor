open NodeAssetType;

open TreeAssetType;

let getNodeName = (~node, ~getTextureNameFunc, ~getMaterialNameFunc) =>
  switch (node) {
  | ScriptEventFunctionNode(_, scriptEventFunctionNodeData) =>
    ScriptEventFunctionNodeAssetService.getNodeNameByData(
      scriptEventFunctionNodeData,
    )
  | ScriptAttributeNode(_, scriptAttributeNodeData) =>
    ScriptAttributeNodeAssetService.getNodeNameByData(scriptAttributeNodeData)
  | TextureNode(_, nodeData) =>
    TextureNodeAssetService.getNodeName(nodeData, getTextureNameFunc)
  | MaterialNode(_, nodeData) =>
    MaterialNodeAssetService.getNodeName(nodeData, getMaterialNameFunc)
  | WDBNode(_, nodeData) => WDBNodeAssetService.getNodeName(nodeData)
  | AssetBundleNode(_, nodeData) =>
    AssetBundleNodeAssetService.getNodeName(nodeData)
  | FolderNode(_, nodeData, _) =>
    FolderNodeAssetService.getNodeName(nodeData)
  };

let isNodeEqualByName =
    (~sourceNode, ~targetNode, ~getTextureNameFunc, ~getMaterialNameFunc) =>
  NodeAssetService.isNodeEqual(
    (
      NodeAssetService.isEqual,
      getNodeName(~getTextureNameFunc, ~getMaterialNameFunc),
    ),
    sourceNode,
    targetNode,
  );

let isTargetNameNode =
    (~node, ~name, ~getTextureNameFunc, ~getMaterialNameFunc) =>
  getNodeName(~node, ~getTextureNameFunc, ~getMaterialNameFunc) == name;