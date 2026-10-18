#include "ultra64.h"
#include "printf.h"
#include "stack_pad.h"
#include "translation.h"
#include "play_state.h"
#include "player.h"
#include "save.h"

#include "assets/textures/icon_item_static/icon_item_static.h"
#include "assets/textures/icon_item_24_static/icon_item_24_static.h"
#include "assets/textures/parameter_static/parameter_static.h"

// Bit Flag array in which gBitFlags[n] is literally (1 << n)
u32 gBitFlags[] = {
    (1 << 0),  (1 << 1),  (1 << 2),  (1 << 3),  (1 << 4),  (1 << 5),  (1 << 6),  (1 << 7),
    (1 << 8),  (1 << 9),  (1 << 10), (1 << 11), (1 << 12), (1 << 13), (1 << 14), (1 << 15),
    (1 << 16), (1 << 17), (1 << 18), (1 << 19), (1 << 20), (1 << 21), (1 << 22), (1 << 23),
    (1 << 24), (1 << 25), (1 << 26), (1 << 27), (1 << 28), (1 << 29), (1 << 30), (1 << 31),
};

u16 gEquipMasks[EQUIP_TYPE_MAX] = {
    0xF << (EQUIP_TYPE_SWORD * 4),  // EQUIP_TYPE_SWORD
    0xF << (EQUIP_TYPE_SHIELD * 4), // EQUIP_TYPE_SHIELD
    0xF << (EQUIP_TYPE_TUNIC * 4),  // EQUIP_TYPE_TUNIC
    0xF << (EQUIP_TYPE_BOOTS * 4),  // EQUIP_TYPE_BOOTS
};
u16 gEquipNegMasks[EQUIP_TYPE_MAX] = {
    (u16) ~(0xF << (EQUIP_TYPE_SWORD * 4)),  // EQUIP_TYPE_SWORD
    (u16) ~(0xF << (EQUIP_TYPE_SHIELD * 4)), // EQUIP_TYPE_SHIELD
    (u16) ~(0xF << (EQUIP_TYPE_TUNIC * 4)),  // EQUIP_TYPE_TUNIC
    (u16) ~(0xF << (EQUIP_TYPE_BOOTS * 4)),  // EQUIP_TYPE_BOOTS
};

u32 gUpgradeMasks[UPG_MAX] = {
    0x00000007, // UPG_QUIVER
    0x00000038, // UPG_BOMB_BAG
    0x000001C0, // UPG_STRENGTH
    0x00000E00, // UPG_SCALE
    0x00003000, // UPG_WALLET
    0x0001C000, // UPG_BULLET_BAG
    0x000E0000, // UPG_DEKU_STICKS
    0x00700000, // UPG_DEKU_NUTS
};
u32 gUpgradeNegMasks[UPG_MAX] = {
    ~0x00000007, // UPG_QUIVER
    ~0x00000038, // UPG_BOMB_BAG
    ~0x000001C0, // UPG_STRENGTH
    ~0x00000E00, // UPG_SCALE
    ~0x00003000, // UPG_WALLET
    ~0x0001C000, // UPG_BULLET_BAG
    ~0x000E0000, // UPG_DEKU_STICKS
    ~0x00700000, // UPG_DEKU_NUTS
};

u8 gEquipShifts[EQUIP_TYPE_MAX] = {
    EQUIP_TYPE_SWORD * 4,  // EQUIP_TYPE_SWORD
    EQUIP_TYPE_SHIELD * 4, // EQUIP_TYPE_SHIELD
    EQUIP_TYPE_TUNIC * 4,  // EQUIP_TYPE_TUNIC
    EQUIP_TYPE_BOOTS * 4,  // EQUIP_TYPE_BOOTS
};

u8 gUpgradeShifts[UPG_MAX] = {
    0,  // UPG_QUIVER
    3,  // UPG_BOMB_BAG
    6,  // UPG_STRENGTH
    9,  // UPG_SCALE
    12, // UPG_WALLET
    14, // UPG_BULLET_BAG
    17, // UPG_DEKU_STICKS
    20, // UPG_DEKU_NUTS
};

u16 gUpgradeCapacities[UPG_MAX][4] = {
    { 0, 30, 40, 50 },     // UPG_QUIVER
    { 0, 20, 30, 40 },     // UPG_BOMB_BAG
    { 0, 0, 0, 0 },        // UPG_STRENGTH (unused)
    { 0, 0, 0, 0 },        // UPG_SCALE (unused)
    { 99, 200, 500, 500 }, // UPG_WALLET
    { 0, 30, 40, 50 },     // UPG_BULLET_BAG
    { 0, 10, 20, 30 },     // UPG_DEKU_STICKS
    { 0, 20, 30, 40 },     // UPG_DEKU_NUTS
};

u32 gGsFlagsMasks[] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
u32 gGsFlagsShifts[] = { 0, 8, 16, 24 };

void* gItemIcons[] = {
    // icon_item_static 32x32 rgba32
    gItemIconDekuStickTex,         // ITEM_DEKU_STICK
    gItemIconDekuNutTex,           // ITEM_DEKU_NUT
    gItemIconBombTex,              // ITEM_BOMB
    gItemIconBowTex,               // ITEM_BOW
    gItemIconArrowFireTex,         // ITEM_ARROW_FIRE
    gItemIconDinsFireTex,          // ITEM_DINS_FIRE
    gItemIconSlingshotTex,         // ITEM_SLINGSHOT
    gItemIconOcarinaFairyTex,      // ITEM_OCARINA_FAIRY
    gItemIconOcarinaOfTimeTex,     // ITEM_OCARINA_OF_TIME
    gItemIconBombchuTex,           // ITEM_BOMBCHU
    gItemIconHookshotTex,          // ITEM_HOOKSHOT
    gItemIconLongshotTex,          // ITEM_LONGSHOT
    gItemIconArrowIceTex,          // ITEM_ARROW_ICE
    gItemIconFaroresWindTex,       // ITEM_FARORES_WIND
    gItemIconBoomerangTex,         // ITEM_BOOMERANG
    gItemIconLensOfTruthTex,       // ITEM_LENS_OF_TRUTH
    gItemIconMagicBeanTex,         // ITEM_MAGIC_BEAN
    gItemIconHammerTex,            // ITEM_HAMMER
    gItemIconArrowLightTex,        // ITEM_ARROW_LIGHT
    gItemIconNayrusLoveTex,        // ITEM_NAYRUS_LOVE
    gItemIconBottleEmptyTex,       // ITEM_BOTTLE_EMPTY
    gItemIconBottlePotionRedTex,   // ITEM_BOTTLE_POTION_RED
    gItemIconBottlePotionGreenTex, // ITEM_BOTTLE_POTION_GREEN
    gItemIconBottlePotionBlueTex,  // ITEM_BOTTLE_POTION_BLUE
    gItemIconBottleFairyTex,       // ITEM_BOTTLE_FAIRY
    gItemIconBottleFishTex,        // ITEM_BOTTLE_FISH
    gItemIconBottleMilkFullTex,    // ITEM_BOTTLE_MILK_FULL
    gItemIconBottleRutosLetterTex, // ITEM_BOTTLE_RUTOS_LETTER
    gItemIconBottleBlueFireTex,    // ITEM_BOTTLE_BLUE_FIRE
    gItemIconBottleBugTex,         // ITEM_BOTTLE_BUG
    gItemIconBottleBigPoeTex,      // ITEM_BOTTLE_BIG_POE
    gItemIconBottleMilkHalfTex,    // ITEM_BOTTLE_MILK_HALF
    gItemIconBottlePoeTex,         // ITEM_BOTTLE_POE
    gItemIconWeirdEggTex,          // ITEM_WEIRD_EGG
    gItemIconChickenTex,           // ITEM_CHICKEN
    gItemIconZeldasLetterTex,      // ITEM_ZELDAS_LETTER
    gItemIconMaskKeatonTex,        // ITEM_MASK_KEATON
    gItemIconMaskSkullTex,         // ITEM_MASK_SKULL
    gItemIconMaskSpookyTex,        // ITEM_MASK_SPOOKY
    gItemIconMaskBunnyHoodTex,     // ITEM_MASK_BUNNY_HOOD
    gItemIconMaskGoronTex,         // ITEM_MASK_GORON
    gItemIconMaskZoraTex,          // ITEM_MASK_ZORA
    gItemIconMaskGerudoTex,        // ITEM_MASK_GERUDO
    gItemIconMaskTruthTex,         // ITEM_MASK_TRUTH
    gItemIconSoldOutTex,           // ITEM_SOLD_OUT
    gItemIconPocketEggTex,         // ITEM_POCKET_EGG
    gItemIconPocketCuccoTex,       // ITEM_POCKET_CUCCO
    gItemIconCojiroTex,            // ITEM_COJIRO
    gItemIconOddMushroomTex,       // ITEM_ODD_MUSHROOM
    gItemIconOddPotionTex,         // ITEM_ODD_POTION
    gItemIconPoachersSawTex,       // ITEM_POACHERS_SAW
    gItemIconBrokenGoronsSwordTex, // ITEM_BROKEN_GORONS_SWORD
    gItemIconPrescriptionTex,      // ITEM_PRESCRIPTION
    gItemIconEyeballFrogTex,       // ITEM_EYEBALL_FROG
    gItemIconEyeDropsTex,          // ITEM_EYE_DROPS
    gItemIconClaimCheckTex,        // ITEM_CLAIM_CHECK
    gItemIconBowFireTex,           // ITEM_BOW_FIRE
    gItemIconBowIceTex,            // ITEM_BOW_ICE
    gItemIconBowLightTex,          // ITEM_BOW_LIGHT
    gItemIconSwordKokiriTex,       // ITEM_SWORD_KOKIRI
    gItemIconSwordMasterTex,       // ITEM_SWORD_MASTER
    gItemIconSwordBiggoronTex,     // ITEM_SWORD_BIGGORON
    gItemIconShieldDekuTex,        // ITEM_SHIELD_DEKU
    gItemIconShieldHylianTex,      // ITEM_SHIELD_HYLIAN
    gItemIconShieldMirrorTex,      // ITEM_SHIELD_MIRROR
    gItemIconTunicKokiriTex,       // ITEM_TUNIC_KOKIRI
    gItemIconTunicGoronTex,        // ITEM_TUNIC_GORON
    gItemIconTunicZoraTex,         // ITEM_TUNIC_ZORA
    gItemIconBootsKokiriTex,       // ITEM_BOOTS_KOKIRI
    gItemIconBootsIronTex,         // ITEM_BOOTS_IRON
    gItemIconBootsHoverTex,        // ITEM_BOOTS_HOVER
    gItemIconBulletBag30Tex,       // ITEM_BULLET_BAG_30
    gItemIconBulletBag40Tex,       // ITEM_BULLET_BAG_40
    gItemIconBulletBag50Tex,       // ITEM_BULLET_BAG_50
    gItemIconQuiver30Tex,          // ITEM_QUIVER_30
    gItemIconQuiver40Tex,          // ITEM_QUIVER_40
    gItemIconQuiver50Tex,          // ITEM_QUIVER_50
    gItemIconBombBag20Tex,         // ITEM_BOMB_BAG_20
    gItemIconBombBag30Tex,         // ITEM_BOMB_BAG_30
    gItemIconBombBag40Tex,         // ITEM_BOMB_BAG_40
    gItemIconGoronsBraceletTex,    // ITEM_STRENGTH_GORONS_BRACELET
    gItemIconSilverGauntletsTex,   // ITEM_STRENGTH_SILVER_GAUNTLETS
    gItemIconGoldenGauntletsTex,   // ITEM_STRENGTH_GOLD_GAUNTLETS
    gItemIconScaleSilverTex,       // ITEM_SCALE_SILVER
    gItemIconScaleGoldenTex,       // ITEM_SCALE_GOLDEN
    gItemIconBrokenGiantsKnifeTex, // ITEM_GIANTS_KNIFE
    gItemIconAdultsWalletTex,      // ITEM_ADULTS_WALLET
    gItemIconGiantsWalletTex,      // ITEM_GIANTS_WALLET
    gItemIconDekuSeedsTex,         // ITEM_DEKU_SEEDS
    gItemIconFishingPoleTex,       // ITEM_FISHING_POLE
    // icon_item_static 16x24 ia8
    gSongNoteTex, // ITEM_SONG_MINUET
    gSongNoteTex, // ITEM_SONG_BOLERO
    gSongNoteTex, // ITEM_SONG_SERENADE
    gSongNoteTex, // ITEM_SONG_REQUIEM
    gSongNoteTex, // ITEM_SONG_NOCTURNE
    gSongNoteTex, // ITEM_SONG_PRELUDE
    gSongNoteTex, // ITEM_SONG_LULLABY
    gSongNoteTex, // ITEM_SONG_EPONA
    gSongNoteTex, // ITEM_SONG_SARIA
    gSongNoteTex, // ITEM_SONG_SUN
    gSongNoteTex, // ITEM_SONG_TIME
    gSongNoteTex, // ITEM_SONG_STORMS
    // icon_item_24static 24x24 rgba32
    gQuestIconMedallionForestTex, // ITEM_MEDALLION_FOREST
    gQuestIconMedallionFireTex,   // ITEM_MEDALLION_FIRE
    gQuestIconMedallionWaterTex,  // ITEM_MEDALLION_WATER
    gQuestIconMedallionSpiritTex, // ITEM_MEDALLION_SPIRIT
    gQuestIconMedallionShadowTex, // ITEM_MEDALLION_SHADOW
    gQuestIconMedallionLightTex,  // ITEM_MEDALLION_LIGHT
    gQuestIconKokiriEmeraldTex,   // ITEM_KOKIRI_EMERALD
    gQuestIconGoronRubyTex,       // ITEM_GORON_RUBY
    gQuestIconZoraSapphireTex,    // ITEM_ZORA_SAPPHIRE
    gQuestIconStoneOfAgonyTex,    // ITEM_STONE_OF_AGONY
    gQuestIconGerudosCardTex,     // ITEM_GERUDOS_CARD
    gQuestIconGoldSkulltulaTex,   // ITEM_SKULL_TOKEN
    gQuestIconHeartContainerTex,  // ITEM_HEART_CONTAINER
    gQuestIconHeartPieceTex,      // ITEM_HEART_PIECE
    gQuestIconDungeonBossKeyTex,  // ITEM_DUNGEON_BOSS_KEY
    gQuestIconDungeonMapTex,      // ITEM_DUNGEON_COMPASS (swapped with map (inconsequential))
    gQuestIconDungeonCompassTex,  // ITEM_DUNGEON_MAP (swapped with compass (inconsequential))
    gQuestIconDungeonBossKeyTex,  // ITEM_SMALL_KEY (but texture is boss key (inconsequential))
    gQuestIconMagicJarSmallTex,   // ITEM_MAGIC_JAR_SMALL
    gQuestIconMagicJarBigTex,     // ITEM_MAGIC_JAR_BIG
    // icon_item_static 48x48 ia8
    gHeartPieceIcon1Tex, // ITEM_HEART_PIECE_2
    gHeartPieceIcon2Tex, // ITEM_INVALID_1
    gHeartPieceIcon3Tex, // ITEM_INVALID_2
    // parameter_static 16x16 ia8
    gOcarinaBtnIconCUpTex,    // ITEM_INVALID_3
    gOcarinaBtnIconCDownTex,  // ITEM_INVALID_4
    gOcarinaBtnIconCLeftTex,  // ITEM_INVALID_5
    gOcarinaBtnIconCRightTex, // ITEM_INVALID_6
    gOcarinaBtnIconATex,      // ITEM_INVALID_7
    // START: MODIFIED
    gPlaceholderTex1,
    gPlaceholderTex2,
    gPlaceholderTex3,
    gPlaceholderTex4,
    gPlaceholderTex5,
    gPlaceholderTex6,
    gPlaceholderTex7,
    gPlaceholderTex8,
    gPlaceholderTex9,
    gPlaceholderTex10,
    gPlaceholderTex11,
    gPlaceholderTex12,
    gPlaceholderTex13,
    gPlaceholderTex14,
    gPlaceholderTex15,
    gPlaceholderTex16,
    gPlaceholderTex17,
    gPlaceholderTex18,
    gPlaceholderTex19,
    gPlaceholderTex20,
    gPlaceholderTex21,
    gPlaceholderTex22,
    gPlaceholderTex23,
    gPlaceholderTex24,
    gPlaceholderTex25,
    gPlaceholderTex26,
    gItemIconCustomItemTex,
    // END: MODIFIED; Added padding so that the texture shows on the item
};

// Used to map item IDs to inventory slots
u8 gItemSlots[] = {

    // START: MODIFIED
    SLOT_DEKU_STICK,    // ITEM_DEKU_STICK          0x00
    SLOT_DEKU_NUT,      // ITEM_DEKU_NUT            0x01
    SLOT_BOMB,          // ITEM_BOMB                0x02
    SLOT_BOW,           // ITEM_BOW                 0x03
    SLOT_ARROW_FIRE,    // ITEM_ARROW_FIRE          0x04
    SLOT_DINS_FIRE,     // ITEM_DINS_FIRE           0x05
    SLOT_SLINGSHOT,     // ITEM_SLINGSHOT           0x06
    SLOT_OCARINA,       // ITEM_OCARINA_FAIRY       0x07
    SLOT_OCARINA,       // ITEM_OCARINA_OF_TIME     0x07
    SLOT_BOMBCHU,       // ITEM_BOMBCHU             0x08
    SLOT_HOOKSHOT,      // ITEM_HOOKSHOT            0x09
    SLOT_HOOKSHOT,      // ITEM_LONGSHOT            0x09
    SLOT_ARROW_ICE,     // ITEM_ARROW_ICE           0x0A
    SLOT_FARORES_WIND,  // ITEM_FARORES_WIND        0x0B
    SLOT_BOOMERANG,     // ITEM_BOOMERANG           0x0C
    SLOT_LENS_OF_TRUTH, // ITEM_LENS_OF_TRUTH       0x0C
    SLOT_MAGIC_BEAN,    // ITEM_MAGIC_BEAN          0x0E
    SLOT_HAMMER,        // ITEM_HAMMER              0x0F
    SLOT_ARROW_LIGHT,   // ITEM_ARROW_LIGHT         0x10
    SLOT_NAYRUS_LOVE,   // ITEM_NAYRUS_LOVE         0x11
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_EMPTY        0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_POTION_RED   0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_POTION_GREEN 0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_POTION_BLUE  0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_FAIRY        0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_FISH         0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_MILK_FULL    0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_RUTOS_LETTER 0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_BLUE_FIRE    0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_BUG          0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_BIG_POE      0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_MILK_HALF    0x12
    SLOT_BOTTLE_1,      // ITEM_BOTTLE_POE          0x12
    SLOT_TRADE_CHILD,   // ITEM_WEIRD_EGG           0x13
    SLOT_TRADE_CHILD,   // ITEM_CHICKEN             0x13
    SLOT_TRADE_CHILD,   // ITEM_ZELDAS_LETTER       0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_KEATON         0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_SKULL          0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_SPOOKY         0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_BUNNY_HOOD     0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_GORON          0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_ZORA           0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_GERUDO         0x13
    SLOT_TRADE_CHILD,   // ITEM_MASK_TRUTH          0x13
    SLOT_TRADE_CHILD,   // ITEM_SOLD_OUT            0x13
    SLOT_TRADE_ADULT,   // ITEM_POCKET_EGG          0x14
    SLOT_TRADE_ADULT,   // ITEM_POCKET_CUCCO        0x14
    SLOT_TRADE_ADULT,   // ITEM_COJIRO              0x14
    SLOT_TRADE_ADULT,   // ITEM_ODD_MUSHROOM        0x14
    SLOT_TRADE_ADULT,   // ITEM_ODD_POTION          0x14
    SLOT_TRADE_ADULT,   // ITEM_POACHERS_SAW        0x14
    SLOT_TRADE_ADULT,   // ITEM_BROKEN_GORONS_SWORD 0x14
    SLOT_TRADE_ADULT,   // ITEM_PRESCRIPTION        0x14
    SLOT_TRADE_ADULT,   // ITEM_EYEBALL_FROG        0x14
    SLOT_TRADE_ADULT,   // ITEM_EYE_DROPS           0x14
    SLOT_TRADE_ADULT,   // ITEM_CLAIM_CHECK         0x14
    SLOT_TRADE_ADULT,   // ITEM_CUSTOM_ITEM         0x14

    // END: MODIFIED; Added hex values and support for CUSTOM_ITEM
};

void Inventory_ChangeEquipment(s16 equipment, u16 value) {
    gSaveContext.save.info.equips.equipment &= gEquipNegMasks[equipment];
    gSaveContext.save.info.equips.equipment |= value << gEquipShifts[equipment];
}

u8 Inventory_DeleteEquipment(PlayState* play, s16 equipment) {
    Player* player = GET_PLAYER(play);
    STACK_PAD(s32);
    u16 equipValue = gSaveContext.save.info.equips.equipment & gEquipMasks[equipment];

    PRINTF(T("装備アイテム抹消 = %d  zzz=%d\n", "Erasing equipment item = %d  zzz=%d\n"), equipment, equipValue);

    if (equipValue) {
        equipValue >>= gEquipShifts[equipment];

        gSaveContext.save.info.equips.equipment &= gEquipNegMasks[equipment];
        gSaveContext.save.info.inventory.equipment ^= OWNED_EQUIP_FLAG(equipment, equipValue - 1);

        if (equipment == EQUIP_TYPE_TUNIC) {
            gSaveContext.save.info.equips.equipment |= EQUIP_VALUE_TUNIC_KOKIRI << (EQUIP_TYPE_TUNIC * 4);
        }

        if (equipment == EQUIP_TYPE_SWORD) {
            gSaveContext.save.info.equips.buttonItems[0] = ITEM_NONE;
            gSaveContext.save.info.infTable[INFTABLE_INDEX_1DX] = 1;
        }

        Player_SetEquipmentData(play, player);
        play->pauseCtx.cursorSpecialPos = PAUSE_CURSOR_PAGE_LEFT;
    }

    return equipValue;
}

void Inventory_ChangeUpgrade(s16 upgrade, s16 value) {
    gSaveContext.save.info.inventory.upgrades &= gUpgradeNegMasks[upgrade];
    gSaveContext.save.info.inventory.upgrades |= value << gUpgradeShifts[upgrade];
}

/*if (itemId == ITEM_CUSTOM_ITEM) {
    return gItemIconCustomItemTex;
}
*/
