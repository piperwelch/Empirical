/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2015-2018
 *
 *  @file  Canvas.hpp
 *  @brief Manage an HTML canvas object.
 *
 */


#ifndef EMP_WEB_CANVAS_HPP
#define EMP_WEB_CANVAS_HPP

#ifdef __EMSCRIPTEN__

#include "_EmscriptenCanvas.hpp"

// https://stackoverflow.com/questions/4102351/test-for-empty-macro-definition/4127073#4127073
#elif __has_include(<SFML/Graphics.hpp>) && (!defined(EMP_HAS_X11) || (EMP_HAS_X11 == 1)) 

#define EMP_NATIVE_SUPPORT
#include "_NativeCanvas.hpp"

#else
static_assert(false, "Neither Emscripten nor SFML exist");

#endif // #ifdef __EMSCRIPTEN__

#endif // #ifndef EMP_WEB_CANVAS_HPP
