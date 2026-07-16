#include "editor_scene.hpp"

#include "main_menu_scene.hpp"
#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"
#include "../utilities/paths.hpp"
#include "../utilities/puzzle_menu_music.hpp"
#include "../utilities/puzzle_input_actions.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include GAMEMANAGE_HEADER
#include RESOURCEMANAGER_HEADER
#include SCENEMANAGER_HEADER
#include UIMANAGER_HEADER
#include <CosmicEngine/managers/camera/camera_manager.hpp>
#include <CosmicEngine/managers/input/input_manager.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <utility>

namespace PuzzleRPG
{
    namespace
    {
        // Tiles ahora son de 16 px, asi que subimos el zoom por defecto x4
        // para que el editor se vea aproximadamente igual de grande que antes.
        constexpr float kEditorDefaultZoom = 4.0f;
        constexpr float kEditorZoomMin     = 2.0f;
        constexpr float kEditorZoomMax     = 14.0f;
        constexpr float kEditorZoomStep    = 0.6f;
        const glm::vec2 kEditorPanelPosition(1450.0f, 70.0f);
        const glm::vec2 kEditorPanelSize(430.0f, 980.0f);

        float ClampEditorZoom(float value)
        {
            return std::max(kEditorZoomMin, std::min(kEditorZoomMax, value));
        }

        bool UsesSpritePicker(EditorTool tool)
        {
            switch (tool)
            {
                case EditorTool::TileFloor:
                case EditorTool::TileWall:
                case EditorTool::PlaceWall2x2:
                case EditorTool::PlaceWall3x3:
                case EditorTool::PlacePickup:
                case EditorTool::PlacePushable:
                case EditorTool::PlaceDecor:
                    return true;
                default:
                    return false;
            }
        }

        bool IsPlacementTool(EditorTool tool)
        {
            switch (tool)
            {
                case EditorTool::TileFloor:
                case EditorTool::TileWall:
                case EditorTool::PlaceWall2x2:
                case EditorTool::PlaceWall3x3:
                case EditorTool::PlaceSpawn:
                case EditorTool::PlacePickup:
                case EditorTool::PlacePlate:
                case EditorTool::PlaceButton:
                case EditorTool::PlacePushable:
                case EditorTool::PlaceDoor:
                case EditorTool::PlaceTeleporter:
                case EditorTool::PlaceTorch:
                case EditorTool::PlaceFirefly:
                case EditorTool::PlaceSign:
                case EditorTool::PlaceReceptacle:
                case EditorTool::PlaceDecor:
                case EditorTool::PlaceBlock:
                    return true;
                default:
                    return false;
            }
        }

        int ClassIdForPlacementTool(EditorTool tool)
        {
            switch (tool)
            {
                case EditorTool::PlaceWall2x2:   return Class_Wall2x2;
                case EditorTool::PlaceWall3x3:   return Class_Wall3x3;
                case EditorTool::PlacePlate:
                case EditorTool::PlaceReceptacle: return Class_PressurePlate;
                case EditorTool::PlaceButton:     return Class_Button;
                case EditorTool::PlacePushable:   return Class_Pushable;
                case EditorTool::PlaceDoor:       return Class_Door;
                case EditorTool::PlaceTeleporter: return Class_Teleporter;
                case EditorTool::PlaceTorch:      return Class_Torch;
                case EditorTool::PlaceFirefly:    return Class_Firefly;
                case EditorTool::PlaceSign:       return Class_Sign;
                case EditorTool::PlacePickup:     return Class_Pickup;
                case EditorTool::PlaceSpawn:      return Class_PlayerSpawn;
                case EditorTool::PlaceDecor:      return Class_Decor;
                case EditorTool::PlaceBlock:      return Class_Block;
                default: return Class_None;
            }
        }

        std::string DigitsOnly(const std::string &text)
        {
            std::string result;
            result.reserve(text.size());
            for (char ch : text)
            {
                if (ch >= '0' && ch <= '9') result.push_back(ch);
            }
            return result;
        }

        void SanitizeUnsignedField(CosmicEngine::UITextField *field, int maxValue, bool allowEmpty = true)
        {
            if (!field) return;
            std::string filtered = DigitsOnly(field->GetText());
            if (filtered.empty())
            {
                if (!allowEmpty) field->SetText("0");
                return;
            }

            int value = 0;
            try { value = std::stoi(filtered); }
            catch (...) { value = 0; }
            value = std::clamp(value, 0, maxValue);
            std::string normalized = std::to_string(value);
            if (field->GetText() != normalized) field->SetText(normalized);
        }

        void DrawTorchMarkerPrimitive(glm::vec2 position, glm::vec2 size, float alpha)
        {
            const glm::vec2 stemMin = position + glm::vec2(size.x * 0.43f, size.y * 0.35f);
            const glm::vec2 stemMax = position + glm::vec2(size.x * 0.57f, size.y * 0.96f);
            RS_MN.RenderRectangle(stemMin, stemMax, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.33f, 0.20f, 0.08f), alpha, 1.0f, true);

            const glm::vec2 flameCenter = position + glm::vec2(size.x * 0.5f, size.y * 0.28f);
            const glm::vec2 outerHalf(size.x * 0.25f, size.y * 0.31f);
            const glm::vec2 innerHalf(size.x * 0.16f, size.y * 0.19f);
            RS_MN.RenderRectangle(flameCenter - outerHalf, flameCenter + outerHalf,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.96f, 0.56f, 0.12f), alpha * 0.85f, 1.0f, true);
            RS_MN.RenderRectangle(flameCenter - innerHalf, flameCenter + innerHalf,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(1.0f, 0.88f, 0.42f), alpha, 1.0f, true);
        }

        void DrawFireflyMarkerPrimitive(glm::vec2 position, glm::vec2 size, float alpha)
        {
            const glm::vec2 center = position + glm::vec2(size.x * 0.5f, size.y * 0.45f);
            const glm::vec2 glowHalf(size.x * 0.05f, size.y * 0.05f);
            const glm::vec2 coreHalf(size.x * 0.025f, size.y * 0.025f);
            RS_MN.RenderRectangle(center - glowHalf, center + glowHalf,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.72f, 0.96f, 0.36f), alpha * 0.45f, 1.0f, true);
            RS_MN.RenderRectangle(center - coreHalf, center + coreHalf,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.94f, 1.0f, 0.58f), alpha, 1.0f, true);
        }

        void DrawSignMarkerPrimitive(glm::vec2 position, glm::vec2 size, float alpha)
        {
            const glm::vec2 postMin = position + glm::vec2(size.x * 0.44f, size.y * 0.40f);
            const glm::vec2 postMax = position + glm::vec2(size.x * 0.56f, size.y * 0.95f);
            RS_MN.RenderRectangle(postMin, postMax, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.39f, 0.24f, 0.12f), alpha, 1.0f, true);

            const glm::vec2 boardMin = position + glm::vec2(size.x * 0.20f, size.y * 0.12f);
            const glm::vec2 boardMax = position + glm::vec2(size.x * 0.80f, size.y * 0.48f);
            RS_MN.RenderRectangle(boardMin, boardMax, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.92f, 0.84f, 0.62f), alpha, 1.0f, true);
            RS_MN.RenderRectangle(boardMin, boardMax, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.55f, 0.32f, 0.12f), alpha, 1.0f, false);
        }

        class SpritePickerButton : public CosmicEngine::UIElement
        {
        public:
            SpritePickerButton(int sheet,
                               int row,
                               int col,
                               int *selectedRow,
                               int *selectedCol,
                               glm::vec2 position,
                               glm::vec2 size,
                               std::function<void()> onClick)
                : UIElement(position, size, true, nullptr, CosmicEngine::UIElementType::Button),
                  sheet(sheet),
                  row(row),
                  col(col),
                  selectedRow(selectedRow),
                  selectedCol(selectedCol),
                  onClick(std::move(onClick)),
                  clickOnArea(false)
            {
            }

            void update(float deltaTime) override
            {
                bool mouseDown = INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, CosmicEngine::KeyDown);
                bool mouseUp = INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, CosmicEngine::KeyUp);
                bool hovering = MouseHover();

                if (mouseDown && hovering)
                {
                    clickOnArea = true;
                }

                if (mouseUp)
                {
                    if (hovering && clickOnArea && onClick)
                    {
                        onClick();
                    }
                    clickOnArea = false;
                }
                else if (!hovering)
                {
                    clickOnArea = false;
                }

                UIElement::update(deltaTime);
            }

            void draw() override
            {
                if (!IsVisible()) return;

                glm::vec2 spritePos = GetPosition() + glm::vec2(1.0f, 1.0f);
                glm::vec2 spriteSize = GetSize() - glm::vec2(2.0f, 2.0f);

                bool drewSprite = DrawSheetCell(SheetKeyForSheetId(sheet),
                                                row,
                                                col,
                                                spritePos,
                                                spriteSize,
                                                0.0f,
                                                glm::vec3(1.0f),
                                                1.0f,
                                                CosmicEngine::ViewType::UI);
                if (!drewSprite)
                {
                    glm::vec3 fallback = IsWallSheet(sheet)
                        ? glm::vec3(0.32f, 0.22f, 0.18f)
                        : glm::vec3(0.55f, 0.62f, 0.58f);
                    RS_MN.RenderRectangle(spritePos,
                                          spritePos + spriteSize,
                                          glm::vec2(0.0f),
                                          glm::vec2(0.0f),
                                          fallback,
                                          1.0f,
                                          1.0f,
                                          true,
                                          CosmicEngine::ViewType::UI);
                }

                const bool selected = selectedRow && selectedCol && *selectedRow == row && *selectedCol == col;
                const bool hovered = MouseHover() || IsFocused();
                glm::vec3 borderColor = selected
                    ? glm::vec3(1.0f, 0.85f, 0.20f)
                    : hovered
                        ? glm::vec3(0.95f, 0.95f, 0.95f)
                        : glm::vec3(0.18f, 0.16f, 0.12f);
                float borderThickness = selected ? 2.0f : 1.0f;

                RS_MN.RenderRectangle(GetPosition(),
                                      GetPosition() + GetSize(),
                                      glm::vec2(0.0f),
                                      glm::vec2(0.0f),
                                      borderColor,
                                      1.0f,
                                      borderThickness,
                                      false,
                                      CosmicEngine::ViewType::UI);

                UIElement::draw();
            }

            bool IsFocusable() const override
            {
                return true;
            }

            void Activate() override
            {
                if (onClick) onClick();
            }

        private:
            int sheet;
            int row;
            int col;
            int *selectedRow;
            int *selectedCol;
            std::function<void()> onClick;
            bool clickOnArea;
        };
    }

    EditorScene::EditorScene(std::string levelName, bool createNew)
        : Scene("EditorScene"),
          levelName(std::move(levelName)),
          createNew(createNew),
          currentTool(EditorTool::TileFloor),
          activatorIdForWire(-1),
          selectedObjectId(-1),
          lastPaintedCell{-1, -1},
                    lastErasedCell{-1, -1},
          leftWasDown(false),
          rightWasDown(false),
                    ignorePointerUntilRelease(true),
                    cameraDragActive(false),
                    backgroundControlsVisible(false),
                    zoom(kEditorDefaultZoom),
                    lastCameraDragMouseUi(0.0f),
          pickedSheet(SheetId::kGround),
          pickedRow(0),
          pickedCol(0),
          pickerRows(0),
          pickerCols(0),
          activePlacementLayer(0),
          activeDrawLayer(0),
          dialogModalOpen(false),
          dialogTargetObjectId(-1),
          moveSelection(),
          title(nullptr), statusLabel(nullptr), toolLabel(nullptr),
          editorPanelImage(nullptr),
          dialogHintLabel(nullptr),
          dialogModalBackdrop(nullptr),
          dialogModalPanel(nullptr),
          dialogModalTitle(nullptr),
          dialogModalHint(nullptr),
          dialogField(nullptr), numericField(nullptr), numericLabel(nullptr),
          dialogConfirmButton(nullptr), dialogCancelButton(nullptr),
          winListLabel(nullptr), saveButton(nullptr), backButton(nullptr),
          layerLabel(nullptr), placementLayerField(nullptr), drawLayerField(nullptr),
          backgroundLabel(nullptr), backgroundRField(nullptr), backgroundGField(nullptr), backgroundBField(nullptr),
          widthField(nullptr), heightField(nullptr), resizeButton(nullptr),
          backgroundToggleButton(nullptr),
          applyBackgroundButton(nullptr),
          debugToggleButton(nullptr),
          dimensionsLabel(nullptr),
          pickerLabel(nullptr),
          toolPagePrevButton(nullptr),
          toolPageNextButton(nullptr),
          currentToolPage(0),
          freeGridButton(nullptr),
          freeGridMode(false)
    {
    }

        EditorScene::~EditorScene()
        {
          GM_MN.setMouseScrollCallback({});
        }

    void EditorScene::loadResources()
    {
        RS_MN.LoadFont(PUZZLERPG_FONT_MAIN_KEY, PUZZLERPG_FONT_MAIN_PATH, PUZZLERPG_FONT_MAIN_SIZE);
        // PuzzleRPG UI textures
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_KEY, PUZZLERPG_UI_BUTTON_IDLE_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_KEY, PUZZLERPG_UI_BUTTON_HOVER_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_M_KEY, PUZZLERPG_UI_BUTTON_IDLE_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_M_KEY, PUZZLERPG_UI_BUTTON_HOVER_M_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_IDLE_L_KEY, PUZZLERPG_UI_BUTTON_IDLE_L_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_BUTTON_HOVER_L_KEY, PUZZLERPG_UI_BUTTON_HOVER_L_PATH);
        RS_MN.LoadTexture(PUZZLERPG_UI_CELL_KEY, PUZZLERPG_UI_CELL_PATH);
        ResetPuzzleSpriteAssetCache();
        RegisterPuzzleSpriteAssets();
    }

    void EditorScene::init()
    {
        RegisterPuzzleInputActions();
        StopMenuMusic();

        if (createNew || !LoadLevelFromFile(levelName, level))
        {
            level = MakeEmptyLevel(levelName, 16, 16);
        }

        SetPuzzleDebugLabelsEnabled(true);

        BuildUi();
        ValidateWinConditions();

        // Encuadrar la camara aproximadamente al centro del nivel.
        glm::vec2 center(level.width * kTileSize * 0.5f, level.height * kTileSize * 0.5f);
        CAM_MN.SetFocusPosition(center);
        CAM_MN.SetZoom(zoom);

        GM_MN.setMouseScrollCallback([this](double, double yoffset)
        {
            zoom = ClampEditorZoom(zoom + static_cast<float>(yoffset) * kEditorZoomStep);
            CAM_MN.SetZoom(zoom);
        });
    }

    void EditorScene::BuildUi()
    {
        auto chooseIdleTexture = [](const std::string &text)
        {
            const size_t length = text.size();
            if (length <= 4) return std::string(PUZZLERPG_UI_BUTTON_IDLE_KEY);
            if (length <= 14) return std::string(PUZZLERPG_UI_BUTTON_IDLE_M_KEY);
            return std::string(PUZZLERPG_UI_BUTTON_IDLE_L_KEY);
        };

        auto hoverTextureFor = [](std::string idle)
        {
            const std::size_t pos = idle.find("idle");
            if (pos != std::string::npos) idle.replace(pos, 4, "hover");
            return idle;
        };

        auto createButton = [&](const std::string &label, glm::vec2 position, glm::vec2 size)
        {
            std::string idleKey = chooseIdleTexture(label);
            auto *button = new CosmicEngine::UIButton(label, "puzzle_font", idleKey,
                                                      position, size, true, true);
            button->SetHoverTexture(hoverTextureFor(idleKey));
            button->SetTextureScale(1.04f);
            button->SetTextOffset(glm::vec2(0.0f, -4.0f));
            button->SetTextColor(glm::vec3(1.0f, 1.0f, 1.0f));
            button->SetSelectedTextColor(glm::vec3(1.0f, 0.9f, 0.0f));
            UI_MN.AddElement(button);
            return button;
        };

        title = new CosmicEngine::UIText("Editor: " + levelName, "puzzle_font",
                                         glm::vec2(40.0f, 30.0f),
                                         glm::vec2(900.0f, 50.0f), true);
        title->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
        UI_MN.AddElement(title);

        editorPanelImage = new CosmicEngine::UIImage(PUZZLERPG_UI_CELL_KEY,
                                                     kEditorPanelPosition,
                                                     kEditorPanelSize,
                                                     true);
        editorPanelImage->SetTint(glm::vec3(0.22f, 0.19f, 0.14f));
        editorPanelImage->SetAlpha(0.95f);
        UI_MN.AddElement(editorPanelImage);

        toolLabel = new CosmicEngine::UIText("Herramienta: Suelo", "puzzle_font",
                                             glm::vec2(375.0f, 994.0f),
                                             glm::vec2(700.0f, 30.0f), true);
        toolLabel->SetTextColor(glm::vec3(0.85f, 0.95f, 0.95f));
        UI_MN.AddElement(toolLabel);

        statusLabel = new CosmicEngine::UIText("Click izq: aplicar. Click der: borrar.", "puzzle_font",
                                               glm::vec2(375.0f, 1030.0f),
                                               glm::vec2(700.0f, 28.0f), true);
        statusLabel->SetTextColor(glm::vec3(0.95f, 0.95f, 0.95f));
        UI_MN.AddElement(statusLabel);

        // Page navigation buttons (above tool list).
        const glm::vec2 pageNavOrigin = kEditorPanelPosition + glm::vec2(16.0f, 50.0f);
        const glm::vec2 pageNavSize(195.0f, 28.0f);
        toolPagePrevButton = createButton("< Pagina 1", pageNavOrigin, pageNavSize);
        toolPageNextButton = createButton("Pagina 2 >", pageNavOrigin + glm::vec2(203.0f, 0.0f), pageNavSize);
        toolPagePrevButton->SetOnClick([this]() { SetToolPage(0); });
        toolPageNextButton->SetOnClick([this]() { SetToolPage(1); });
        toolPagePrevButton->SetFocused(true);

        struct ToolDef { const char *label; EditorTool tool; const char *desc; };
        const ToolDef defs[] = {
            // ---- Pagina 1 (indices 0-15) ----
            { "Mover camara",   EditorTool::MoveCamera,          "Mover camara"      },
            { "Suelo",          EditorTool::TileFloor,            "Suelo"              },
            { "Muro 1x1",       EditorTool::TileWall,             "Muro 1x1"           },
            { "Muro 2x2",       EditorTool::PlaceWall2x2,         "Muro 2x2"           },
            { "Muro 3x3",       EditorTool::PlaceWall3x3,         "Muro 3x3"           },
            { "Spawn",          EditorTool::PlaceSpawn,           "Spawn"              },
            { "Pickup",         EditorTool::PlacePickup,          "Pickup"             },
            { "Placa",          EditorTool::PlacePlate,           "Placa"              },
            { "Receptaculo",    EditorTool::PlaceReceptacle,      "Receptaculo item"   },
            { "Boton",          EditorTool::PlaceButton,          "Boton"              },
            { "Empujable",      EditorTool::PlacePushable,        "Empujable"          },
            { "Puerta",         EditorTool::PlaceDoor,            "Puerta"             },
            { "Teletransporte", EditorTool::PlaceTeleporter,      "Teletransporte"     },
            { "Antorcha",       EditorTool::PlaceTorch,           "Antorcha"           },
            { "Luciernaga",     EditorTool::PlaceFirefly,         "Luciernaga"         },
            { "Letrero",        EditorTool::PlaceSign,            "Letrero"            },
            // ---- Pagina 2 (indices 16-24) ----
            { "Adorno",         EditorTool::PlaceDecor,           "Adorno (visual)"    },
            { "Borrar",         EditorTool::EraseObject,          "Borrar objeto"      },
            { "Duplicar",       EditorTool::DuplicatePlacedObject,"Duplicar objeto"    },
            { "Mover obj.",     EditorTool::MovePlacedObject,     "Mover objeto"       },
            { "Conectar",       EditorTool::WireActivatorToDoor,  "Conectar -> Puerta" },
            { "Dialogo",        EditorTool::EditDialog,           "Asignar dialogo"    },
            { "PairId",         EditorTool::EditPairId,           "Asignar pairId (Tele)" },
            { "Item req.",      EditorTool::EditRequiredItem,     "Item requerido (Receptaculo)" },
            { "Meta",           EditorTool::ToggleWinActivator,   "Toggle activador de victoria" },
            { "Bloque col.",    EditorTool::PlaceBlock,           "Bloque de colision (1x1)"     }
        };

        // 2 columns × 8 rows per page. Page 1: defs[0..15], Page 2: defs[16..24].
        const glm::vec2 panelOrigin = kEditorPanelPosition + glm::vec2(16.0f, 82.0f);
        const glm::vec2 buttonSize(195.0f, 34.0f);
        const float colGap = 8.0f;
        const float rowGap = 4.0f;
        const int rowsPerCol = 8;
        const int pageSize   = 16;
        for (std::size_t i = 0; i < sizeof(defs) / sizeof(defs[0]); ++i)
        {
            const auto &def = defs[i];
            int page       = (static_cast<int>(i) < pageSize) ? 0 : 1;
            int withinPage = (page == 0) ? static_cast<int>(i) : static_cast<int>(i) - pageSize;
            int col        = withinPage / rowsPerCol;
            int row        = withinPage % rowsPerCol;
            glm::vec2 pos  = panelOrigin + glm::vec2(col * (buttonSize.x + colGap),
                                                     row * (buttonSize.y + rowGap));
            auto *button = createButton(def.label, pos, buttonSize);
            button->SetVisible(page == 0);
            EditorTool tool = def.tool;
            std::string desc = def.desc;
            button->SetOnClick([this, tool, desc]() { SetTool(tool, desc); });
            toolButtons.push_back(button);
        }

        pickerLabel = new CosmicEngine::UIText("Sprite picker: -", "puzzle_font",
                                               kEditorPanelPosition + glm::vec2(18.0f, 384.0f),
                                               glm::vec2(390.0f, 24.0f), true);
        pickerLabel->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
        UI_MN.AddElement(pickerLabel);

        layerLabel = new CosmicEngine::UIText("Capas: grid / draw", "puzzle_font",
                                              kEditorPanelPosition + glm::vec2(18.0f, 646.0f),
                                              glm::vec2(220.0f, 24.0f), true);
        layerLabel->SetTextColor(glm::vec3(0.85f, 0.95f, 0.95f));
        UI_MN.AddElement(layerLabel);

        placementLayerField = new CosmicEngine::UITextField("0", "Grid", "puzzle_font",
                                                             kEditorPanelPosition + glm::vec2(18.0f, 674.0f),
                                                             glm::vec2(92.0f, 40.0f), 4, true);
        placementLayerField->SetOnTextChanged([this](const std::string &)
        {
            SanitizeUnsignedField(placementLayerField, 9999);
            SyncLayerInputsFromFields();
        });
        UI_MN.AddElement(placementLayerField);

        drawLayerField = new CosmicEngine::UITextField("0", "Draw", "puzzle_font",
                                                        kEditorPanelPosition + glm::vec2(122.0f, 674.0f),
                                                        glm::vec2(92.0f, 40.0f), 4, true);
        drawLayerField->SetOnTextChanged([this](const std::string &)
        {
            SanitizeUnsignedField(drawLayerField, 9999);
            SyncLayerInputsFromFields();
        });
        UI_MN.AddElement(drawLayerField);

        backgroundToggleButton = createButton("Color fondo", kEditorPanelPosition + glm::vec2(236.0f, 674.0f), glm::vec2(172.0f, 40.0f));
        backgroundToggleButton->SetOnClick([this]()
        {
            backgroundControlsVisible = !backgroundControlsVisible;
            UpdateContextualUiVisibility();
            SetStatus(backgroundControlsVisible ? "Edicion de fondo abierta." : "Edicion de fondo cerrada.");
        });

        numericLabel = new CosmicEngine::UIText("Valor:", "puzzle_font",
                                                kEditorPanelPosition + glm::vec2(18.0f, 724.0f),
                                                glm::vec2(180.0f, 24.0f), true);
        numericLabel->SetTextColor(glm::vec3(0.85f, 0.95f, 0.95f));
        UI_MN.AddElement(numericLabel);

        numericField = new CosmicEngine::UITextField("", "Entero", "puzzle_font",
                                                     kEditorPanelPosition + glm::vec2(18.0f, 750.0f),
                                                     glm::vec2(196.0f, 42.0f), 16, true);
        UI_MN.AddElement(numericField);

        backgroundLabel = new CosmicEngine::UIText("Fondo RGB:", "puzzle_font",
                                                   kEditorPanelPosition + glm::vec2(236.0f, 724.0f),
                                                   glm::vec2(172.0f, 24.0f), true);
        backgroundLabel->SetTextColor(glm::vec3(0.85f, 0.95f, 0.95f));
        UI_MN.AddElement(backgroundLabel);

        backgroundRField = new CosmicEngine::UITextField(std::to_string(level.backgroundColor.r), "R", "puzzle_font",
                                                         kEditorPanelPosition + glm::vec2(236.0f, 750.0f),
                                                         glm::vec2(52.0f, 40.0f), 3, true);
        backgroundRField->SetOnTextChanged([this](const std::string &)
        {
            SanitizeUnsignedField(backgroundRField, 255);
        });
        UI_MN.AddElement(backgroundRField);

        backgroundGField = new CosmicEngine::UITextField(std::to_string(level.backgroundColor.g), "G", "puzzle_font",
                                                         kEditorPanelPosition + glm::vec2(294.0f, 750.0f),
                                                         glm::vec2(52.0f, 40.0f), 3, true);
        backgroundGField->SetOnTextChanged([this](const std::string &)
        {
            SanitizeUnsignedField(backgroundGField, 255);
        });
        UI_MN.AddElement(backgroundGField);

        backgroundBField = new CosmicEngine::UITextField(std::to_string(level.backgroundColor.b), "B", "puzzle_font",
                                                         kEditorPanelPosition + glm::vec2(352.0f, 750.0f),
                                                         glm::vec2(52.0f, 40.0f), 3, true);
        backgroundBField->SetOnTextChanged([this](const std::string &)
        {
            SanitizeUnsignedField(backgroundBField, 255);
        });
        UI_MN.AddElement(backgroundBField);

        applyBackgroundButton = createButton("Aplicar fondo", kEditorPanelPosition + glm::vec2(236.0f, 804.0f), glm::vec2(172.0f, 42.0f));
        applyBackgroundButton->SetOnClick([this]() { ApplyBackgroundColorFromFields(true); });

        dialogHintLabel = new CosmicEngine::UIText("Click en un letrero para abrir el editor de dialogo.", "puzzle_font",
                               kEditorPanelPosition + glm::vec2(18.0f, 804.0f),
                               glm::vec2(390.0f, 42.0f), true);
        dialogHintLabel->SetTextColor(glm::vec3(0.90f, 0.93f, 0.78f));
        UI_MN.AddElement(dialogHintLabel);

        winListLabel = new CosmicEngine::UIText("Meta: -", "puzzle_font",
                                                kEditorPanelPosition + glm::vec2(18.0f, 860.0f),
                                                glm::vec2(390.0f, 24.0f), true);
        winListLabel->SetTextColor(glm::vec3(0.95f, 0.85f, 0.30f));
        UI_MN.AddElement(winListLabel);
        UpdateWinListLabel();

        saveButton = createButton("Guardar", kEditorPanelPosition + glm::vec2(18.0f, 886.0f), glm::vec2(190.0f, 42.0f));
        saveButton->SetOnClick([this]()
        {
            ApplyBackgroundColorFromFields(false);
            if (SaveLevelToFile(level))
            {
                SetStatus("Guardado: " + LevelFilePath(level.name));
            }
            else
            {
                SetStatus("ERROR al guardar nivel.");
            }
        });

        backButton = createButton("Volver", kEditorPanelPosition + glm::vec2(218.0f, 886.0f), glm::vec2(190.0f, 42.0f));
        backButton->SetOnClick([]()
        {
            GM_MN.setMouseScrollCallback({});
            SCN_MN.ReplaceScene(new MainMenuScene());
        });

        dimensionsLabel = new CosmicEngine::UIText("Tam:", "puzzle_font",
                                                   kEditorPanelPosition + glm::vec2(18.0f, 966.0f),
                                                   glm::vec2(120.0f, 24.0f), true);
        dimensionsLabel->SetTextColor(glm::vec3(0.85f, 0.95f, 0.95f));
        UI_MN.AddElement(dimensionsLabel);

        widthField = new CosmicEngine::UITextField(std::to_string(level.width), "W", "puzzle_font",
                                                   kEditorPanelPosition + glm::vec2(150.0f, 956.0f),
                                                   glm::vec2(56.0f, 40.0f), 4, true);
        UI_MN.AddElement(widthField);

        heightField = new CosmicEngine::UITextField(std::to_string(level.height), "H", "puzzle_font",
                                                    kEditorPanelPosition + glm::vec2(214.0f, 956.0f),
                                                    glm::vec2(56.0f, 40.0f), 4, true);
        UI_MN.AddElement(heightField);

        resizeButton = createButton("Aplicar", kEditorPanelPosition + glm::vec2(278.0f, 956.0f), glm::vec2(130.0f, 40.0f));
        resizeButton->SetOnClick([this]() { ApplyResizeFromFields(); });

        debugToggleButton = createButton("Debug", glm::vec2(10.0f, 1028.0f), glm::vec2(160.0f, 40.0f));
        debugToggleButton->SetOnClick([this]()
        {
            TogglePuzzleDebugVisuals();
            SetStatus(std::string("Debug visuales: ") + (IsPuzzleDebugVisualsEnabled() ? "ON" : "OFF"));
        });

        freeGridButton = createButton("Grid libre", glm::vec2(178.0f, 1028.0f), glm::vec2(200.0f, 40.0f));
        freeGridButton->SetOnClick([this]()
        {
            freeGridMode = !freeGridMode;
            freeGridButton->SetFocused(freeGridMode);
            SetStatus(freeGridMode ? "Grid libre: suelos, muros y adornos siguen al mouse."
                                   : "Grid sticky: preview alineado a la celda.");
        });

        const glm::vec2 windowSize = GM_MN.getWindowSize();
        const glm::vec2 modalBackdropPosition(0.0f, 0.0f);
        const glm::vec2 modalBackdropSize(windowSize.x, windowSize.y);
        const glm::vec2 modalPanelPosition(windowSize.x * 0.12f, windowSize.y * 0.16f);
        const glm::vec2 modalPanelSize(windowSize.x * 0.76f, windowSize.y * 0.46f);

        dialogModalBackdrop = new CosmicEngine::UIImage(PUZZLERPG_UI_CELL_KEY,
                                                        modalBackdropPosition,
                                                        modalBackdropSize,
                                                        false);
        dialogModalBackdrop->SetTint(glm::vec3(0.02f, 0.02f, 0.03f));
        dialogModalBackdrop->SetAlpha(0.78f);
        UI_MN.AddElement(dialogModalBackdrop);

        dialogModalPanel = new CosmicEngine::UIImage(PUZZLERPG_UI_CELL_KEY,
                                                     modalPanelPosition,
                                                     modalPanelSize,
                                                     false);
        dialogModalPanel->SetTint(glm::vec3(0.20f, 0.17f, 0.12f));
        dialogModalPanel->SetAlpha(0.98f);
        UI_MN.AddElement(dialogModalPanel);

        dialogModalTitle = new CosmicEngine::UIText("Editar dialogo del letrero", "puzzle_font",
                                                     modalPanelPosition + glm::vec2(28.0f, 26.0f),
                                                     glm::vec2(modalPanelSize.x - 56.0f, 34.0f), false);
        dialogModalTitle->SetTextColor(glm::vec3(0.98f, 0.88f, 0.42f));
        UI_MN.AddElement(dialogModalTitle);

        dialogModalHint = new CosmicEngine::UIText("Escribe el texto y confirma para asignarlo al letrero. Cancelar cierra sin aplicar cambios.",
                                                    "puzzle_font",
                                                    modalPanelPosition + glm::vec2(28.0f, 66.0f),
                                                    glm::vec2(modalPanelSize.x - 56.0f, 30.0f), false);
        dialogModalHint->SetTextColor(glm::vec3(0.86f, 0.92f, 0.94f));
        UI_MN.AddElement(dialogModalHint);

        dialogField = new CosmicEngine::UITextArea("", "puzzle_font",
                                                   modalPanelPosition + glm::vec2(28.0f, 118.0f),
                                                   glm::vec2(modalPanelSize.x - 56.0f, 200.0f), 1024, false);
        dialogField->SetBackgroundColor(glm::vec3(0.11f, 0.10f, 0.08f));
        dialogField->SetBorderColor(glm::vec3(0.44f, 0.36f, 0.24f));
        dialogField->SetFocusBorderColor(glm::vec3(0.92f, 0.74f, 0.24f));
        dialogField->SetTextColor(glm::vec3(0.96f, 0.96f, 0.96f));
        dialogField->SetFocusTextColor(glm::vec3(1.0f, 1.0f, 1.0f));
        UI_MN.AddElement(dialogField);

        dialogConfirmButton = createButton("Confirmar", modalPanelPosition + glm::vec2(modalPanelSize.x - 304.0f, modalPanelSize.y - 72.0f), glm::vec2(136.0f, 44.0f));
        dialogConfirmButton->SetOnClick([this]() { ConfirmDialogModal(); });
        dialogConfirmButton->SetVisible(false);

        dialogCancelButton = createButton("Cancelar", modalPanelPosition + glm::vec2(modalPanelSize.x - 152.0f, modalPanelSize.y - 72.0f), glm::vec2(136.0f, 44.0f));
        dialogCancelButton->SetOnClick([this]() { CloseDialogModal(); });
        dialogCancelButton->SetVisible(false);

        SyncLayerInputsFromFields();
        UpdateDimensionsLabel();
        RebuildSheetPickerForTool(currentTool);
        UpdateContextualUiVisibility();
    }

    void EditorScene::RebuildSheetPickerForTool(EditorTool tool)
    {
        int sheet = 0, rows = 0, cols = 0;
        switch (tool)
        {
            case EditorTool::TileFloor:
                sheet = SheetId::kGround;
                rows  = PUZZLERPG_SHEET_TILE_FLOOR_ROWS;
                cols  = PUZZLERPG_SHEET_TILE_FLOOR_COLS;
                break;
            case EditorTool::TileWall:
                sheet = SheetId::kWalls1x1;
                rows  = PUZZLERPG_SHEET_TILE_WALL_ROWS;
                cols  = PUZZLERPG_SHEET_TILE_WALL_COLS;
                break;
            case EditorTool::PlaceWall2x2:
                sheet = SheetId::kWalls2x2;
                rows  = PUZZLERPG_SHEET_TILE_WALL_2X2_ROWS;
                cols  = PUZZLERPG_SHEET_TILE_WALL_2X2_COLS;
                break;
            case EditorTool::PlaceWall3x3:
                sheet = SheetId::kWalls3x3;
                rows  = PUZZLERPG_SHEET_TILE_WALL_3X3_ROWS;
                cols  = PUZZLERPG_SHEET_TILE_WALL_3X3_COLS;
                break;
            case EditorTool::PlacePickup:
                sheet = SheetId::kPickable;
                rows  = PUZZLERPG_SHEET_PICKUP_ROWS;
                cols  = PUZZLERPG_SHEET_PICKUP_COLS;
                break;
            case EditorTool::PlacePushable:
                sheet = SheetId::kPushable;
                rows  = PUZZLERPG_SHEET_PUSHABLE_ROWS;
                cols  = PUZZLERPG_SHEET_PUSHABLE_COLS;
                break;
            case EditorTool::PlaceDecor:
                sheet = SheetId::kDecor;
                rows  = PUZZLERPG_SHEET_DECOR_ROWS;
                cols  = PUZZLERPG_SHEET_DECOR_COLS;
                break;
            default:
                break;
        }

        // Libera botones previos.
        for (auto *b : sheetPickerButtons)
        {
            UI_MN.RemoveElement(b);
            delete b;
        }
        sheetPickerButtons.clear();

        pickedSheet = sheet;
        pickedRow   = 0;
        pickedCol   = 0;
        pickerRows  = rows;
        pickerCols  = cols;

        if (pickerLabel)
        {
            if (sheet == 0)
                pickerLabel->SetText("Sprite picker: -");
            else
                pickerLabel->SetText("Sprite picker: " + std::to_string(rows) + "x" + std::to_string(cols)
                                    + " sel(0,0)");
        }

        if (sheet == 0 || rows <= 0 || cols <= 0) return;

        const float kBtn = 14.0f;
        const float kGap = 1.0f;
        const glm::vec2 origin = kEditorPanelPosition + glm::vec2(18.0f, 410.0f);
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                glm::vec2 pos = origin + glm::vec2(c * (kBtn + kGap), r * (kBtn + kGap));
                int rr = r, cc = c;
                auto *btn = new SpritePickerButton(sheet,
                                                   rr,
                                                   cc,
                                                   &pickedRow,
                                                   &pickedCol,
                                                   pos,
                                                   glm::vec2(kBtn, kBtn),
                                                   [this, rr, cc]()
                                                   {
                                                       pickedRow = rr;
                                                       pickedCol = cc;
                                                       if (pickerLabel)
                                                       {
                                                           pickerLabel->SetText("Sprite picker: " + std::to_string(pickerRows) + "x" + std::to_string(pickerCols)
                                                                              + " sel(" + std::to_string(rr) + "," + std::to_string(cc) + ")");
                                                       }
                                                       SetStatus("Sprite (" + std::to_string(rr) + "," + std::to_string(cc) + ") seleccionado.");
                                                   });
                UI_MN.AddElement(btn);
                sheetPickerButtons.push_back(btn);
            }
        }
    }

    void EditorScene::SetTool(EditorTool tool, const std::string &label)
    {
        if (dialogModalOpen && tool != EditorTool::EditDialog)
        {
            CloseDialogModal();
        }

        currentTool = tool;
        activatorIdForWire = -1;
        selectedObjectId   = -1;
        ClearMoveSelection();
        cameraDragActive = false;
        for (auto *button : toolButtons)
        {
            if (button) button->SetFocused(false);
        }
        if (dialogField) dialogField->SetFocused(false);
        CosmicEngine::UITextField *fields[] = {
            numericField,
            placementLayerField,
            drawLayerField,
            backgroundRField,
            backgroundGField,
            backgroundBField,
            widthField,
            heightField
        };
        for (auto *field : fields)
        {
            if (field) field->SetFocused(false);
        }
        if (toolLabel) toolLabel->SetText("Herramienta: " + label);
        SetStatus("Herramienta seleccionada: " + label);
        RebuildSheetPickerForTool(tool);
        UpdateContextualUiVisibility();
    }

    void EditorScene::SetStatus(const std::string &text)
    {
        if (statusLabel) statusLabel->SetText(text);
    }

    bool EditorScene::MouseToCell(Cell &outCell) const
    {
        glm::vec2 mouse = INP_MN.GetMousePosition();
        int cx = static_cast<int>(mouse.x / kTileSize);
        int cy = static_cast<int>(mouse.y / kTileSize);
        if (cx < 0 || cy < 0 || cx >= level.width || cy >= level.height) return false;
        outCell = Cell{cx, cy};
        return true;
    }

    bool EditorScene::IsMouseOverUi() const
    {
        glm::vec2 m = INP_MN.GetMousePosition_UI();
        auto inRect = [&](CosmicEngine::UIElement *e) {
            if (!e || !e->IsVisible()) return false;
            glm::vec2 p = e->GetPosition();
            glm::vec2 s = e->GetSize();
            return (m.x >= p.x && m.x <= p.x + s.x && m.y >= p.y && m.y <= p.y + s.y);
        };
        if (dialogModalOpen && dialogModalBackdrop && dialogModalBackdrop->IsVisible()) return true;
        if (inRect(editorPanelImage))     return true;
        for (auto *b : toolButtons) if (inRect(b)) return true;
        for (auto *b : sheetPickerButtons) if (inRect(b)) return true;
        if (inRect(dialogConfirmButton)) return true;
        if (inRect(dialogCancelButton)) return true;
        if (inRect(dialogModalPanel)) return true;
        if (inRect(saveButton))        return true;
        if (inRect(backButton))        return true;
        if (inRect(resizeButton))      return true;
        if (inRect(backgroundToggleButton)) return true;
        if (inRect(debugToggleButton)) return true;
        if (inRect(dialogField))       return true;
        if (inRect(placementLayerField)) return true;
        if (inRect(drawLayerField))      return true;
        if (inRect(numericField))      return true;
        if (inRect(backgroundRField))  return true;
        if (inRect(backgroundGField))  return true;
        if (inRect(backgroundBField))  return true;
        if (inRect(widthField))        return true;
        if (inRect(heightField))       return true;
        if (inRect(applyBackgroundButton)) return true;
        return false;
    }

    bool EditorScene::HasFocusedEditorField() const
    {
        if (dialogField && dialogField->IsFocused()) return true;
        const CosmicEngine::UITextField *fields[] = {
            numericField,
            placementLayerField,
            drawLayerField,
            backgroundRField,
            backgroundGField,
            backgroundBField,
            widthField,
            heightField
        };
        for (const auto *field : fields)
        {
            if (field && field->IsFocused()) return true;
        }
        return false;
    }

    void EditorScene::update(double)
    {
        SyncLayerInputsFromFields();

        if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
        {
            if (dialogModalOpen)
            {
                CloseDialogModal();
                SetStatus("Edicion de dialogo cancelada.");
                return;
            }

            GM_MN.setMouseScrollCallback({});
            SCN_MN.ReplaceScene(new MainMenuScene());
            return;
        }

        bool leftDown  = INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT,  CosmicEngine::KeyRelease);
        bool rightDown = INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT, CosmicEngine::KeyRelease);
        glm::vec2 mouseUi = INP_MN.GetMousePosition_UI();

        if (dialogModalOpen)
        {
            cameraDragActive = false;
            lastPaintedCell = Cell{-1, -1};
            lastErasedCell = Cell{-1, -1};
            leftWasDown = leftDown;
            rightWasDown = rightDown;
            return;
        }

        if (ignorePointerUntilRelease)
        {
            if (leftDown || rightDown)
            {
                leftWasDown = leftDown;
                rightWasDown = rightDown;
                return;
            }

            ignorePointerUntilRelease = false;
        }

        bool overUi    = IsMouseOverUi();
        bool fieldFocused = HasFocusedEditorField();

        Cell cell{0, 0};
        bool overGrid = !overUi && MouseToCell(cell);

        if (currentTool == EditorTool::MoveCamera)
        {
            if (overGrid && leftDown)
            {
                if (!cameraDragActive)
                {
                    cameraDragActive = true;
                    lastCameraDragMouseUi = mouseUi;
                }
                else
                {
                    glm::vec2 delta = mouseUi - lastCameraDragMouseUi;
                    if (delta.x != 0.0f || delta.y != 0.0f)
                    {
                        CAM_MN.MoveFocus(delta / std::max(zoom, 1.0f));
                        lastCameraDragMouseUi = mouseUi;
                    }
                }
            }
            else
            {
                cameraDragActive = false;
            }
        }
        else
        {
            cameraDragActive = false;
        }

        // Scene::update() corre antes que UIManager::update(), asi que si un
        // text field sigue enfocado este frame, el click del mapa todavia puede
        // usar el valor anterior. Consumimos ese primer click y dejamos que la
        // UI aplique el texto/focus antes de permitir la accion sobre el grid.
        if (fieldFocused && overGrid && (leftDown || rightDown))
        {
            if (!leftWasDown && !rightWasDown)
            {
                SetStatus("Campo activo: confirma el valor y vuelve a hacer click en el mapa.");
            }
            overGrid = false;
        }

        if (currentTool == EditorTool::MoveCamera)
        {
            lastPaintedCell = Cell{-1, -1};
            lastErasedCell = Cell{-1, -1};
            leftWasDown  = leftDown;
            rightWasDown = rightDown;
            return;
        }

        // Click izquierdo: aplicar herramienta. Para tiles y borrar permite drag.
        if (overGrid && leftDown)
        {
            bool isTileTool = (currentTool == EditorTool::TileFloor
                            || currentTool == EditorTool::TileWall);
            bool isDragTool = isTileTool || currentTool == EditorTool::EraseObject;
            if (!leftWasDown || (isDragTool && !(cell == lastPaintedCell)))
            {
                ApplyToolAtCell(cell);
                lastPaintedCell = cell;
            }
        }
        else
        {
            lastPaintedCell = Cell{-1, -1};
        }

        // Click derecho: borrar objeto / tile, tambien al arrastrar.
        if (overGrid && rightDown)
        {
            if (!rightWasDown || !(cell == lastErasedCell))
            {
                HandleEraseAt(cell);
                lastErasedCell = cell;
            }
        }
        else
        {
            lastErasedCell = Cell{-1, -1};
        }

        leftWasDown  = leftDown;
        rightWasDown = rightDown;
    }

    void EditorScene::ApplyToolAtCell(Cell cell)
    {
        switch (currentTool)
        {
            case EditorTool::TileFloor:
            {
                TileLayer layer;
                layer.sheet = SheetId::kGround;
                layer.row   = pickedRow;
                layer.col   = pickedCol;
                layer.placementLayer = GetPlacementLayerInput();
                layer.drawLayer      = std::max(0, activeDrawLayer);
                level.SetTileLayer(cell.x, cell.y, layer);
                break;
            }
            case EditorTool::TileWall:
            {
                TileLayer layer;
                layer.sheet = SheetId::kWalls1x1;
                layer.row   = pickedRow;
                layer.col   = pickedCol;
                layer.placementLayer = GetPlacementLayerInput();
                layer.drawLayer      = std::max(0, activeDrawLayer);
                level.SetTileLayer(cell.x, cell.y, layer);
                break;
            }
            case EditorTool::PlaceWall2x2:
            case EditorTool::PlaceWall3x3:
            {
                int wallClassId = (currentTool == EditorTool::PlaceWall2x2) ? Class_Wall2x2 : Class_Wall3x3;
                if (freeGridMode)
                {
                    int span = WallObjectSpan(wallClassId);
                    glm::vec2 mouseWorld = INP_MN.GetMousePosition();
                    float halfSpan = span * kTileSize * 0.5f;
                    LevelObject obj;
                    obj.id          = level.NextObjectId();
                    obj.classId     = wallClassId;
                    obj.textureId   = DefaultTextureId::kWall;
                    obj.x           = cell.x;
                    obj.y           = cell.y;
                    obj.pixelX      = mouseWorld.x - halfSpan;
                    obj.pixelY      = mouseWorld.y - halfSpan;
                    obj.spriteSheet = (wallClassId == Class_Wall2x2) ? SheetId::kWalls2x2 : SheetId::kWalls3x3;
                    obj.spriteRow   = pickedRow;
                    obj.spriteCol   = pickedCol;
                    obj.placementLayer = GetPlacementLayerInput();
                    obj.drawLayer      = GetDrawLayerInput(wallClassId);
                    level.objects.push_back(obj);
                    SetStatus("Muro " + std::to_string(span) + "x" + std::to_string(span)
                              + " libre #" + std::to_string(obj.id) + " en px("
                              + std::to_string(static_cast<int>(obj.pixelX)) + ","
                              + std::to_string(static_cast<int>(obj.pixelY)) + ").");
                }
                else
                {
                    MakeWallBlock(cell, wallClassId);
                }
                break;
            }
            case EditorTool::PlaceSpawn:
                EnsureUniqueSpawn(cell);
                break;
            case EditorTool::PlacePickup:
            {
                LevelObject *obj = MakeObject(Class_Pickup, DefaultTextureId::kPickup, cell);
                if (obj)
                {
                    obj->spriteSheet = SheetId::kPickable;
                    obj->spriteRow   = pickedRow;
                    obj->spriteCol   = pickedCol;
                }
                break;
            }
            case EditorTool::PlacePlate:
                MakeObject(Class_PressurePlate, DefaultTextureId::kPressurePlate, cell);
                break;
            case EditorTool::PlaceButton:
                MakeObject(Class_Button, DefaultTextureId::kButtonOff, cell);
                break;
            case EditorTool::PlacePushable:
            {
                LevelObject *obj = MakeObject(Class_Pushable, DefaultTextureId::kPushable, cell);
                if (obj)
                {
                    obj->spriteSheet = SheetId::kPushable;
                    obj->spriteRow   = pickedRow;
                    obj->spriteCol   = pickedCol;
                }
                break;
            }
            case EditorTool::PlaceDoor:
                MakeObject(Class_Door, DefaultTextureId::kDoorClosed, cell);
                break;
            case EditorTool::PlaceTeleporter:
            {
                LevelObject *obj = MakeObject(Class_Teleporter, DefaultTextureId::kTeleporter, cell);
                if (obj && numericField)
                {
                    // Si el usuario ha escrito un pairId, lo aplicamos.
                    try {
                        const std::string &txt = numericField->GetText();
                        if (!txt.empty()) obj->pairId = std::stoi(txt);
                    } catch (...) { /* ignorar */ }
                }
                if (obj && obj->pairId == 0)
                {
                    // Auto-asigna pairId 1, 2, 3... agrupando de a dos.
                    int next = 1;
                    while (true)
                    {
                        int count = 0;
                        for (const auto &o : level.objects)
                        {
                            if (o.classId == Class_Teleporter && o.pairId == next) ++count;
                        }
                        if (count < 2) { obj->pairId = next; break; }
                        ++next;
                    }
                    SetStatus("Teletransporte pair " + std::to_string(obj->pairId) + " #" + std::to_string(obj->id));
                }
                break;
            }
            case EditorTool::PlaceTorch:
                MakeObject(Class_Torch, 0, cell);
                break;
            case EditorTool::PlaceFirefly:
                MakeObject(Class_Firefly, 0, cell);
                break;
            case EditorTool::PlaceSign:
                MakeObject(Class_Sign, 0, cell);
                break;
            case EditorTool::PlaceDecor:
            {
                if (freeGridMode)
                {
                    glm::vec2 mouseWorld = INP_MN.GetMousePosition();
                    LevelObject obj;
                    obj.id           = level.NextObjectId();
                    obj.classId      = Class_Decor;
                    obj.x            = cell.x;
                    obj.y            = cell.y;
                    obj.pixelX       = mouseWorld.x - kTileSize * 0.5f;
                    obj.pixelY       = mouseWorld.y - kTileSize * 0.5f;
                    obj.spriteSheet  = SheetId::kDecor;
                    obj.spriteRow    = pickedRow;
                    obj.spriteCol    = pickedCol;
                    obj.placementLayer = GetPlacementLayerInput();
                    obj.drawLayer    = GetDrawLayerInput(Class_Decor);
                    level.objects.push_back(obj);
                    SetStatus("Adorno libre #" + std::to_string(obj.id) + " en px("
                              + std::to_string(static_cast<int>(obj.pixelX)) + ","
                              + std::to_string(static_cast<int>(obj.pixelY)) + ").");
                }
                else
                {
                    LevelObject *obj = MakeObject(Class_Decor, 0, cell);
                    if (obj)
                    {
                        obj->spriteSheet = SheetId::kDecor;
                        obj->spriteRow   = pickedRow;
                        obj->spriteCol   = pickedCol;
                    }
                }
                break;
            }
            case EditorTool::PlaceReceptacle:
            {
                LevelObject *obj = MakeObject(Class_PressurePlate, DefaultTextureId::kReceptacle, cell);
                if (obj)
                {
                    obj->activator = Plate_SpecificItem;
                    if (numericField)
                    {
                        try {
                            const std::string &txt = numericField->GetText();
                            if (!txt.empty()) obj->requiredItemTextureId = std::stoi(txt);
                        } catch (...) {}
                    }
                    SetStatus("Receptaculo #" + std::to_string(obj->id) +
                              " (pickup id requerido: " + std::to_string(obj->requiredItemTextureId) + ")");
                }
                break;
            }
            case EditorTool::EraseObject:
                HandleEraseAt(cell);
                break;
            case EditorTool::DuplicatePlacedObject:
                HandleDuplicateObjectAt(cell);
                break;
            case EditorTool::MovePlacedObject:
                HandleMoveObjectAt(cell);
                break;
            case EditorTool::WireActivatorToDoor:
            {
                // Primer click: selecciona un activador (placa o boton).
                // Segundo click: selecciona la puerta y enlaza.
                LevelObject *obj = FindObjectAt(cell);
                if (!obj) { SetStatus("Conectar: no hay objeto en esa capa/celda."); return; }
                if (activatorIdForWire < 0)
                {
                    if (obj->classId != Class_PressurePlate && obj->classId != Class_Button)
                    {
                        SetStatus("Conectar: primero selecciona placa o boton.");
                        return;
                    }
                    activatorIdForWire = obj->id;
                    SetStatus("Activador seleccionado #" + std::to_string(obj->id) + ". Ahora click en la puerta.");
                }
                else
                {
                    if (obj->classId != Class_Door)
                    {
                        SetStatus("Conectar: el segundo click debe ser una puerta.");
                        return;
                    }
                    LevelObject *activator = level.FindObject(activatorIdForWire);
                    if (activator)
                    {
                        if (std::find(activator->targets.begin(), activator->targets.end(), obj->id) == activator->targets.end())
                        {
                            activator->targets.push_back(obj->id);
                            SetStatus("Conectado #" + std::to_string(activator->id) + " -> puerta #" + std::to_string(obj->id));
                        }
                        else
                        {
                            SetStatus("Ya estaba conectado.");
                        }
                    }
                    activatorIdForWire = -1;
                }
                break;
            }
            case EditorTool::EditDialog:
                HandlePickAt(cell);
                break;
            case EditorTool::EditPairId:
            {
                LevelObject *obj = FindObjectAt(cell);
                if (!obj) { SetStatus("PairId: no hay objeto en esa capa/celda."); break; }
                if (obj->classId != Class_Teleporter) { SetStatus("PairId solo aplica a teletransportes."); break; }
                if (numericField)
                {
                    try { obj->pairId = std::stoi(numericField->GetText()); }
                    catch (...) { SetStatus("PairId invalido."); break; }
                }
                SetStatus("Tele #" + std::to_string(obj->id) + " pairId=" + std::to_string(obj->pairId));
                break;
            }
            case EditorTool::EditRequiredItem:
            {
                LevelObject *obj = FindObjectAt(cell);
                if (!obj) { SetStatus("ItemReq: no hay objeto en esa capa/celda."); break; }
                if (obj->classId == Class_Pickup)
                {
                    if (numericField) numericField->SetText(std::to_string(obj->id));
                    SetStatus("Item #" + std::to_string(obj->id) + " capturado. Ahora haz click en el receptaculo.");
                    break;
                }
                if (obj->classId != Class_PressurePlate || obj->textureId != DefaultTextureId::kReceptacle)
                {
                    SetStatus("Solo receptaculos pueden requerir un item.");
                    break;
                }
                if (numericField)
                {
                    const std::string &txt = numericField->GetText();
                    if (!txt.empty())
                    {
                        try { obj->requiredItemTextureId = std::stoi(txt); }
                        catch (...) { SetStatus("Item invalido."); break; }
                    }
                }
                obj->activator = Plate_SpecificItem;
                if (numericField) numericField->SetText(std::to_string(obj->requiredItemTextureId));
                SetStatus("Placa #" + std::to_string(obj->id) + " requiere pickup id=" + std::to_string(obj->requiredItemTextureId));
                break;
            }
            case EditorTool::PlaceBlock:
                MakeObject(Class_Block, 0, cell);
                break;
            case EditorTool::ToggleWinActivator:
            {
                LevelObject *obj = FindObjectAt(cell);
                if (!obj) { SetStatus("Meta: selecciona placa o boton."); break; }
                if (obj->classId != Class_PressurePlate && obj->classId != Class_Button)
                { SetStatus("Meta: solo placas/botones pueden ser parte de la victoria."); break; }
                auto it = std::find(level.winActivatorIds.begin(), level.winActivatorIds.end(), obj->id);
                if (it == level.winActivatorIds.end())
                {
                    level.winActivatorIds.push_back(obj->id);
                    SetStatus("Anyadido a meta: #" + std::to_string(obj->id));
                }
                else
                {
                    level.winActivatorIds.erase(it);
                    SetStatus("Quitado de meta: #" + std::to_string(obj->id));
                }
                UpdateWinListLabel();
                break;
            }
        }
    }

    void EditorScene::UpdateWinListLabel()
    {
        if (!winListLabel) return;
        if (level.winActivatorIds.empty()) { winListLabel->SetText("Meta: (sin condicion)"); return; }
        std::string text = "Meta: ";
        for (std::size_t i = 0; i < level.winActivatorIds.size(); ++i)
        {
            if (i) text += ",";
            text += "#" + std::to_string(level.winActivatorIds[i]);
        }
        winListLabel->SetText(text);
    }

    void EditorScene::HandleEraseAt(Cell cell)
    {
        bool removedSomething = false;
        int placementLayer = GetPlacementLayerInput();

        // Collect ids before erasing so we can clean up the win condition list.
        std::vector<int> erasedIds;
        for (const auto &o : level.objects)
        {
            if (o.placementLayer != placementLayer) continue;
            if (o.x == cell.x && o.y == cell.y) { erasedIds.push_back(o.id); continue; }
            int span = WallObjectSpan(o.classId);
            if (span > 0 && cell.x >= o.x && cell.x < o.x + span && cell.y >= o.y && cell.y < o.y + span)
                erasedIds.push_back(o.id);
        }

        auto before = level.objects.size();
        level.objects.erase(std::remove_if(level.objects.begin(), level.objects.end(),
            [cell, placementLayer](const LevelObject &o)
            {
                if (o.placementLayer != placementLayer) return false;
                if (o.x == cell.x && o.y == cell.y) return true;
                int span = WallObjectSpan(o.classId);
                return span > 0
                    && cell.x >= o.x && cell.x < o.x + span
                    && cell.y >= o.y && cell.y < o.y + span;
            }),
            level.objects.end());
        removedSomething = removedSomething || (level.objects.size() != before);

        // Remove any erased objects that were part of the win condition.
        bool winChanged = false;
        for (int id : erasedIds)
        {
            auto it = std::find(level.winActivatorIds.begin(), level.winActivatorIds.end(), id);
            if (it != level.winActivatorIds.end())
            {
                level.winActivatorIds.erase(it);
                winChanged = true;
            }
        }
        if (winChanged) UpdateWinListLabel();

        removedSomething = level.RemoveTileLayer(cell.x, cell.y, placementLayer) || removedSomething;

        if (removedSomething)
        {
            SetStatus("Contenido borrado en (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ").");
        }
        else
        {
            SetStatus("No habia nada que borrar en (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ").");
        }
    }

    void EditorScene::ValidateWinConditions()
    {
        bool changed = false;
        auto &ids = level.winActivatorIds;
        ids.erase(std::remove_if(ids.begin(), ids.end(), [&](int id)
        {
            bool missing = (level.FindObject(id) == nullptr);
            if (missing) changed = true;
            return missing;
        }), ids.end());
        if (changed) UpdateWinListLabel();
    }

    void EditorScene::SetToolPage(int page)
    {
        currentToolPage = page;
        const int pageSize = 16;
        for (std::size_t i = 0; i < toolButtons.size(); ++i)
        {
            if (!toolButtons[i]) continue;
            int buttonPage = (static_cast<int>(i) < pageSize) ? 0 : 1;
            toolButtons[i]->SetVisible(buttonPage == page);
        }
        if (toolPagePrevButton) toolPagePrevButton->SetFocused(page == 0);
        if (toolPageNextButton) toolPageNextButton->SetFocused(page == 1);
    }

    void EditorScene::HandlePickAt(Cell cell)
    {
        LevelObject *obj = FindObjectAt(cell);
        if (!obj) { SetStatus("Dialogo: no hay objeto en esa capa/celda."); return; }
        if (obj->classId != Class_Sign)
        {
            SetStatus("Dialogo: por ahora solo los letreros aceptan dialogo.");
            return;
        }
        selectedObjectId = obj->id;
        OpenDialogModal(*obj);
    }

    void EditorScene::HandleDuplicateObjectAt(Cell cell)
    {
        LevelObject *obj = FindObjectAt(cell, true);
        if (obj)
        {
            LoadObjectIntoPlacementTool(*obj);
            SetStatus("Duplicar: objeto #" + std::to_string(obj->id) + " cargado como plantilla.");
            return;
        }

        const TileLayer *tile = FindTileLayerAt(cell, GetPlacementLayerInput());
        if (tile)
        {
            LoadTileIntoPlacementTool(*tile);
            SetStatus("Duplicar: tile cargado como plantilla desde (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ").");
            return;
        }

        SetStatus("Duplicar: no hay contenido en esa capa/celda.");
    }

    bool EditorScene::CanPlaceObjectAt(const LevelObject &object, Cell cell) const
    {
        const int span = std::max(1, WallObjectSpan(object.classId));
        if (cell.x < 0 || cell.y < 0 || cell.x + span > level.width || cell.y + span > level.height)
        {
            return false;
        }

        const int objectMinX = cell.x;
        const int objectMinY = cell.y;
        const int objectMaxX = cell.x + span;
        const int objectMaxY = cell.y + span;

        for (const auto &other : level.objects)
        {
            if (other.id == object.id || other.placementLayer != object.placementLayer)
            {
                continue;
            }

            const int otherSpan = std::max(1, WallObjectSpan(other.classId));
            const int otherMinX = other.x;
            const int otherMinY = other.y;
            const int otherMaxX = other.x + otherSpan;
            const int otherMaxY = other.y + otherSpan;

            const bool overlaps = objectMinX < otherMaxX && objectMaxX > otherMinX
                && objectMinY < otherMaxY && objectMaxY > otherMinY;
            if (overlaps)
            {
                return false;
            }
        }

        return true;
    }

    const TileLayer *EditorScene::FindTileLayerAt(Cell cell, int placementLayer) const
    {
        return level.FindTileLayer(cell.x, cell.y, placementLayer);
    }

    void EditorScene::LoadTileIntoPlacementTool(const TileLayer &tile)
    {
        EditorTool tool = IsWallSheet(tile.sheet) ? EditorTool::TileWall : EditorTool::TileFloor;
        currentTool = tool;
        activatorIdForWire = -1;
        selectedObjectId = -1;
        ClearMoveSelection();

        pickedSheet = tile.sheet;
        pickedRow = tile.row;
        pickedCol = tile.col;
        activePlacementLayer = tile.placementLayer;
        activeDrawLayer = tile.drawLayer;

        if (placementLayerField) placementLayerField->SetText(std::to_string(tile.placementLayer));
        if (drawLayerField) drawLayerField->SetText(std::to_string(tile.drawLayer));
        if (toolLabel) toolLabel->SetText(std::string("Herramienta: ") + (tool == EditorTool::TileWall ? "Muro 1x1" : "Suelo"));

        RebuildSheetPickerForTool(tool);
        pickedSheet = tile.sheet;
        pickedRow = tile.row;
        pickedCol = tile.col;
        if (pickerLabel)
        {
            pickerLabel->SetText("Sprite picker: " + std::to_string(pickerRows) + "x" + std::to_string(pickerCols)
                + " sel(" + std::to_string(pickedRow) + "," + std::to_string(pickedCol) + ")");
        }
    }

    void EditorScene::ClearMoveSelection()
    {
        moveSelection = {};
    }

    void EditorScene::HandleMoveObjectAt(Cell cell)
    {
        if (moveSelection.kind == EditorSelectionKind::None)
        {
            LevelObject *clickedObject = FindObjectAt(cell, true);
            if (clickedObject)
            {
                moveSelection.kind = EditorSelectionKind::Object;
                moveSelection.objectId = clickedObject->id;
                moveSelection.cell = Cell{clickedObject->x, clickedObject->y};
                moveSelection.placementLayer = clickedObject->placementLayer;
                selectedObjectId = clickedObject->id;
                SetStatus("Mover: objeto #" + std::to_string(clickedObject->id) + " seleccionado. Ahora elige destino.");
                return;
            }

            const TileLayer *tile = FindTileLayerAt(cell, GetPlacementLayerInput());
            if (tile)
            {
                moveSelection.kind = EditorSelectionKind::Tile;
                moveSelection.cell = cell;
                moveSelection.placementLayer = tile->placementLayer;
                moveSelection.tile = *tile;
                SetStatus("Mover: tile seleccionado en (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + "). Ahora elige destino.");
                return;
            }

            SetStatus("Mover: no hay contenido en esa capa/celda.");
            return;
        }

        if (moveSelection.kind == EditorSelectionKind::Tile)
        {
            if (moveSelection.cell == cell)
            {
                SetStatus("Mover: el tile ya esta en esa celda.");
                return;
            }

            if (FindTileLayerAt(cell, moveSelection.tile.placementLayer))
            {
                SetStatus("Mover: ya existe un tile en esa capa para la celda destino.");
                return;
            }

            level.RemoveTileLayer(moveSelection.cell.x, moveSelection.cell.y, moveSelection.tile.placementLayer);
            level.SetTileLayer(cell.x, cell.y, moveSelection.tile);
            SetStatus("Tile movido a (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ").");
            ClearMoveSelection();
            selectedObjectId = -1;
            return;
        }

        LevelObject *selected = level.FindObject(moveSelection.objectId);
        LevelObject *clickedObject = FindObjectAt(cell, true);
        if (!selected)
        {
            SetStatus("Mover: el objeto seleccionado ya no existe.");
            selectedObjectId = -1;
            ClearMoveSelection();
            return;
        }

        if (clickedObject && clickedObject->id != selected->id)
        {
            moveSelection.kind = EditorSelectionKind::Object;
            moveSelection.objectId = clickedObject->id;
            moveSelection.cell = Cell{clickedObject->x, clickedObject->y};
            moveSelection.placementLayer = clickedObject->placementLayer;
            selectedObjectId = clickedObject->id;
            SetStatus("Mover: objeto #" + std::to_string(clickedObject->id) + " seleccionado. Ahora elige destino.");
            return;
        }

        if (selected->x == cell.x && selected->y == cell.y)
        {
            SetStatus("Mover: el objeto #" + std::to_string(selected->id) + " ya esta en esa celda.");
            return;
        }

        if (!CanPlaceObjectAt(*selected, cell))
        {
            SetStatus("Mover: el objeto no cabe o la celda de destino esta ocupada en esa capa.");
            return;
        }

        selected->x = cell.x;
        selected->y = cell.y;
        SetStatus("Objeto #" + std::to_string(selected->id) + " movido a (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ").");
        selectedObjectId = -1;
        ClearMoveSelection();
    }

    LevelObject *EditorScene::FindObjectAt(Cell cell, bool includeWallCoverage)
    {
        int placementLayer = GetPlacementLayerInput();
        for (auto &object : level.objects)
        {
            if (object.placementLayer != placementLayer) continue;
            if (object.x == cell.x && object.y == cell.y) return &object;
            if (!includeWallCoverage) continue;
            int span = WallObjectSpan(object.classId);
            if (span > 0
                && cell.x >= object.x && cell.x < object.x + span
                && cell.y >= object.y && cell.y < object.y + span)
            {
                return &object;
            }
        }
        return nullptr;
    }

    int EditorScene::GetPlacementLayerInput() const
    {
        return activePlacementLayer;
    }

    int EditorScene::GetDrawLayerInput(int classId) const
    {
        return std::max(0, activeDrawLayer >= 0 ? activeDrawLayer : DefaultDrawLayerForClass(classId));
    }

    void EditorScene::SyncLayerInputsFromFields()
    {
        auto parseOrDefault = [](CosmicEngine::UITextField *field, int fallback)
        {
            if (!field) return fallback;
            std::string text = field->GetText();
            if (text.empty()) return fallback;
            try { return std::max(0, std::stoi(text)); }
            catch (...) { return fallback; }
        };

        activePlacementLayer = parseOrDefault(placementLayerField, activePlacementLayer);
        activeDrawLayer = parseOrDefault(drawLayerField, activeDrawLayer);
    }

    void EditorScene::ApplyBackgroundColorFromFields(bool reportStatus)
    {
        auto parseChannel = [](const CosmicEngine::UITextField *field, int fallback)
        {
            try
            {
                return std::clamp(field ? std::stoi(field->GetText()) : fallback, 0, 255);
            }
            catch (...)
            {
                return fallback;
            }
        };

        level.backgroundColor.r = parseChannel(backgroundRField, level.backgroundColor.r);
        level.backgroundColor.g = parseChannel(backgroundGField, level.backgroundColor.g);
        level.backgroundColor.b = parseChannel(backgroundBField, level.backgroundColor.b);

        if (backgroundRField) backgroundRField->SetText(std::to_string(level.backgroundColor.r));
        if (backgroundGField) backgroundGField->SetText(std::to_string(level.backgroundColor.g));
        if (backgroundBField) backgroundBField->SetText(std::to_string(level.backgroundColor.b));

        if (reportStatus)
        {
            SetStatus("Fondo RGB aplicado: (" + std::to_string(level.backgroundColor.r) + ","
                      + std::to_string(level.backgroundColor.g) + ","
                      + std::to_string(level.backgroundColor.b) + ").");
        }
    }

    void EditorScene::LoadObjectIntoPlacementTool(const LevelObject &object)
    {
        EditorTool tool = EditorTool::PlacePickup;
        std::string label = "Pickup";

        switch (object.classId)
        {
            case Class_Wall2x2: tool = EditorTool::PlaceWall2x2; label = "Muro 2x2"; break;
            case Class_Wall3x3: tool = EditorTool::PlaceWall3x3; label = "Muro 3x3"; break;
            case Class_PlayerSpawn: tool = EditorTool::PlaceSpawn; label = "Spawn"; break;
            case Class_Pickup: tool = EditorTool::PlacePickup; label = "Pickup"; break;
            case Class_PressurePlate:
                if (object.activator == Plate_SpecificItem)
                {
                    tool = EditorTool::PlaceReceptacle;
                    label = "Receptaculo item";
                }
                else
                {
                    tool = EditorTool::PlacePlate;
                    label = "Placa";
                }
                break;
            case Class_Button: tool = EditorTool::PlaceButton; label = "Boton"; break;
            case Class_Pushable: tool = EditorTool::PlacePushable; label = "Empujable"; break;
            case Class_Door: tool = EditorTool::PlaceDoor; label = "Puerta"; break;
            case Class_Teleporter: tool = EditorTool::PlaceTeleporter; label = "Teletransporte"; break;
            case Class_Torch: tool = EditorTool::PlaceTorch; label = "Antorcha"; break;
            case Class_Firefly: tool = EditorTool::PlaceFirefly; label = "Luciernaga"; break;
            case Class_Sign:  tool = EditorTool::PlaceSign;  label = "Letrero"; break;
            case Class_Decor:  tool = EditorTool::PlaceDecor; label = "Adorno (visual)"; break;
            case Class_Block:  tool = EditorTool::PlaceBlock; label = "Bloque de colision (1x1)"; break;
            default:
                SetStatus("Tomar: ese tipo de objeto no se puede reutilizar como plantilla.");
                return;
        }

        currentTool = tool;
        activatorIdForWire = -1;
        selectedObjectId = object.id;
        ClearMoveSelection();
        if (toolLabel) toolLabel->SetText("Herramienta: " + label);
        RebuildSheetPickerForTool(tool);

        if (placementLayerField) placementLayerField->SetText(std::to_string(object.placementLayer));
        if (drawLayerField) drawLayerField->SetText(std::to_string(object.drawLayer));
        activePlacementLayer = object.placementLayer;
        activeDrawLayer = object.drawLayer;
        if (dialogField) dialogField->SetText(object.dialog);

        if (numericField)
        {
            switch (object.classId)
            {
                case Class_Teleporter:
                    numericField->SetText(std::to_string(object.pairId));
                    break;
                case Class_PressurePlate:
                    if (object.activator == Plate_SpecificItem)
                        numericField->SetText(std::to_string(object.requiredItemTextureId));
                    else
                        numericField->SetText("");
                    break;
                default:
                    numericField->SetText("");
                    break;
            }
        }

        if (object.spriteSheet != 0)
        {
            pickedSheet = object.spriteSheet;
            pickedRow   = object.spriteRow;
            pickedCol   = object.spriteCol;
            if (pickerLabel)
            {
                pickerLabel->SetText("Sprite picker: " + std::to_string(pickerRows) + "x" + std::to_string(pickerCols)
                                   + " sel(" + std::to_string(pickedRow) + "," + std::to_string(pickedCol) + ")");
            }
        }
    }

    LevelObject *EditorScene::MakeObject(int classId, int textureId, Cell cell)
    {
        int placementLayer = GetPlacementLayerInput();
        int drawLayer = GetDrawLayerInput(classId);

        // Solo un objeto por celda en la misma capa de colocacion.
        for (auto &o : level.objects)
        {
            if (o.placementLayer != placementLayer) continue;
            if (o.x == cell.x && o.y == cell.y)
            {
                SetStatus("Ya hay un objeto en esa celda para esa capa. Borra primero.");
                return nullptr;
            }
            // Si la celda esta cubierta por un wall block, tambien rechaza.
            int span = WallObjectSpan(o.classId);
            if (span > 0 && cell.x >= o.x && cell.x < o.x + span
                         && cell.y >= o.y && cell.y < o.y + span)
            {
                SetStatus("Celda cubierta por un muro multi-celda en esa capa. Borra primero.");
                return nullptr;
            }
        }
        LevelObject obj;
        obj.id        = level.NextObjectId();
        obj.classId   = classId;
        obj.textureId = textureId;
        obj.x         = cell.x;
        obj.y         = cell.y;
        obj.activator = Plate_Both;
        obj.facing    = 0;
        obj.placementLayer = placementLayer;
        obj.drawLayer      = drawLayer;
        level.objects.push_back(obj);
        SetStatus("Objeto #" + std::to_string(obj.id) + " colocado en (" + std::to_string(cell.x) + "," + std::to_string(cell.y)
            + ") capa=" + std::to_string(placementLayer) + " draw=" + std::to_string(drawLayer) + ".");
        return &level.objects.back();
    }

    LevelObject *EditorScene::MakeWallBlock(Cell topLeft, int classId)
    {
        int span = WallObjectSpan(classId);
        int placementLayer = GetPlacementLayerInput();
        int drawLayer = GetDrawLayerInput(classId);
        if (span <= 0) return nullptr;
        if (topLeft.x + span > level.width || topLeft.y + span > level.height)
        {
            SetStatus("Muro " + std::to_string(span) + "x" + std::to_string(span) + ": no cabe en el mapa.");
            return nullptr;
        }
        // Comprueba colisiones con cualquier objeto existente.
        for (int dy = 0; dy < span; ++dy)
        {
            for (int dx = 0; dx < span; ++dx)
            {
                Cell c{ topLeft.x + dx, topLeft.y + dy };
                if (IsCellBlockedByObject(c, placementLayer))
                {
                    SetStatus("Muro " + std::to_string(span) + "x" + std::to_string(span) + ": celda ocupada en esa capa.");
                    return nullptr;
                }
            }
        }
        LevelObject obj;
        obj.id          = level.NextObjectId();
        obj.classId     = classId;
        obj.textureId   = DefaultTextureId::kWall;
        obj.x           = topLeft.x;
        obj.y           = topLeft.y;
        obj.spriteSheet = (classId == Class_Wall2x2) ? SheetId::kWalls2x2 : SheetId::kWalls3x3;
        obj.spriteRow   = pickedRow;
        obj.spriteCol   = pickedCol;
        obj.placementLayer = placementLayer;
        obj.drawLayer      = drawLayer;
        level.objects.push_back(obj);
        SetStatus("Muro " + std::to_string(span) + "x" + std::to_string(span) + " #" + std::to_string(obj.id)
            + " colocado en (" + std::to_string(topLeft.x) + "," + std::to_string(topLeft.y)
            + ") capa=" + std::to_string(placementLayer) + " draw=" + std::to_string(drawLayer) + ".");
        return &level.objects.back();
    }

    bool EditorScene::IsCellBlockedByObject(Cell cell, int placementLayer) const
    {
        for (const auto &o : level.objects)
        {
            if (o.placementLayer != placementLayer) continue;
            if (o.x == cell.x && o.y == cell.y) return true;
            int span = WallObjectSpan(o.classId);
            if (span > 0 && cell.x >= o.x && cell.x < o.x + span
                         && cell.y >= o.y && cell.y < o.y + span) return true;
        }
        return false;
    }

    void EditorScene::EnsureUniqueSpawn(Cell cell)
    {
        int placementLayer = GetPlacementLayerInput();

        for (const auto &object : level.objects)
        {
            if (object.classId != Class_PlayerSpawn) continue;
            if (object.placementLayer != placementLayer) continue;
            if (object.x == cell.x && object.y == cell.y)
            {
                SetStatus("Ya hay un spawn ahi.");
                return;
            }
        }

        int spawnCount = 0;
        std::size_t firstSpawnIndex = 0;
        for (std::size_t index = 0; index < level.objects.size(); ++index)
        {
            if (level.objects[index].classId != Class_PlayerSpawn) continue;
            if (spawnCount == 0) firstSpawnIndex = index;
            ++spawnCount;
        }

        if (spawnCount >= 2)
        {
            level.objects.erase(level.objects.begin() + static_cast<std::ptrdiff_t>(firstSpawnIndex));
        }

        LevelObject spawn;
        spawn.id        = level.NextObjectId();
        spawn.classId   = Class_PlayerSpawn;
        spawn.textureId = DefaultTextureId::kPlayerDown;
        spawn.x         = cell.x;
        spawn.y         = cell.y;
        spawn.placementLayer = placementLayer;
        spawn.drawLayer      = GetDrawLayerInput(Class_PlayerSpawn);
        level.objects.push_back(spawn);
        SetStatus("Spawn colocado en (" + std::to_string(cell.x) + "," + std::to_string(cell.y) + ").");
    }

    void EditorScene::draw()
    {
        RS_MN.RenderRectangle(glm::vec2(-4096.0f, -4096.0f), glm::vec2(4096.0f, 4096.0f),
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              glm::vec3(level.backgroundColor.r / 255.0f,
                                        level.backgroundColor.g / 255.0f,
                                        level.backgroundColor.b / 255.0f),
                              1.0f, 1.0f, true);

        DrawGrid();
        DrawObjects();
        DrawPlacementPreview();
        if (IsPuzzleDebugVisualsEnabled()) DrawWorldBounds();
    }

    void EditorScene::DrawSheetPickerPreviews() const
    {
        // Picker previews are now drawn by their own UI elements so they are
        // rendered in the same pass and z-order as the rest of the editor UI.
    }

    void EditorScene::DrawGrid() const
    {
        // Celdas (suelo/muro) con tonalidades claramente distintas.
        for (int y = 0; y < level.height; ++y)
        {
            for (int x = 0; x < level.width; ++x)
            {
                TileCell tc = level.TileAt(x, y);
                glm::vec2 origin(x * kTileSize, y * kTileSize);
                glm::vec2 farCorner = origin + glm::vec2(kTileSize, kTileSize);

                bool drewAnySprite = false;
                bool hasWallLayer = false;
                for (const auto &layer : tc.layers)
                {
                    hasWallLayer = hasWallLayer || IsWallSheet(layer.sheet);
                    bool drewSprite = DrawSheetCell(SheetKeyForSheetId(layer.sheet), layer.row, layer.col,
                                                    origin, glm::vec2(kTileSize, kTileSize));
                    drewAnySprite = drewAnySprite || drewSprite;
                    if (!drewSprite && IsPuzzleDebugVisualsEnabled())
                    {
                        glm::vec3 color = IsWallSheet(layer.sheet)
                                          ? glm::vec3(0.18f, 0.12f, 0.10f)
                                          : glm::vec3(0.55f, 0.62f, 0.58f);
                        RS_MN.RenderRectangle(origin, farCorner, glm::vec2(0.0f), glm::vec2(0.0f),
                                              color, 1.0f, 1.0f, true);
                    }
                }

                if (!drewAnySprite && IsPuzzleDebugVisualsEnabled())
                {
                    glm::vec3 color = hasWallLayer
                                      ? glm::vec3(0.18f, 0.12f, 0.10f)
                                      : glm::vec3(0.05f, 0.05f, 0.08f);
                    RS_MN.RenderRectangle(origin, farCorner, glm::vec2(0.0f), glm::vec2(0.0f),
                                          color, 1.0f, 1.0f, true);
                    if (hasWallLayer)
                    {
                        glm::vec2 inA = origin + glm::vec2(2.0f, 2.0f);
                        glm::vec2 inB = farCorner - glm::vec2(2.0f, 2.0f);
                        RS_MN.RenderRectangle(inA, inB, glm::vec2(0.0f), glm::vec2(0.0f),
                                              glm::vec3(0.35f, 0.22f, 0.18f), 1.0f, 1.0f, true);
                    }
                }
            }
        }
        if (IsPuzzleDebugVisualsEnabled())
        {
            // Lineas de la cuadricula bien visibles.
            const glm::vec3 gridColor(0.95f, 0.95f, 0.95f);
            const float gridAlpha = 0.55f;
            const float lineHalf  = std::max(0.25f, kTileSize * 0.04f);
            for (int x = 0; x <= level.width; ++x)
            {
                glm::vec2 a(x * kTileSize - lineHalf, 0.0f);
                glm::vec2 b(x * kTileSize + lineHalf, level.height * kTileSize);
                RS_MN.RenderRectangle(a, b, glm::vec2(0.0f), glm::vec2(0.0f),
                                      gridColor, gridAlpha, 1.0f, true);
            }
            for (int y = 0; y <= level.height; ++y)
            {
                glm::vec2 a(0.0f, y * kTileSize - lineHalf);
                glm::vec2 b(level.width * kTileSize, y * kTileSize + lineHalf);
                RS_MN.RenderRectangle(a, b, glm::vec2(0.0f), glm::vec2(0.0f),
                                      gridColor, gridAlpha, 1.0f, true);
            }
        }
    }

    void EditorScene::DrawWorldBounds() const
    {
        // Marco brillante alrededor del nivel para marcar los limites del mundo.
        const glm::vec3 borderColor(1.0f, 0.85f, 0.20f);
        const float thickness = 5.0f;
        float w = level.width  * kTileSize;
        float h = level.height * kTileSize;
        // Top
        RS_MN.RenderRectangle(glm::vec2(-thickness, -thickness),
                              glm::vec2(w + thickness, 0.0f),
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              borderColor, 1.0f, 1.0f, true);
        // Bottom
        RS_MN.RenderRectangle(glm::vec2(-thickness, h),
                              glm::vec2(w + thickness, h + thickness),
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              borderColor, 1.0f, 1.0f, true);
        // Left
        RS_MN.RenderRectangle(glm::vec2(-thickness, 0.0f),
                              glm::vec2(0.0f, h),
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              borderColor, 1.0f, 1.0f, true);
        // Right
        RS_MN.RenderRectangle(glm::vec2(w, 0.0f),
                              glm::vec2(w + thickness, h),
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              borderColor, 1.0f, 1.0f, true);
    }

    void EditorScene::DrawObjects() const
    {
        auto classLabel = [](int classId) -> std::string
        {
            switch (classId)
            {
                case Class_Pickup:        return "Pickup";
                case Class_PlayerSpawn:   return "Spawn";
                case Class_PressurePlate: return "Placa";
                case Class_Door:          return "Puerta";
                case Class_Button:        return "Boton";
                case Class_Pushable:      return "Pushable";
                case Class_Teleporter:    return "Teleporter";
                case Class_Torch:         return "Antorcha";
                case Class_Firefly:       return "Luciernaga";
                case Class_Sign:          return "Letrero";
                case Class_Decor:         return "Adorno";
                case Class_Block:         return "Bloque";
                default:                  return "Objeto";
            }
        };

        auto plateModeLabel = [](int activator) -> std::string
        {
            switch (activator)
            {
                case Plate_Player:       return "player";
                case Plate_Object:       return "object";
                case Plate_Both:         return "both";
                case Plate_SpecificItem: return "item";
                default:                 return "?";
            }
        };

        std::vector<const LevelObject *> orderedObjects;
        orderedObjects.reserve(level.objects.size());
        for (const auto &object : level.objects) orderedObjects.push_back(&object);
        std::sort(orderedObjects.begin(), orderedObjects.end(),
            [](const LevelObject *left, const LevelObject *right)
            {
                if (left->drawLayer != right->drawLayer) return left->drawLayer < right->drawLayer;
                return left->id < right->id;
            });

        // Objetos coloreados por clase.
        for (const LevelObject *object : orderedObjects)
        {
            const auto &o = *object;
            // Muros multi-celda: sprite cubre span x span y se dibuja entero.
            int wallSpan = WallObjectSpan(o.classId);
            if (wallSpan > 0)
            {
                glm::vec2 origin = (o.pixelX != 0.0f || o.pixelY != 0.0f)
                    ? glm::vec2(o.pixelX, o.pixelY)
                    : glm::vec2(o.x * kTileSize, o.y * kTileSize);
                glm::vec2 size(kTileSize * wallSpan, kTileSize * wallSpan);
                int sheet = o.spriteSheet != 0
                                ? o.spriteSheet
                                : (o.classId == Class_Wall2x2 ? SheetId::kWalls2x2 : SheetId::kWalls3x3);
                bool drewSprite = DrawSheetCell(SheetKeyForSheetId(sheet), o.spriteRow, o.spriteCol,
                                                origin, size);
                if (!drewSprite && IsPuzzleDebugVisualsEnabled())
                {
                    RS_MN.RenderRectangle(origin, origin + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                          glm::vec3(0.30f, 0.20f, 0.15f), 1.0f, 1.0f, true);
                    RS_MN.RenderRectangle(origin, origin + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                          glm::vec3(0.95f, 0.80f, 0.30f), 1.0f, 2.0f, false);
                }
                if (IsPuzzleDebugVisualsEnabled())
                {
                    RenderDebugTextBlock(origin + size * 0.5f,
                                         {
                                             {"W" + std::to_string(wallSpan) + "x" + std::to_string(wallSpan),
                                              glm::vec3(0.95f, 0.85f, 0.55f)},
                                             {"#" + std::to_string(o.id), glm::vec3(1.0f, 0.92f, 0.45f)}
                                         }, 0.11f, 4.4f, 20.0f);
                }
                continue;
            }

            glm::vec2 origin(o.x * kTileSize, o.y * kTileSize);
            if (o.classId == Class_Decor && (o.pixelX != 0.0f || o.pixelY != 0.0f))
                origin = glm::vec2(o.pixelX, o.pixelY);
            glm::vec2 size(kTileSize, kTileSize);
            glm::vec3 color(1.0f);
            const char *previewClip = nullptr;
            bool drewVariant = false;
            bool drewCustom = false;
            if (o.spriteSheet != 0 && (o.classId == Class_Pickup || o.classId == Class_Pushable || o.classId == Class_Decor))
            {
                drewVariant = DrawSheetCell(SheetKeyForSheetId(o.spriteSheet), o.spriteRow, o.spriteCol,
                                            origin, size);
            }
            switch (o.classId)
            {
                case Class_Pickup:
                    color = glm::vec3(0.95f, 0.85f, 0.30f);
                    previewClip = ClipForTextureId(o.textureId);
                    break;
                case Class_PlayerSpawn:
                {
                    color = glm::vec3(0.30f, 0.45f, 0.95f);
                    const glm::vec2 spawnOrigin(o.x * kTileSize, (o.y - 1.0f) * kTileSize);
                    const glm::vec2 spawnSize(kTileSize, 2.0f * kTileSize);
                    if (!DrawClipFirstFrame(ClipForPlayer(0, false), spawnOrigin, spawnSize))
                    {
                        if (IsPuzzleDebugVisualsEnabled())
                            RS_MN.RenderRectangle(spawnOrigin, spawnOrigin + spawnSize,
                                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                                  color, 1.0f, 1.0f, true);
                    }
                    drewCustom = true;
                    break;
                }
                case Class_PressurePlate:
                    color = (o.activator == Plate_SpecificItem)
                            ? glm::vec3(0.20f, 0.60f, 0.45f)
                            : glm::vec3(0.70f, 0.20f, 0.85f);
                    previewClip = ClipForPressurePlate(false, o.textureId);
                    break;
                case Class_Button:
                    color = glm::vec3(0.20f, 0.85f, 0.95f);
                    previewClip = ClipForButton(false, o.textureId);
                    break;
                case Class_Pushable:
                    color = glm::vec3(0.65f, 0.45f, 0.25f);
                    previewClip = ClipForTextureId(o.textureId);
                    break;
                case Class_Door:
                    color = glm::vec3(0.85f, 0.45f, 0.30f);
                    previewClip = ClipForDoor(false, o.textureId);
                    break;
                case Class_Teleporter:
                    color = glm::vec3(0.30f, 0.85f, 0.95f);
                    previewClip = ClipForTextureId(o.textureId);
                    break;
                case Class_Torch:
                {
                    color = glm::vec3(1.0f, 0.62f, 0.18f);
                    const glm::vec2 spriteSize(size.x * 0.5f, size.y);
                    const glm::vec2 spriteOrigin = origin + glm::vec2((size.x - spriteSize.x) * 0.5f, 0.0f);
                    if (!DrawClipFirstFrame(PUZZLERPG_CLIP_TORCH_IDLE, spriteOrigin, spriteSize))
                    {
                        DrawTorchMarkerPrimitive(origin, size, 1.0f);
                    }
                    drewCustom = true;
                    break;
                }
                case Class_Firefly:
                    color = glm::vec3(0.88f, 1.0f, 0.55f);
                    DrawFireflyMarkerPrimitive(origin, size, 1.0f);
                    drewCustom = true;
                    break;
                case Class_Sign:
                    color = glm::vec3(0.92f, 0.84f, 0.62f);
                    if (!DrawClipFirstFrame(PUZZLERPG_CLIP_SIGN_IDLE, origin, size))
                    {
                        DrawSignMarkerPrimitive(origin, size, 1.0f);
                    }
                    drewCustom = true;
                    break;
                case Class_Block:
                    RS_MN.RenderRectangle(origin, origin + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                          glm::vec3(1.0f, 0.25f, 0.15f), 0.35f, 1.0f, true);
                    RS_MN.RenderRectangle(origin, origin + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                          glm::vec3(1.0f, 0.25f, 0.15f), 0.85f, 2.0f, false);
                    drewCustom = true;
                    break;
                default:                     color = glm::vec3(0.7f); break;
            }
            if (!drewCustom && !drewVariant && !DrawClipFirstFrame(previewClip, origin, size))
            {
                if (IsPuzzleDebugVisualsEnabled())
                {
                    RS_MN.RenderRectangle(origin, origin + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                          color, 1.0f, 1.0f, true);
                }
            }

            if (IsPuzzleDebugVisualsEnabled())
            {
                std::vector<std::pair<std::string, glm::vec3>> lines;
                lines.push_back({classLabel(o.classId) + " #" + std::to_string(o.id), glm::vec3(0.75f, 0.95f, 1.0f)});
                lines.push_back({"P" + std::to_string(o.placementLayer) + "/D" + std::to_string(o.drawLayer),
                                 glm::vec3(0.95f, 0.80f, 0.55f)});

                switch (o.classId)
                {
                    case Class_PressurePlate:
                        lines.push_back({"A:" + plateModeLabel(o.activator), glm::vec3(0.95f, 0.65f, 1.0f)});
                        if (o.requiredItemTextureId > 0)
                        {
                            lines.push_back({"R:" + std::to_string(o.requiredItemTextureId), glm::vec3(1.0f, 0.72f, 0.58f)});
                        }
                        break;
                    case Class_Button:
                        lines.push_back({"F:" + std::to_string(o.facing), glm::vec3(0.45f, 0.98f, 1.0f)});
                        break;
                    case Class_Teleporter:
                        if (o.pairId > 0)
                        {
                            lines.push_back({"Pair:" + std::to_string(o.pairId), glm::vec3(0.45f, 0.98f, 1.0f)});
                        }
                        break;
                    default:
                        break;
                }

                if (!o.targets.empty())
                {
                    std::string targets = ">";
                    for (std::size_t i = 0; i < o.targets.size() && i < 2; ++i)
                    {
                        if (i > 0) targets += ",";
                        targets += std::to_string(o.targets[i]);
                    }
                    if (o.targets.size() > 2) targets += "+";
                    lines.push_back({targets, glm::vec3(1.0f, 0.72f, 0.58f)});
                }
                RenderDebugTextBlock(glm::vec2((static_cast<float>(o.x) + 0.5f) * kTileSize,
                                               (static_cast<float>(o.y) + 0.5f) * kTileSize),
                                     lines, 0.11f, 4.4f, 20.0f);
            }
        }
    }

    void EditorScene::DrawPlacementPreview() const
    {
        if (IsMouseOverUi())
        {
            return;
        }

        glm::vec2 mouse = INP_MN.GetMousePosition();
        Cell hoveredCell{0, 0};
        const bool hasHoveredCell = MouseToCell(hoveredCell);

        auto drawPreviewRect = [&](glm::vec2 position, glm::vec2 size, glm::vec3 color)
        {
            RS_MN.RenderRectangle(position,
                                  position + size,
                                  glm::vec2(0.0f),
                                  glm::vec2(0.0f),
                                  color,
                                  0.35f,
                                  1.0f,
                                  true);
            RS_MN.RenderRectangle(position,
                                  position + size,
                                  glm::vec2(0.0f),
                                  glm::vec2(0.0f),
                                  color,
                                  0.90f,
                                  2.0f,
                                  false);
        };

        auto drawObjectPreview = [&](const LevelObject &object)
        {
            const int wallSpan = std::max(1, WallObjectSpan(object.classId));
            const bool isSpawn  = (object.classId == Class_PlayerSpawn);
            // Free mode applies to decorations and multi-cell wall blocks.
            const bool isFree   = freeGridMode && (object.classId == Class_Decor
                                                || object.classId == Class_Wall2x2
                                                || object.classId == Class_Wall3x3);

            const glm::vec2 size = isSpawn
                ? glm::vec2(kTileSize, 2.0f * kTileSize)
                : glm::vec2(kTileSize * wallSpan, kTileSize * wallSpan);

            // Snapped grid position (top-left of the target cell).
            const glm::vec2 snappedPosition = isSpawn
                ? glm::vec2(hoveredCell.x * kTileSize, (hoveredCell.y - 1.0f) * kTileSize)
                : glm::vec2(hoveredCell.x * kTileSize, hoveredCell.y * kTileSize);

            // Mouse-relative position (centered on cursor).
            const glm::vec2 mousePosition = isSpawn
                ? mouse - glm::vec2(kTileSize * 0.5f, kTileSize * 1.5f)
                : mouse - size * 0.5f;

            // When snapping is active and we have a valid cell, render at snapped position.
            const glm::vec2 position = (hasHoveredCell && !isFree) ? snappedPosition : mousePosition;

            auto drawTorchPreview = [&]()
            {
                const glm::vec2 spriteSize(size.x * 0.5f, size.y);
                const glm::vec2 spriteOrigin = position + glm::vec2((size.x - spriteSize.x) * 0.5f, 0.0f);
                if (!DrawClipFirstFrame(PUZZLERPG_CLIP_TORCH_IDLE, spriteOrigin, spriteSize, 0.0f, glm::vec3(1.0f), 0.85f))
                {
                    DrawTorchMarkerPrimitive(position, size, 0.85f);
                }
            };

            auto drawFireflyPreview = [&]()
            {
                DrawFireflyMarkerPrimitive(position, size, 0.85f);
            };

            bool drewSprite = false;
            if (wallSpan > 1)
            {
                const int sheet = object.spriteSheet != 0
                    ? object.spriteSheet
                    : (object.classId == Class_Wall2x2 ? SheetId::kWalls2x2 : SheetId::kWalls3x3);
                drewSprite = DrawSheetCell(SheetKeyForSheetId(sheet),
                                           object.spriteRow,
                                           object.spriteCol,
                                           position,
                                           size,
                                           0.0f,
                                           glm::vec3(1.0f),
                                           0.70f);
            }
            else if (object.spriteSheet != 0 && (object.classId == Class_Pickup || object.classId == Class_Pushable || object.classId == Class_Decor))
            {
                drewSprite = DrawSheetCell(SheetKeyForSheetId(object.spriteSheet),
                                           object.spriteRow,
                                           object.spriteCol,
                                           position,
                                           size,
                                           0.0f,
                                           glm::vec3(1.0f),
                                           0.70f);
            }

            if (!drewSprite)
            {
                const char *clip = nullptr;
                switch (object.classId)
                {
                    case Class_PlayerSpawn:   clip = ClipForPlayer(0, false); break;
                    case Class_Pickup:        clip = ClipForTextureId(object.textureId); break;
                    case Class_PressurePlate: clip = ClipForPressurePlate(false, object.textureId); break;
                    case Class_Button:        clip = ClipForButton(false, object.textureId); break;
                    case Class_Pushable:      clip = ClipForTextureId(object.textureId); break;
                    case Class_Door:          clip = ClipForDoor(false, object.textureId); break;
                    case Class_Teleporter:    clip = ClipForTextureId(object.textureId); break;
                    case Class_Torch:         drawTorchPreview(); drewSprite = true; break;
                    case Class_Firefly:       drawFireflyPreview(); drewSprite = true; break;
                    case Class_Sign:
                        if (!DrawClipFirstFrame(PUZZLERPG_CLIP_SIGN_IDLE, position, size, 0.0f, glm::vec3(1.0f), 0.85f))
                        {
                            DrawSignMarkerPrimitive(position, size, 0.85f);
                        }
                        drewSprite = true;
                        break;
                    case Class_Block:
                        RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                              glm::vec3(1.0f, 0.25f, 0.15f), 0.35f, 1.0f, true);
                        RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f),
                                              glm::vec3(1.0f, 0.25f, 0.15f), 0.85f, 2.0f, false);
                        drewSprite = true;
                        break;
                    default:                  clip = nullptr; break;
                }
                if (!drewSprite)
                {
                    drewSprite = DrawClipFirstFrame(clip, position, size, 0.0f, glm::vec3(1.0f), 0.70f);
                }
            }

            if (!drewSprite)
            {
                drawPreviewRect(position, size, glm::vec3(0.90f, 0.82f, 0.35f));
            }

            // Snap border: green = can place, red = blocked. Hidden in free mode.
            if (hasHoveredCell && !isFree)
            {
                const glm::vec2 snapSz = isSpawn
                    ? glm::vec2(kTileSize, 2.0f * kTileSize)
                    : glm::vec2(kTileSize * wallSpan, kTileSize * wallSpan);
                const glm::vec3 snapColor = CanPlaceObjectAt(object, hoveredCell)
                    ? glm::vec3(0.25f, 1.0f, 0.45f)
                    : glm::vec3(1.0f, 0.35f, 0.30f);
                RS_MN.RenderRectangle(snappedPosition,
                                      snappedPosition + snapSz,
                                      glm::vec2(0.0f),
                                      glm::vec2(0.0f),
                                      snapColor,
                                      0.90f,
                                      2.0f,
                                      false);
            }
        };

        auto drawTilePreview = [&](int sheet)
        {
            const glm::vec2 size(kTileSize, kTileSize);
            const glm::vec2 snappedPosition(hoveredCell.x * kTileSize, hoveredCell.y * kTileSize);
            const glm::vec2 mousePosition = mouse - size * 0.5f;
            // In free mode tiles follow the mouse; otherwise snap to the hovered cell.
            const glm::vec2 position = (hasHoveredCell && !freeGridMode) ? snappedPosition : mousePosition;

            bool drewSprite = DrawSheetCell(SheetKeyForSheetId(sheet),
                                            pickedRow,
                                            pickedCol,
                                            position,
                                            size,
                                            0.0f,
                                            glm::vec3(1.0f),
                                            0.70f);
            if (!drewSprite)
            {
                drawPreviewRect(position, size, sheet == SheetId::kWalls1x1
                    ? glm::vec3(0.32f, 0.22f, 0.18f)
                    : glm::vec3(0.55f, 0.62f, 0.58f));
            }

            if (hasHoveredCell && !freeGridMode)
            {
                RS_MN.RenderRectangle(snappedPosition,
                                      snappedPosition + size,
                                      glm::vec2(0.0f),
                                      glm::vec2(0.0f),
                                      glm::vec3(0.25f, 1.0f, 0.45f),
                                      0.90f,
                                      2.0f,
                                      false);
            }
        };

        switch (currentTool)
        {
            case EditorTool::TileFloor:
                drawTilePreview(SheetId::kGround);
                break;
            case EditorTool::TileWall:
                drawTilePreview(SheetId::kWalls1x1);
                break;
            case EditorTool::PlaceWall2x2:
            case EditorTool::PlaceWall3x3:
            case EditorTool::PlaceSpawn:
            case EditorTool::PlacePickup:
            case EditorTool::PlacePlate:
            case EditorTool::PlaceButton:
            case EditorTool::PlacePushable:
            case EditorTool::PlaceDoor:
            case EditorTool::PlaceTeleporter:
            case EditorTool::PlaceTorch:
            case EditorTool::PlaceFirefly:
            case EditorTool::PlaceSign:
            case EditorTool::PlaceReceptacle:
            case EditorTool::PlaceDecor:
            case EditorTool::PlaceBlock:
            {
                const int classId = ClassIdForPlacementTool(currentTool);
                if (classId == Class_None)
                {
                    break;
                }

                LevelObject previewObject;
                previewObject.classId = classId;
                switch (classId)
                {
                    case Class_PlayerSpawn:   previewObject.textureId = DefaultTextureId::kPlayerDown; break;
                    case Class_Pickup:        previewObject.textureId = DefaultTextureId::kPickup; break;
                    case Class_PressurePlate: previewObject.textureId = DefaultTextureId::kPressurePlate; break;
                    case Class_Button:        previewObject.textureId = DefaultTextureId::kButtonOff; break;
                    case Class_Pushable:      previewObject.textureId = DefaultTextureId::kPushable; break;
                    case Class_Door:          previewObject.textureId = DefaultTextureId::kDoorClosed; break;
                    case Class_Teleporter:    previewObject.textureId = DefaultTextureId::kTeleporter; break;
                    case Class_Torch:
                    case Class_Firefly:
                    case Class_Sign:
                    case Class_Decor:
                    case Class_Block:         previewObject.textureId = 0; break;
                    default:                  previewObject.textureId = DefaultTextureId::kWall; break;
                }
                previewObject.placementLayer = GetPlacementLayerInput();
                previewObject.drawLayer = GetDrawLayerInput(classId);
                previewObject.spriteSheet = pickedSheet;
                previewObject.spriteRow = pickedRow;
                previewObject.spriteCol = pickedCol;

                if (currentTool == EditorTool::PlaceReceptacle)
                {
                    previewObject.textureId = DefaultTextureId::kReceptacle;
                    previewObject.activator = Plate_SpecificItem;
                }
                else if (currentTool == EditorTool::PlacePlate)
                {
                    previewObject.textureId = DefaultTextureId::kPressurePlate;
                    previewObject.activator = Plate_Both;
                }
                else if (currentTool == EditorTool::PlaceButton)
                {
                    previewObject.textureId = DefaultTextureId::kButtonOff;
                }
                else if (currentTool == EditorTool::PlaceDoor)
                {
                    previewObject.textureId = DefaultTextureId::kDoorClosed;
                }

                drawObjectPreview(previewObject);
                break;
            }
            case EditorTool::MovePlacedObject:
            {
                if (moveSelection.kind == EditorSelectionKind::Object)
                {
                    const LevelObject *selected = level.FindObject(moveSelection.objectId);
                    if (selected)
                    {
                        drawObjectPreview(*selected);
                    }
                }
                else if (moveSelection.kind == EditorSelectionKind::Tile)
                {
                    const glm::vec2 size(kTileSize, kTileSize);
                    const glm::vec2 position = mouse - size * 0.5f;
                    bool drewSprite = DrawSheetCell(SheetKeyForSheetId(moveSelection.tile.sheet),
                                                    moveSelection.tile.row,
                                                    moveSelection.tile.col,
                                                    position,
                                                    size,
                                                    0.0f,
                                                    glm::vec3(1.0f),
                                                    0.70f);
                    if (!drewSprite)
                    {
                        drawPreviewRect(position, size, glm::vec3(0.55f, 0.62f, 0.58f));
                    }

                    if (hasHoveredCell)
                    {
                        const glm::vec2 snappedPosition(hoveredCell.x * kTileSize, hoveredCell.y * kTileSize);
                        const glm::vec3 snappedColor = FindTileLayerAt(hoveredCell, moveSelection.tile.placementLayer)
                            ? glm::vec3(1.0f, 0.35f, 0.30f)
                            : glm::vec3(0.25f, 1.0f, 0.45f);
                        RS_MN.RenderRectangle(snappedPosition,
                                              snappedPosition + size,
                                              glm::vec2(0.0f),
                                              glm::vec2(0.0f),
                                              snappedColor,
                                              0.90f,
                                              2.0f,
                                              false);
                    }
                }
                else if (selectedObjectId > 0)
                {
                    const LevelObject *selected = level.FindObject(selectedObjectId);
                    if (selected)
                    {
                    drawObjectPreview(*selected);
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    void EditorScene::reset() {}

    void EditorScene::UpdateDimensionsLabel()
    {
        if (!dimensionsLabel) return;
        dimensionsLabel->SetText("Tam:" + std::to_string(level.width) + "x" + std::to_string(level.height));
    }

    void EditorScene::ApplyResizeFromFields()
    {
        if (!widthField || !heightField) return;
        int newW = level.width;
        int newH = level.height;
        try { newW = std::stoi(widthField->GetText()); } catch (...) {}
        try { newH = std::stoi(heightField->GetText()); } catch (...) {}
        newW = std::max(4, std::min(64, newW));
        newH = std::max(4, std::min(64, newH));

        // Redimensiona la matriz de tiles preservando el contenido existente.
        std::vector<std::vector<TileCell>> newTiles(
            newH, std::vector<TileCell>(newW, TileCell{}));
        for (int y = 0; y < newH; ++y)
        {
            for (int x = 0; x < newW; ++x)
            {
                if (y < level.height && x < level.width)
                {
                    newTiles[y][x] = level.tiles[y][x];
                }
            }
        }
        level.tiles  = std::move(newTiles);
        level.width  = newW;
        level.height = newH;

        // Quita objetos / spawns que quedaron fuera del nuevo limite.
        level.objects.erase(std::remove_if(level.objects.begin(), level.objects.end(),
            [&](const LevelObject &o)
            {
                int span = WallObjectSpan(o.classId);
                if (span > 0) return (o.x + span) > newW || (o.y + span) > newH;
                return o.x >= newW || o.y >= newH;
            }),
            level.objects.end());

        UpdateDimensionsLabel();
        widthField->SetText(std::to_string(level.width));
        heightField->SetText(std::to_string(level.height));
        SetStatus("Tamanyo del mapa: " + std::to_string(level.width) + "x" + std::to_string(level.height));
    }

    void EditorScene::OpenDialogModal(LevelObject &object)
    {
        dialogTargetObjectId = object.id;
        dialogModalOpen = true;
        if (dialogField)
        {
            dialogField->SetText(object.dialog);
            dialogField->SetFocused(true);
        }
        UpdateContextualUiVisibility();
        SetStatus("Editando dialogo del letrero #" + std::to_string(object.id) + ".");
    }

    void EditorScene::CloseDialogModal()
    {
        dialogModalOpen = false;
        dialogTargetObjectId = -1;
        if (dialogField)
        {
            dialogField->SetFocused(false);
        }
        UpdateContextualUiVisibility();
    }

    void EditorScene::ConfirmDialogModal()
    {
        auto it = std::find_if(level.objects.begin(), level.objects.end(), [this](const LevelObject &candidate)
        {
            return candidate.id == dialogTargetObjectId;
        });
        if (it == level.objects.end())
        {
            CloseDialogModal();
            SetStatus("Dialogo: el letrero ya no existe.");
            return;
        }

        it->dialog = dialogField ? dialogField->GetText() : std::string();
        selectedObjectId = it->id;
        CloseDialogModal();
        SetStatus("Dialogo asignado al letrero #" + std::to_string(it->id) + ".");
    }

    void EditorScene::UpdateContextualUiVisibility()
    {
        const bool showLayers = currentTool != EditorTool::MoveCamera;
        const bool showDialog = currentTool == EditorTool::EditDialog;
        const bool showNumeric = currentTool == EditorTool::PlaceTeleporter
                              || currentTool == EditorTool::EditPairId
                              || currentTool == EditorTool::PlaceReceptacle
                              || currentTool == EditorTool::EditRequiredItem;
        const bool showWinList = currentTool == EditorTool::ToggleWinActivator;
        const bool showPicker = UsesSpritePicker(currentTool);

        if (layerLabel) layerLabel->SetVisible(showLayers);
        if (placementLayerField) placementLayerField->SetVisible(showLayers);
        if (drawLayerField) drawLayerField->SetVisible(showLayers);

        if (numericLabel)
        {
            if (currentTool == EditorTool::PlaceTeleporter || currentTool == EditorTool::EditPairId)
                numericLabel->SetText("PairId:");
            else if (currentTool == EditorTool::PlaceReceptacle || currentTool == EditorTool::EditRequiredItem)
                numericLabel->SetText("Item req:");
            else
                numericLabel->SetText("Valor:");
            numericLabel->SetVisible(showNumeric);
        }

        if (numericField)
        {
            numericField->SetVisible(showNumeric);
            if (!showNumeric) numericField->SetFocused(false);
        }

        if (dialogHintLabel)
        {
            dialogHintLabel->SetVisible(showDialog && !dialogModalOpen);
        }

        if (dialogModalBackdrop) dialogModalBackdrop->SetVisible(dialogModalOpen);
        if (dialogModalPanel) dialogModalPanel->SetVisible(dialogModalOpen);
        if (dialogModalTitle) dialogModalTitle->SetVisible(dialogModalOpen);
        if (dialogModalHint) dialogModalHint->SetVisible(dialogModalOpen);
        if (dialogField)
        {
            dialogField->SetVisible(dialogModalOpen);
            if (!dialogModalOpen) dialogField->SetFocused(false);
        }
        if (dialogConfirmButton) dialogConfirmButton->SetVisible(dialogModalOpen);
        if (dialogCancelButton) dialogCancelButton->SetVisible(dialogModalOpen);

        if (winListLabel) winListLabel->SetVisible(showWinList);
        if (pickerLabel) pickerLabel->SetVisible(showPicker);
        for (auto *button : sheetPickerButtons)
        {
            if (button) button->SetVisible(showPicker);
        }

        if (backgroundLabel) backgroundLabel->SetVisible(backgroundControlsVisible);
        if (backgroundRField)
        {
            backgroundRField->SetVisible(backgroundControlsVisible);
            if (!backgroundControlsVisible) backgroundRField->SetFocused(false);
        }
        if (backgroundGField)
        {
            backgroundGField->SetVisible(backgroundControlsVisible);
            if (!backgroundControlsVisible) backgroundGField->SetFocused(false);
        }
        if (backgroundBField)
        {
            backgroundBField->SetVisible(backgroundControlsVisible);
            if (!backgroundControlsVisible) backgroundBField->SetFocused(false);
        }
        if (applyBackgroundButton) applyBackgroundButton->SetVisible(backgroundControlsVisible);
        if (backgroundToggleButton) backgroundToggleButton->SetText(backgroundControlsVisible ? "Ocultar fondo" : "Color fondo");
    }
}
