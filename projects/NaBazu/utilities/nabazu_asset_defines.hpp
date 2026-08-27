#ifndef NABAZU_ASSET_DEFINES_HPP
#define NABAZU_ASSET_DEFINES_HPP

// Rutas y keys centralizadas de assets. NOTA: RS_MN.RenderParallelepipedTextureLit/
// Unlit ignoran el parametro `color` (baseColor) siempre que haya una textura ligada
// -- el shader COSMIC_3DModel_Unlit usa `hasDiffuseTexture ? texture(...) : baseColor`,
// asi que "tenir" una unica textura compartida por entidad no funciona. En su lugar,
// cada tipo de entidad usa su propia textura placeholder solida (32x32) hasta que
// existan modelos 3D reales.

#define NABAZU_TEXTURE_PLACEHOLDER_KEY   "nabazu.texture.placeholder"
#define NABAZU_TEXTURE_PLACEHOLDER_PATH  "assets/textures/placeholder.png"

#define NABAZU_TEXTURE_SHIP_KEY    "nabazu.texture.ship"
#define NABAZU_TEXTURE_SHIP_PATH   "assets/textures/ship_cyan.png"

#define NABAZU_TEXTURE_ENEMY_KEY   "nabazu.texture.enemy"
#define NABAZU_TEXTURE_ENEMY_PATH  "assets/textures/enemy_red.png"

#define NABAZU_TEXTURE_BULLET_KEY   "nabazu.texture.bullet"
#define NABAZU_TEXTURE_BULLET_PATH  "assets/textures/bullet_yellow.png"

#define NABAZU_TEXTURE_MISSILE_KEY   "nabazu.texture.missile"
#define NABAZU_TEXTURE_MISSILE_PATH  "assets/textures/missile_orange.png"

#define NABAZU_TEXTURE_PICKUP_AMMO_KEY    "nabazu.texture.pickup_ammo"
#define NABAZU_TEXTURE_PICKUP_AMMO_PATH   "assets/textures/pickup_ammo_green.png"

#define NABAZU_TEXTURE_PICKUP_MISSILE_KEY   "nabazu.texture.pickup_missile"
#define NABAZU_TEXTURE_PICKUP_MISSILE_PATH  "assets/textures/pickup_missile_purple.png"

#define NABAZU_TEXTURE_EXPLOSION_KEY   "nabazu.texture.explosion"
#define NABAZU_TEXTURE_EXPLOSION_PATH  "assets/textures/explosion_orange.png"

#define NABAZU_TEXTURE_RETICLE_KEY    "nabazu.texture.reticle"
#define NABAZU_TEXTURE_RETICLE_PATH   "assets/textures/reticle_yellow.png"

#define NABAZU_TEXTURE_ENEMY_BULLET_KEY   "nabazu.texture.enemy_bullet"
#define NABAZU_TEXTURE_ENEMY_BULLET_PATH  "assets/textures/enemy_bullet_magenta.png"

#define NABAZU_FONT_MAIN_KEY   "nabazu.font.main"
#define NABAZU_FONT_MAIN_PATH  "assets/fonts/ThaleahFat.ttf"

#define NABAZU_MUSIC_GAMEPLAY_KEY   "nabazu.music.gameplay"
#define NABAZU_MUSIC_GAMEPLAY_PATH  "assets/music/gameplay.mp3"

// --- SFX (assets/audio/*.wav) --------------------------------------------------
// Played through AUD_MN.PlayAt(..., SoundSpace::World3D, ...) so they pan and
// attenuate around the listener, which GameplayScene keeps pinned to the camera.

#define NABAZU_SFX_SHOOT_KEY    "nabazu.sfx.shoot"
#define NABAZU_SFX_SHOOT_PATH   "assets/audio/shoot.wav"

#define NABAZU_SFX_BOMB_KEY     "nabazu.sfx.bomb"
#define NABAZU_SFX_BOMB_PATH    "assets/audio/shoot_bomb.wav"

#define NABAZU_SFX_BOMB_DESTROY_KEY   "nabazu.sfx.bomb_destroy"
#define NABAZU_SFX_BOMB_DESTROY_PATH  "assets/audio/bomb_destroy.wav"

#define NABAZU_SFX_ENEMY_DESTROY_KEY  "nabazu.sfx.enemy_destroy"
#define NABAZU_SFX_ENEMY_DESTROY_PATH "assets/audio/enemy_destroy.wav"

#define NABAZU_SFX_PLAYER_DESTROY_KEY  "nabazu.sfx.player_destroy"
#define NABAZU_SFX_PLAYER_DESTROY_PATH "assets/audio/player_destroy.wav"

#define NABAZU_SFX_PICKUP_KEY   "nabazu.sfx.pickup_item"
#define NABAZU_SFX_PICKUP_PATH  "assets/audio/pickup_item.wav"

#endif
