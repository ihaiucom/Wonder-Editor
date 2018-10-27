open Wonder_jest;

open Expect;

open Expect.Operators;

open Sinon;

open MainEditorTransform;

open Js.Promise;

let _ =
  describe("MainEditorGeometry component", () => {
    let sandbox = getSandboxDefaultVal();
    beforeEach(() => {
      sandbox := createSandbox();

      MainEditorSceneTool.initState(~sandbox, ());

      MainEditorSceneTool.createDefaultScene(
        sandbox,
        MainEditorSceneTool.setFirstBoxToBeCurrentSceneTreeNode,
      );

      CurrentSelectSourceEditorService.setCurrentSelectSource(
        EditorType.SceneTree,
      )
      |> StateLogicService.getAndSetEditorState;
    });
    afterEach(() => restoreSandbox(refJsObjToSandbox(sandbox^)));

    describe("test set currentSceneTreeNode", () => {
      describe("test change geometry", () => {
        describe("test snapshot", () => {
          describe("test show select geometry group widget", () => {
            let _addNoTexCoordGeometry = () => {
              let engineState = StateEngineService.unsafeGetState();

              let (engineState, _, _, _, _) =
                GeometryToolEngine.createGameObjectAndSetPointData(
                  ~state=engineState,
                  ~hasTexCoords=false,
                  (),
                );

              engineState |> StateEngineService.setState |> ignore;
            };

            let _setGameObjectLightMateiralDiffuseMap = gameObject => {
              let engineState = StateEngineService.unsafeGetState();

              let (engineState, map) =
                BasicSourceTextureEngineService.create(engineState);
              let engineState =
                GameObjectComponentEngineService.unsafeGetLightMaterialComponent(
                  gameObject,
                  engineState,
                )
                |> LightMaterialEngineService.setLightMaterialDiffuseMap(
                     _,
                     map,
                     engineState,
                   );

              StateEngineService.setState(engineState);
            };

            test(
              "if current geometry has no texCoord, select geometry group should contain geometry which has texCoord or no texCoord",
              () => {
                _addNoTexCoordGeometry();
                let currentGameObjectGeometry =
                  GameObjectTool.getCurrentGameObjectGeometry();

                let component =
                  BuildComponentTool.buildGeometry(
                    ~geometryComponent=currentGameObjectGeometry,
                    ~isShowGeometryGroup=true,
                    (),
                  );

                component |> ReactTestTool.createSnapshotAndMatch;
              },
            );
            test(
              "else, select geometry group should only contain geometry which has texCoord",
              () => {
              _addNoTexCoordGeometry();
              _setGameObjectLightMateiralDiffuseMap(
                GameObjectTool.unsafeGetCurrentSceneTreeNode(),
              );
              let currentGameObjectGeometry =
                GameObjectTool.getCurrentGameObjectGeometry();

              let component =
                BuildComponentTool.buildGeometry(
                  ~geometryComponent=currentGameObjectGeometry,
                  ~isShowGeometryGroup=true,
                  (),
                );

              component |> ReactTestTool.createSnapshotAndMatch;
            });
          });

          test("test hide select geometry group widget", () => {
            let currentGameObjectGeometry =
              GameObjectTool.getCurrentGameObjectGeometry();

            let component =
              BuildComponentTool.buildGeometry(
                ~geometryComponent=currentGameObjectGeometry,
                ~isShowGeometryGroup=false,
                (),
              );

            component |> ReactTestTool.createSnapshotAndMatch;
          });
        });

        describe("test logic", () => {
          test("test the current gameObject geometry should be Cube", () => {
            let currentGameObjectGeometry =
              GameObjectTool.getCurrentGameObjectGeometry();

            GeometryEngineService.unsafeGetGeometryName(
              currentGameObjectGeometry,
            )
            |> StateLogicService.getEngineStateToGetData
            |> expect == MainEditorGeometryTool.getCubeGeometryName();
          });
          test(
            "change geometry to be Sphere, the current gameObject geometry should be Sphere",
            () => {
              MainEditorGeometryTool.changeGeometry(
                ~sourceGeometry=GameObjectTool.getCurrentGameObjectGeometry(),
                ~targetGeometry=
                  MainEditorGeometryTool.getDefaultSphereGeometryComponent(),
                (),
              );

              let newGameObjectGeometry =
                GameObjectTool.getCurrentGameObjectGeometry();

              GeometryEngineService.unsafeGetGeometryName(
                newGameObjectGeometry,
              )
              |> StateLogicService.getEngineStateToGetData
              |> expect == MainEditorGeometryTool.getSphereGeometryName();
            },
          );
          test(
            "test add Cube geometry component again and again, currentSceneTreeNode's geometry should be Cube",
            () => {
              let component =
                BuildComponentTool.buildGeometry(
                  ~geometryComponent=
                    GameObjectTool.getCurrentGameObjectGeometry(),
                  (),
                );

              MainEditorGeometryTool.changeGeometry(
                ~sourceGeometry=GameObjectTool.getCurrentGameObjectGeometry(),
                ~targetGeometry=
                  MainEditorGeometryTool.getDefaultSphereGeometryComponent(),
                (),
              );
              MainEditorGeometryTool.changeGeometry(
                ~sourceGeometry=GameObjectTool.getCurrentGameObjectGeometry(),
                ~targetGeometry=
                  MainEditorGeometryTool.getDefaultCubeGeometryComponent(),
                (),
              );
              MainEditorGeometryTool.changeGeometry(
                ~sourceGeometry=GameObjectTool.getCurrentGameObjectGeometry(),
                ~targetGeometry=
                  MainEditorGeometryTool.getDefaultSphereGeometryComponent(),
                (),
              );
              MainEditorGeometryTool.changeGeometry(
                ~sourceGeometry=GameObjectTool.getCurrentGameObjectGeometry(),
                ~targetGeometry=
                  MainEditorGeometryTool.getDefaultCubeGeometryComponent(),
                (),
              );

              let newGameObjectGeometry =
                GameObjectTool.getCurrentGameObjectGeometry();

              StateEngineService.unsafeGetState()
              |> GeometryEngineService.unsafeGetGeometryName(
                   newGameObjectGeometry,
                 )
              |> expect == "Cube";
            },
          );
        });
      });

      describe("test load asset wdb", () => {
        beforeEach(() => {
          MainEditorAssetTool.buildFakeFileReader();
          MainEditorAssetTool.buildFakeImage();

          LoadTool.buildFakeTextDecoder(
            LoadTool.convertUint8ArrayToBuffer,
          );
          LoadTool.buildFakeURL(sandbox^);

          LoadTool.buildFakeLoadImage(.);
        });

        testPromise(
          "test select geometry group widget should show all geometry", () => {
          MainEditorAssetTreeTool.BuildAssetTree.buildEmptyAssetTree()
          |> ignore;
          let fileName = "BoxTextured";
          let newWDBArrayBuffer = NodeToolEngine.getWDBArrayBuffer(fileName);

          MainEditorAssetUploadTool.loadOneWDB(
            ~fileName,
            ~arrayBuffer=newWDBArrayBuffer,
            (),
          )
          |> then_(uploadedWDBNodeId => {
               let component =
                 BuildComponentTool.buildGeometry(
                   ~geometryComponent=
                     GameObjectTool.getCurrentGameObjectGeometry(),
                   ~isShowGeometryGroup=true,
                   (),
                 );

               component |> ReactTestTool.createSnapshotAndMatch |> resolve;
             });
        });
        testPromise("test set new geometry should set into engineState", () => {
          MainEditorAssetTreeTool.BuildAssetTree.buildEmptyAssetTree()
          |> ignore;
          let fileName = "BoxTextured";
          let newWDBArrayBuffer = NodeToolEngine.getWDBArrayBuffer(fileName);
          let newGeometry = GeometryToolEngine.getNewGeometry();

          MainEditorAssetUploadTool.loadOneWDB(
            ~fileName,
            ~arrayBuffer=newWDBArrayBuffer,
            (),
          )
          |> then_(uploadedWDBNodeId => {
               let oldGameObjectGeometry =
                 GameObjectTool.getCurrentGameObjectGeometry();

               MainEditorGeometryTool.changeGeometry(
                 ~sourceGeometry=oldGameObjectGeometry,
                 ~targetGeometry=newGeometry,
                 (),
               );

               let newGameObjectGeometry =
                 GameObjectTool.getCurrentGameObjectGeometry();

               GeometryEngineService.unsafeGetGeometryName(
                 newGameObjectGeometry,
               )
               |> StateLogicService.getEngineStateToGetData
               |> expect == MainEditorGeometryTool.getBoxTexturedGeometryName()
               |> resolve;
             });
        });
      });
    });
  });