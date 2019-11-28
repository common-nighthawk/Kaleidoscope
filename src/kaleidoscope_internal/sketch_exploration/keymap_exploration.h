/* Kaleidoscope - Firmware for computer input devices
 * Copyright (C) 2013-2019  Keyboard.io, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "kaleidoscope/key_defs.h"

namespace kaleidoscope {
namespace sketch_exploration {

// A simple keymap adaptor class that makes the keymap conveniently accessible.
// at compiletime.
//
template<uint8_t _n_layers, uint8_t _layer_size>
class KeymapAdaptor {
 private:

  const Key(&keymap_)[_n_layers][_layer_size];

 public:

  static constexpr uint8_t n_layers = _n_layers;
  static constexpr uint8_t layer_size = _layer_size;

  constexpr KeymapAdaptor(const Key(&keymap)[_n_layers][_layer_size])
    :  keymap_{keymap}
  {}

  constexpr Key getKey(uint8_t layer, uint8_t offset) {
    return keymap_[layer][offset];
  }
  constexpr Key getKey(uint8_t layer, KeyAddr key_addr) {
    return this->getKey(layer, key_addr.toInt());
  }
};

// This class implements compile time keymap traversal.
//
// Every key is visited an the _Accumulation functor decides on how
// the key's value affects the overal return value (accumulation).
//
template<uint8_t _n_layers, uint8_t _layer_size, typename _Accumulation>
class AccumulationHelper : public KeymapAdaptor<_n_layers, _layer_size> {
 private:

  const _Accumulation &op_;

 private:

  typedef typename _Accumulation::ResultType ResultType;

  constexpr ResultType accumulateOnLayer(uint8_t layer, uint8_t offset) {
    return (offset >= _layer_size)
           ?  op_.init_value
           :  op_.apply(this->getKey(layer, offset),
                        this->accumulateOnLayer(layer, offset + 1));
  }

  constexpr ResultType accumulate(uint8_t layer) {
    return (layer >= _n_layers)
           ?   op_.init_value
           :   op_.apply(this->accumulateOnLayer(layer, 0),
                         this->accumulate(layer + 1));
  }

 public:

  typedef KeymapAdaptor<_n_layers, _layer_size> ParentType;

  constexpr AccumulationHelper(const Key(&keymap)[_n_layers][_layer_size],
                               const _Accumulation &op)
    :   ParentType{keymap},
        op_{op}
  {}

  constexpr ResultType apply() {
    return this->accumulate(0);
  }
};

// Accumulation functors to be used with the KeymapInterface's collect
// method.
//
// Plugins can implement their own functors to request other types of
// information

struct MaxKeyRaw {
  typedef Key ResultType;
  static constexpr ResultType init_value = 0;

  constexpr ResultType apply(Key k1, Key k2) {
    return (k1 > k2) ? k1 : k2;
  }
};

struct NumKeysEqual {
  typedef uint8_t ResultType;
  static constexpr ResultType init_value = 0;

  constexpr NumKeysEqual(Key k) : k_{k} {}

  constexpr ResultType apply(Key test_key, ResultType r) {
    return (test_key == k_) ? r + 1 : r;
  }
  constexpr ResultType apply(ResultType r1, ResultType r2) {
    return r1 + r2;
  }

  Key k_;
};

struct HasKey {
  typedef bool ResultType;
  static constexpr ResultType init_value = false;

  constexpr HasKey(Key k) : k_{k} {}

  constexpr ResultType apply(Key test_key, ResultType r) {
    return (test_key == k_) ? true : r;
  }
  constexpr ResultType apply(ResultType r1, ResultType r2) {
    return r1 || r2;
  }

  Key k_;
};

// This class is actually defined and implemented in _INIT_KEYMAP_EXPLORATION
// which is invoked by KALEIDOSCOPE_INIT_PLUGINS
//
class KeymapInterface;

extern void pluginsExploreSketch();

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Read carefully
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// About compile-time-only functions (COMPILE_TIME_USE_ONLY)
//
// On Harvard architecture an array like the Kaleidoscope keymap
// that is declared 'constexpr PROGMEM' must either be accessed
// by a runtime function through functions like pgm_read_byte
// or at compile time with ordinary array access. Using normal array
// access at run-time will cause a memory access failures as the MCU
// will try to read from SRAM instead of PROGMEM.
//
// There are some functions that are therefore restricted to compile-time
// use and will fail if instantiated by the compiler. There is no language
// inherent means to prevent the latter. Therefore, we flag all such functions
// as
//
// COMPILE_TIME_USE_ONLY
//
// Call such functions consistently as
//
// constexpr auto val = aCompileTimeOnlyFunction(...);
//
// This ensures that they can not be instantiated and called at runtime.
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// This macro defines a Sketch interface class that is passed to the
// exploreSketch<_Sketch>(...)-hook.
//
#define _INIT_KEYMAP_EXPLORATION                                               \
  namespace kaleidoscope {                                                     \
  namespace sketch_exploration {                                               \
    class StaticKeymap                                                         \
    {                                                                          \
      private:                                                                 \
                                                                               \
        template<int _n_layers, int _layer_size, typename _Accumulation>       \
        static constexpr auto accumulationHelper(                              \
                  const Key (&keymap)[_n_layers][_layer_size],                 \
                  const _Accumulation &op)                                     \
          -> AccumulationHelper<_n_layers, _layer_size, _Accumulation>         \
        {                                                                      \
          return AccumulationHelper<_n_layers, _layer_size,                    \
                                    _Accumulation>{keymap, op};                \
        }                                                                      \
                                                                               \
        template<typename _Accumulation>                                       \
        static constexpr auto accumulationHelper(const _Accumulation &op)      \
          -> decltype(accumulationHelper(::keymaps_linear, op))                \
        {                                                                      \
          return accumulationHelper(::keymaps_linear, op);                     \
        }                                                                      \
                                                                               \
        template<int _n_layers, int _layer_size>                               \
        static constexpr auto keymapAdaptor(                                   \
                  const Key (&keymap)[_n_layers][_layer_size])                 \
          -> KeymapAdaptor<_n_layers, _layer_size>                             \
        {                                                                      \
          return KeymapAdaptor<_n_layers, _layer_size>{keymap};                \
        }                                                                      \
                                                                               \
        static constexpr auto keymapAdaptor()                                  \
          -> decltype(keymapAdaptor(::keymaps_linear))                         \
        {                                                                      \
          return keymapAdaptor(::keymaps_linear);                              \
        }                                                                      \
                                                                               \
      public:                                                                  \
                                                                               \
        /* COMPILE_TIME_USE_ONLY (see explanation above)                       \
         *                                                                     \
         * The collect function visits all keys in the keymap and generates    \
         * a result based on the requirements that are defined by the          \
         * _Accumulation functor op.                                           \
         *                                                                     \
         * This approach is necessary as there is currently (c++11) no         \
         * language feature available that would enable compile time iterative \
         * traversal of arrays like the keymap.                                \
         *                                                                     \
         * Examples for an accumulation could e.g. be the number of occurences \
         * of a specific key or if a key is present at all...                  \
         */                                                                    \
        template<typename _Accumulation>                                       \
        static constexpr auto collect(const _Accumulation &op)                 \
               -> typename _Accumulation::ResultType                           \
        {                                                                      \
           return accumulationHelper(op).apply();                              \
        }                                                                      \
                                                                               \
        /* COMPILE_TIME_USE_ONLY (see explanation above)                       \
         */                                                                    \
        static constexpr Key getKey(uint8_t layer, KeyAddr key_addr) {         \
          return keymapAdaptor().getKey(layer, key_addr);                      \
        }                                                                      \
                                                                               \
        static constexpr uint8_t nLayers() {                                   \
           return keymapAdaptor().n_layers;                                    \
        }                                                                      \
        static constexpr uint8_t layerSize() {                                 \
           return keymapAdaptor().layer_size;                                  \
        }                                                                      \
    };                                                                         \
  } /* namespace sketch_exploration */                                         \
  } /* namespace kaleidoscope */

} // namespace sketch_exploration
} // namespace kaleidoscope
