# Third-Party Licenses

This folder contains local copies of the upstream license texts for the third-party libraries vendored under `include/`.

## Mapping

| Include path | Dependency | Local license copy | Upstream reference |
| --- | --- | --- | --- |
| `include/assimp/` | assimp | `assimp_LICENSE.txt` | https://github.com/assimp/assimp/blob/master/LICENSE |
| `include/enet/` | ENet | `enet_LICENSE.txt` | https://github.com/lsalzman/enet/blob/master/LICENSE |
| `include/freetype/` and `include/ft2build.h` | FreeType | `freetype_FTL.TXT`, `freetype_GPLv2.TXT` | https://freetype.org/license.html |
| `include/glad/` | glad | `glad_LICENSE.txt` | https://github.com/Dav1dde/glad/blob/master/LICENSE |
| `include/GLFW/` | GLFW | `glfw_LICENSE.txt` | https://github.com/glfw/glfw/blob/master/LICENSE.md |
| `include/glm/` | GLM | `glm_copying.txt` | https://github.com/g-truc/glm/blob/master/copying.txt |
| `include/imgui/` | Dear ImGui | `imgui_LICENSE.txt` | https://github.com/ocornut/imgui/blob/master/LICENSE.txt |
| `include/KHR/` | Khronos platform headers | `KHR_khrplatform_LICENSE.txt` | https://github.com/KhronosGroup/EGL-Registry/blob/main/api/KHR/khrplatform.h |
| `include/miniaudio/` | miniaudio | `miniaudio_LICENSE.txt` | https://github.com/mackron/miniaudio/blob/master/LICENSE |
| `include/nlohmann/` | nlohmann/json | `nlohmann_json_LICENSE.MIT` | https://github.com/nlohmann/json/blob/develop/LICENSE.MIT |
| `include/stb_image.h` and `include/stb_image.cpp` | stb | `stb_LICENSE.txt` | https://github.com/nothings/stb/blob/master/LICENSE |

## Notes

- `CosmicEngine/` is the project codebase, not a third-party dependency.
- `KHR/` is tracked separately because `khrplatform.h` carries its own permissive notice in the header used by the project.
- `glad_LICENSE.txt` already includes additional notices related to Khronos specifications used by glad.
- FreeType is distributed with the FreeType License and GPLv2 texts upstream; both copies are included here.
- `miniaudio/` in this repository identifies itself as `v0.11.24 - 2026-01-17` in `miniaudio.h`; the local license copy was pulled from the official upstream repository linked above.