#pragma once
#include <raylib.h>

// How many screen pixels one pixel of art covers. Backgrounds, characters and small icons
// share one scale so they look like parts of the same picture. Boxes and buttons are drawn
// finer so they do not look blocky.
inline constexpr float worldScale = 6.0f;
inline constexpr float iconScale = 6.0f;
inline constexpr float uiScale = 4.0f;
inline constexpr float detailScale = 3.0f;
// A hero shown alone on a screen (character select, rest) is drawn larger, as a portrait.
inline constexpr float portraitScale = 12.0f;

// Text in the main font (Jersey 10) stays sharp at multiples of 20.
inline constexpr float smallText = 40.0f;
inline constexpr float bigText = 60.0f;
inline constexpr float titleText = 80.0f;

// Headings use the title font, which stays sharp at multiples of 13.
inline constexpr float headingText = 65.0f;
inline constexpr float bannerText = 130.0f;
inline constexpr float logoText = 234.0f;

// The fonts sit a little low in their line box; drawing this much higher centers them.
inline constexpr float smallTextNudge = 4.0f;
inline constexpr float bigTextNudge = 6.0f;
inline constexpr float titleTextNudge = 8.0f;

inline constexpr float buttonHeight = 80.0f;
inline constexpr float headerY = 130.0f;

inline constexpr Color inkColor{61, 41, 54, 255};
inline constexpr Color shadeColor{30, 20, 40, 255};
inline constexpr Color textColor{245, 238, 220, 255};
inline constexpr Color peachColor{255, 194, 161, 255};
inline constexpr Color goldColor{255, 238, 131, 255};
inline constexpr Color redColor{230, 69, 57, 255};
inline constexpr Color tealColor{146, 232, 192, 255};
inline constexpr Color debuffColor{196, 140, 235, 255};
inline constexpr Color outlineColor{245, 238, 220, 220};
inline constexpr Color damageOutline{230, 69, 57, 255};
inline constexpr Color blockOutline{79, 164, 220, 255};
inline constexpr Color panelTint{120, 114, 138, 255};
inline constexpr Color unplayableTint{120, 112, 128, 255};
