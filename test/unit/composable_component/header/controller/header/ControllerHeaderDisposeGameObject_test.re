open Wonder_jest;

open Expect;

open Expect.Operators;

open Sinon;

let _ =
  describe(
    "controller header dispose gameObject",
    () => {
      let sandbox = getSandboxDefaultVal();
      let _triggerClickDispose = (component) =>
        BaseEventTool.triggerComponentEvent
          (component, OperateGameObjectEventTool.triggerClickDisposeAndExecDisposeJob);
          /* NoWorkerJobToolEngine.execDisposeJob() */
      beforeEach(
        () => {
          TestTool.closeContractCheck();
          sandbox := createSandbox()
        }
      );
      afterEach(
        () => {
          restoreSandbox(refJsObjToSandbox(sandbox^));
          TestTool.openContractCheck()
        }
      );
      describe(
        "test dispose gameObject",
        () => {
          beforeEach(
            () => {
              MainEditorSceneTool.initStateAndGl(~sandbox, ());
              MainEditorSceneTool.createDefaultScene(
                sandbox,
                MainEditorSceneTool.setFirstBoxTobeCurrentGameObject
              );
              GameObjectTool.unsafeGetCurrentGameObject()
              |> GameObjectTool.addFakeVboBufferForGameObject;
              ControllerTool.setRequest(createEmptyStubWithJsObjSandbox(sandbox));
              ControllerTool.run()
            }
          );
          describe(
            "gameObject should remove from editEngineState and runEngineState",
            () =>
              describe(
                "test dispose current gameObject",
                () =>
                  describe(
                    "current gameObject should be disposed from scene",
                    () => {
                      test(
                        "test scene children length",
                        () => {
                          let component =
                            BuildComponentTool.buildHeader(
                              SceneTreeTool.buildAppStateSceneGraphFromEngine()
                            );
                          _triggerClickDispose(component);
                          (
                            StateLogicService.getEditEngineState()
                            |> GameObjectUtils.getChildren(MainEditorSceneTool.unsafeGetScene())
                            |> Js.Array.length,
                            StateLogicService.getRunEngineState()
                            |> GameObjectUtils.getChildren(MainEditorSceneTool.unsafeGetScene())
                            |> Js.Array.length
                          )
                          |> expect == (3, 2)
                        }
                      );
                      test(
                        "test scene children shouldn't include it",
                        () => {
                          let currentGameObject = GameObjectTool.unsafeGetCurrentGameObject();
                          let component =
                            BuildComponentTool.buildHeader(
                              SceneTreeTool.buildAppStateSceneGraphFromEngine()
                            );
                          _triggerClickDispose(component);
                          (
                            StateLogicService.getEditEngineState()
                            |> GameObjectUtils.getChildren(MainEditorSceneTool.unsafeGetScene())
                            |> Js.Array.includes(
                                 DiffComponentTool.getEditEngineComponent(
                                   DiffType.GameObject,
                                   currentGameObject
                                 )
                               ),
                            StateLogicService.getRunEngineState()
                            |> GameObjectUtils.getChildren(MainEditorSceneTool.unsafeGetScene())
                            |> Js.Array.includes(currentGameObject)
                          )
                          |> expect == (false, false)
                        }
                      )
                    }
                  )
              )
          )
        }
      );
      describe(
        "test scene tree",
        () => {
          beforeEach(() => MainEditorSceneTool.initStateAndGl(~sandbox, ()));
          test(
            "if not set currentGameObject, disposed button's disabled props should == true",
            () => {
              MainEditorSceneTool.createDefaultScene(sandbox, () => ());
              BuildComponentTool.buildHeader(SceneTreeTool.buildAppStateSceneGraphFromEngine())
              |> ReactTestTool.createSnapshotAndMatch
            }
          );
          test(
            "if set currentGameObject, disposed button's disabled props should == false",
            () => {
              MainEditorSceneTool.createDefaultScene(
                sandbox,
                MainEditorSceneTool.setFirstBoxTobeCurrentGameObject
              );
              BuildComponentTool.buildHeader(SceneTreeTool.buildAppStateSceneGraphFromEngine())
              |> ReactTestTool.createSnapshotAndMatch
            }
          );
          test(
            "dispose current gameObject",
            () => {
              MainEditorSceneTool.createDefaultScene(
                sandbox,
                MainEditorSceneTool.setFirstBoxTobeCurrentGameObject
              );
              GameObjectTool.unsafeGetCurrentGameObject()
              |> GameObjectTool.addFakeVboBufferForGameObject;
              let component =
                BuildComponentTool.buildHeader(SceneTreeTool.buildAppStateSceneGraphFromEngine());
              _triggerClickDispose(component);
              BuildComponentTool.buildSceneTree(SceneTreeTool.buildAppStateSceneGraphFromEngine())
              |> ReactTestTool.createSnapshotAndMatch
            }
          )
        }
      );
      describe(
        "fix bug",
        () => {
          test(
            "dispose gameObject should re-render edit canvas and run canvas",
            () => {
              TestToolEngine.createAndSetEngineState(
                ~sandbox,
                ~noWorkerJobRecord=
                  NoWorkerJobConfigToolEngine.buildNoWorkerJobConfig(
                    ~loopPipelines={|[
                                  {"name": "default", "jobs": [
                                    {"name": "clear_color"}
                                    ]}
                                ]|},
                    ()
                  ),
                ()
              );
              TestTool.createScene();
              TestToolEngine.setFakeGl(sandbox);
              AllMaterialToolEngine.prepareForInit();
              MainEditorSceneTool.createDefaultScene(
                sandbox,
                MainEditorSceneTool.setFirstBoxTobeCurrentGameObject
              );
              let editEngineState = StateLogicService.getEditEngineState();
              let runEngineState = StateLogicService.getRunEngineState();
              let eeGl = DeviceManagerToolEngine.getGl(editEngineState) |> Obj.magic;
              let reGl = DeviceManagerToolEngine.getGl(runEngineState) |> Obj.magic;
              let component =
                BuildComponentTool.buildHeader(SceneTreeTool.buildAppStateSceneGraphFromEngine());
              _triggerClickDispose(component);
              MainEditorSceneTool.setFirstBoxTobeCurrentGameObject();
              _triggerClickDispose(component);
              (eeGl##clearColor |> getCallCount, reGl##clearColor |> getCallCount)
              |> expect == (1, 1)
            }
          );
          test(
            "can't dispose last camera",
            () => {
              MainEditorSceneTool.initStateAndGl(~sandbox, ());
              MainEditorSceneTool.createDefaultScene(
                sandbox,
                MainEditorSceneTool.setFirstBoxTobeCurrentGameObject
              );
              GameObjectTool.unsafeGetCurrentGameObject()
              |> GameObjectTool.addFakeVboBufferForGameObject;
              (
                MainEditorSceneTool.unsafeGetScene()
                |> GameObjectTool.getChildren
                |> Js.Array.filter(
                     (gameObject) =>
                       CameraEngineService.isCamera(gameObject)
                       |> StateLogicService.getEngineStateToGetData
                   )
                |> Js.Array.length,
                HeaderUtils.doesSceneHasRemoveableCamera()
              )
              |> expect == (1, false)
            }
          )
        }
      )
    }
  );