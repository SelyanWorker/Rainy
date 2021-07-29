#pragma once

#include <cstdint>

namespace Rainy
{
    enum RN_KEY_STATE
    {
        RELEASE = 0,
        PRESS = 1
    };

    enum RN_KEY
    {
        RN_KEY_SPACE = 32,
        RN_KEY_APOSTROPHE = 39, /* ' */
        RN_KEY_COMMA = 44,      /* , */
        RN_KEY_MINUS = 45,      /* - */
        RN_KEY_PERIOD = 46,     /* . */
        RN_KEY_SLASH = 47,      /* / */
        RN_KEY_0 = 48,
        RN_KEY_1 = 49,
        RN_KEY_2 = 50,
        RN_KEY_3 = 51,
        RN_KEY_4 = 52,
        RN_KEY_5 = 53,
        RN_KEY_6 = 54,
        RN_KEY_7 = 55,
        RN_KEY_8 = 56,
        RN_KEY_9 = 57,
        RN_KEY_SEMICOLON = 59, /* ; */
        RN_KEY_EQUAL = 61,     /* = */
        RN_KEY_A = 65,
        RN_KEY_B = 66,
        RN_KEY_C = 67,
        RN_KEY_D = 68,
        RN_KEY_E = 69,
        RN_KEY_F = 70,
        RN_KEY_G = 71,
        RN_KEY_H = 72,
        RN_KEY_I = 73,
        RN_KEY_J = 74,
        RN_KEY_K = 75,
        RN_KEY_L = 76,
        RN_KEY_M = 77,
        RN_KEY_N = 78,
        RN_KEY_O = 79,
        RN_KEY_P = 80,
        RN_KEY_Q = 81,
        RN_KEY_R = 82,
        RN_KEY_S = 83,
        RN_KEY_T = 84,
        RN_KEY_U = 85,
        RN_KEY_V = 86,
        RN_KEY_W = 87,
        RN_KEY_X = 88,
        RN_KEY_Y = 89,
        RN_KEY_Z = 90,
        RN_KEY_LEFT_BRACKET = 91,  /* [ */
        RN_KEY_BACKSLASH = 92,     /* \ */
        RN_KEY_RIGHT_BRACKET = 93, /* ] */
        RN_KEY_GRAVE_ACCENT = 96,  /* ` */

        ///* Function Keys */
        RN_KEY_ESCAPE = 256,
        RN_KEY_ENTER = 257,
        RN_KEY_TAB = 258,
        RN_KEY_BACKSPACE = 259,
        RN_KEY_INSERT = 260,
        RN_KEY_DELETE = 261,
        RN_KEY_RIGHT = 262,
        RN_KEY_LEFT = 263,
        RN_KEY_DOWN = 264,
        RN_KEY_UP = 265,
        RN_KEY_PAGE_UP = 266,
        RN_KEY_PAGE_DOWN = 267,
        RN_KEY_HOME = 268,
        RN_KEY_END = 269,
        RN_KEY_CAPS_LOCK = 280,
        RN_KEY_SCROLL_LOCK = 281,
        RN_KEY_NUM_LOCK = 282,
        RN_KEY_PRINT_SCREEN = 283,
        RN_KEY_PAUSE = 284,
        RN_KEY_F1 = 290,
        RN_KEY_F2 = 291,
        RN_KEY_F3 = 292,
        RN_KEY_F4 = 293,
        RN_KEY_F5 = 294,
        RN_KEY_F6 = 295,
        RN_KEY_F7 = 296,
        RN_KEY_F8 = 297,
        RN_KEY_F9 = 298,
        RN_KEY_F10 = 299,
        RN_KEY_F11 = 300,
        RN_KEY_F12 = 301,
        RN_KEY_F13 = 302,
        RN_KEY_F14 = 303,
        RN_KEY_F15 = 304,
        RN_KEY_F16 = 305,
        RN_KEY_F17 = 306,
        RN_KEY_F18 = 307,
        RN_KEY_F19 = 308,
        RN_KEY_F20 = 309,
        RN_KEY_F21 = 310,
        RN_KEY_F22 = 311,
        RN_KEY_F23 = 312,
        RN_KEY_F24 = 313,
        RN_KEY_F25 = 314,
        RN_KEY_KP_0 = 320,
        RN_KEY_KP_1 = 321,
        RN_KEY_KP_2 = 322,
        RN_KEY_KP_3 = 323,
        RN_KEY_KP_4 = 324,
        RN_KEY_KP_5 = 325,
        RN_KEY_KP_6 = 326,
        RN_KEY_KP_7 = 327,
        RN_KEY_KP_8 = 328,
        RN_KEY_KP_9 = 329,
        RN_KEY_KP_DECIMAL = 330,
        RN_KEY_KP_DIVIDE = 331,
        RN_KEY_KP_MULTIPLY = 332,
        RN_KEY_KP_SUBTRACT = 333,
        RN_KEY_KP_ADD = 334,
        RN_KEY_KP_ENTER = 335,
        RN_KEY_KP_EQUAL = 336,
        RN_KEY_LEFT_SHIFT = 340,
        RN_KEY_LEFT_CONTROL = 341,
        RN_KEY_LEFT_ALT = 342,
        RN_KEY_LEFT_SUPER = 343,
        RN_KEY_RIGHT_SHIFT = 344,
        RN_KEY_RIGHT_CONTROL = 345,
        RN_KEY_RIGHT_ALT = 346,
        RN_KEY_RIGHT_SUPER = 347,
        RN_KEY_MENU = 348,

        RN_KEY_LAST = RN_KEY_MENU,

        RN_MOD_SHIFT = 0x0001,
        RN_MOD_CONTROL = 0x0002,
        RN_MOD_ALT = 0x0004,
        RN_MOD_SUPER = 0x0008,
        RN_MOD_CAPS_LOCK = 0x0010,

        RN_MOD_NUM_LOCK = 0x0020   // dsada
    };

    enum RN_MOUSE
    {
        BUTTON_1 = 0,
        BUTTON_2 = 1,
        BUTTON_3 = 2,
        BUTTON_4 = 3,
        BUTTON_5 = 4,
        BUTTON_6 = 5,
        BUTTON_7 = 6,
        BUTTON_8 = 7,
        BUTTON_LAST = BUTTON_8,
        BUTTON_LEFT = BUTTON_1,
        BUTTON_RIGHT = BUTTON_2,
        BUTTON_MIDDLE = BUTTON_3
    };

    extern uint8_t KeyStates[512];
}