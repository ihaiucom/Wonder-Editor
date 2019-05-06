open HeaderAssetBundleType;

let buildSelectTreeForGenerateSingleRAB = HeaderAssetBundle.Method.buildSelectTreeForGenerateSingleRAB;

let buildGenerateSingleRABModal =
    (
      ~selectTree,
      ~send,
      ~languageType=LanguageEditorService.unsafeGetType
                    |> StateLogicService.getEditorState,
      (),
    ) =>
  HeaderAssetBundleGenerateSingleRAB.Method.renderGenerateSingleRABModal(
    languageType,
    selectTree,
    send,
    (() => (), () => ()),
  );

let filter =
    (
      ~tree,
      ~acc,
      ~pushNodeFunc,
      ~predValueNodeFunc=node => false,
      ~predFolderNodeFunc=node => false,
      (),
    )
    : 'r => {
  let _nodeFunc = (acc, node, predNodeFunc) =>
    predNodeFunc(node) ? pushNodeFunc(node, acc) : acc;
  let _valueNodeFunc = (acc, nodeId, nodeData) =>
    _nodeFunc(
      acc,
      ValueNodeSelectTreeService.buildNodeByNodeData(~nodeId, ~nodeData),
      predValueNodeFunc,
    );
  let _folderNodeFunc = (acc, nodeId, nodeData, children) =>
    _nodeFunc(
      acc,
      FolderNodeSelectTreeService.buildNodeByNodeData(
        ~nodeId,
        ~nodeData,
        ~children,
      ),
      predFolderNodeFunc,
    );

  IterateTreeSelectTreeService.fold(
    ~acc,
    ~tree,
    ~valueNodeFunc=_valueNodeFunc,
    ~folderNodeFunc=_folderNodeFunc,
    (),
  );
};

let find =
    (
      ~tree,
      ~predValueNodeFunc=node => false,
      ~predFolderNodeFunc=node => false,
      (),
    )
    : 'r =>
  switch (
    filter(
      ~acc=[],
      ~pushNodeFunc=(node, acc) => [node, ...acc],
      ~tree,
      ~predValueNodeFunc,
      ~predFolderNodeFunc,
      (),
    )
  ) {
  | list when List.length(list) === 0 => None
  | list => Some(list)
  };

let findOne =
    (
      ~tree,
      ~predValueNodeFunc=node => false,
      ~predFolderNodeFunc=node => false,
      (),
    )
    : 'r =>
  find(~tree, ~predValueNodeFunc, ~predFolderNodeFunc, ())
  |> Js.Option.map((. list) => list |> List.hd);

let findNodeByName = (targetNodeName, tree) => {
  let predNodeFunc = node =>
    NodeSelectTreeService.getNodeName(node) === targetNodeName;

  findOne(
    ~tree,
    ~predValueNodeFunc=predNodeFunc,
    ~predFolderNodeFunc=predNodeFunc,
    (),
  );
};

let setSelectForSelectTree = (isSelect, nodeName, tree) =>
  HeaderAssetBundleGenerateSingleRAB.Method._setSelectForSelectTree(
    tree,
    isSelect,
    findNodeByName(nodeName, tree) |> OptionService.unsafeGet,
  );

let generateSingleRABResourceData = HeaderAssetBundleGenerateSingleRAB.Method._generateSingleRABResourceData;

let buildTextureData = (textureComponent, imageDataIndex): textureData => {
  textureComponent,
  imageDataIndex,
};

let buildWDBGeometryFolderName = HeaderAssetBundle.Method._buildWDBGeometryFolderName;