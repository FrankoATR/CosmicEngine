#ifndef PUZZLERPG_EDITOR_SCENE_HPP
#define PUZZLERPG_EDITOR_SCENE_HPP

#include <CosmicEngine/models/scene/scene.hpp>
#include <CosmicEngine/models/ui/derived/ui_button.hpp>
#include <CosmicEngine/models/ui/derived/ui_image.hpp>
#include <CosmicEngine/models/ui/derived/ui_text.hpp>
#include <CosmicEngine/models/ui/derived/ui_text_field.hpp>
#include <CosmicEngine/models/ui/derived/ui_text_area.hpp>

#include "../systems/grid.hpp"
#include "../systems/level_data.hpp"

#include <string>
#include <vector>

namespace PuzzleRPG
{
    enum class EditorSelectionKind
    {
        None,
        Tile,
        Object
    };

    struct EditorCellSelection
    {
        EditorSelectionKind kind = EditorSelectionKind::None;
        Cell cell{0, 0};
        int placementLayer = 0;
        TileLayer tile;
        int objectId = -1;
    };

    // Herramientas que ofrece el editor. Cada herramienta define cuyo
    // comportamiento se aplica al hacer click izquierdo en una celda.
    enum class EditorTool
    {
        MoveCamera,
        TileFloor,
        TileWall,
        PlaceWall2x2,
        PlaceWall3x3,
        PlaceSpawn,
        PlacePickup,
        PlacePlate,
        PlaceButton,
        PlacePushable,
        PlaceDoor,
        PlaceTeleporter,
        PlaceTorch,
        PlaceFirefly,
        PlaceSign,
        PlaceReceptacle,
        PlaceDecor,
        EraseObject,
        DuplicatePlacedObject,
        MovePlacedObject,
        WireActivatorToDoor,
        EditDialog,
        EditPairId,
        EditRequiredItem,
        ToggleWinActivator,
        PlaceBlock
    };

    // Editor visual minimalista que cubre los requisitos pedidos: pintar
    // tiles, colocar/borrar objetos, conectar activadores con puertas y
    // asignar dialogos. Guarda en assets/levels/<nombre>.json.
    class EditorScene : public CosmicEngine::Scene
    {
    public:
        EditorScene(std::string levelName, bool createNew);
        ~EditorScene() override;

        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;
        void reset() override;

    private:
        std::string levelName;
        bool        createNew;
        LevelData   level;
        EditorTool  currentTool;
        int         activatorIdForWire;   // -1 si no hay
        int         selectedObjectId;     // -1 si no hay
        Cell        lastPaintedCell;
        Cell        lastErasedCell;
        bool        leftWasDown;
        bool        rightWasDown;
        bool        ignorePointerUntilRelease;
        bool        cameraDragActive;
        bool        backgroundControlsVisible;
        float       zoom;
        glm::vec2   lastCameraDragMouseUi;
        // Sub-celda elegida del spritesheet de la herramienta activa.
        int         pickedSheet;
        int         pickedRow;
        int         pickedCol;
        int         pickerRows;
        int         pickerCols;
        int         activePlacementLayer;
        int         activeDrawLayer;
        bool        dialogModalOpen;
        int         dialogTargetObjectId;
        EditorCellSelection moveSelection;

        // UI lateral.
        CosmicEngine::UIText                  *title;
        CosmicEngine::UIText                  *statusLabel;
        CosmicEngine::UIText                  *toolLabel;
        CosmicEngine::UIImage                 *editorPanelImage;
        CosmicEngine::UIText                  *dialogHintLabel;
        CosmicEngine::UIImage                 *dialogModalBackdrop;
        CosmicEngine::UIImage                 *dialogModalPanel;
        CosmicEngine::UIText                  *dialogModalTitle;
        CosmicEngine::UIText                  *dialogModalHint;
        CosmicEngine::UITextArea              *dialogField;
        CosmicEngine::UITextField             *numericField;
        CosmicEngine::UIText                  *numericLabel;
        CosmicEngine::UIText                  *winListLabel;
        CosmicEngine::UIButton                *dialogConfirmButton;
        CosmicEngine::UIButton                *dialogCancelButton;
        CosmicEngine::UIButton                *saveButton;
        CosmicEngine::UIButton                *backButton;
        CosmicEngine::UITextField             *widthField;
        CosmicEngine::UITextField             *heightField;
        CosmicEngine::UIText                  *layerLabel;
        CosmicEngine::UITextField             *placementLayerField;
        CosmicEngine::UITextField             *drawLayerField;
        CosmicEngine::UIText                  *backgroundLabel;
        CosmicEngine::UITextField             *backgroundRField;
        CosmicEngine::UITextField             *backgroundGField;
        CosmicEngine::UITextField             *backgroundBField;
        CosmicEngine::UIButton                *resizeButton;
        CosmicEngine::UIButton                *backgroundToggleButton;
        CosmicEngine::UIButton                *applyBackgroundButton;
        CosmicEngine::UIButton                *debugToggleButton;
        CosmicEngine::UIText                  *dimensionsLabel;
        CosmicEngine::UIText                  *pickerLabel;
        CosmicEngine::UIButton                *toolPagePrevButton;
        CosmicEngine::UIButton                *toolPageNextButton;
        int                                    currentToolPage;
        CosmicEngine::UIButton                *freeGridButton;
        bool                                   freeGridMode;
        std::vector<CosmicEngine::UIButton *>  toolButtons;
        std::vector<CosmicEngine::UIElement *>  sheetPickerButtons;

        void BuildUi();
        void SetTool(EditorTool tool, const std::string &label);
        void RebuildSheetPickerForTool(EditorTool tool);
        void DrawSheetPickerPreviews() const;
        bool MouseToCell(Cell &outCell) const;
        bool IsMouseOverUi() const;
        bool HasFocusedEditorField() const;
        void ApplyToolAtCell(Cell cell);
        void HandleEraseAt(Cell cell);
        void HandlePickAt(Cell cell);
        void HandleDuplicateObjectAt(Cell cell);
        void HandleMoveObjectAt(Cell cell);
        bool CanPlaceObjectAt(const LevelObject &object, Cell cell) const;
        const TileLayer *FindTileLayerAt(Cell cell, int placementLayer) const;
        void LoadTileIntoPlacementTool(const TileLayer &tile);
        void ClearMoveSelection();
        void DrawGrid() const;
        void DrawObjects() const;
        void DrawPlacementPreview() const;
        void DrawWorldBounds() const;
        void SetStatus(const std::string &text);
        void EnsureUniqueSpawn(Cell cell);
        LevelObject *MakeObject(int classId, int textureId, Cell cell);
        LevelObject *MakeWallBlock(Cell topLeft, int classId);
        LevelObject *FindObjectAt(Cell cell, bool includeWallCoverage = false);
        int GetPlacementLayerInput() const;
        int GetDrawLayerInput(int classId) const;
        void SyncLayerInputsFromFields();
        void ApplyBackgroundColorFromFields(bool reportStatus);
        bool IsCellBlockedByObject(Cell cell, int placementLayer) const;
        void LoadObjectIntoPlacementTool(const LevelObject &object);
        void UpdateWinListLabel();
        void UpdateDimensionsLabel();
        void ApplyResizeFromFields();
        void UpdateContextualUiVisibility();
        void ValidateWinConditions();
        void SetToolPage(int page);
        void OpenDialogModal(LevelObject &object);
        void CloseDialogModal();
        void ConfirmDialogModal();
    };
}

#endif
