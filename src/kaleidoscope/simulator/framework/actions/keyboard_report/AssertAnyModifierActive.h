/* -*- mode: c++ -*-
 * kaleidoscope::simulator::framework - A keyboard simulation framework
 * Copyright (C) 2019  noseglasses (shinynoseglasses@gmail.com)
 * Copyright (C) 2020  Keyboard.io, Inc
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

#ifdef KALEIDOSCOPE_VIRTUAL_BUILD

#include "kaleidoscope/simulator/framework/actions/generic_report/ReportAction.h"

namespace kaleidoscope {
namespace simulator {
namespace framework {
namespace actions {

/// @brief Asserts that any modifiers are active in the current keyboard report.
///
class AssertAnyModifierActive {

 public:

  SIMULATOR_ACTION_STD_CONSTRUCTOR(AssertAnyModifierActive)

 private:

  class Action : public ReportAction<KeyboardReport_> {

   public:

    virtual void describe(const char *add_indent = "") const override {
      this->getSimulator()->log() << add_indent << "Any modifiers active";
    }

    virtual void describeState(const char *add_indent = "") const {
      this->getSimulator()->log() << add_indent << "Any modifiers active: ";
      this->getSimulator()->log() << this->getReport().isAssertAnyModifierActive();
    }

    virtual bool evalInternal() override {
      return this->getReport().isAssertAnyModifierActive();
    }
  };

  SIMULATOR_AUTO_DEFINE_ACTION_INVENTORY(AssertAnyModifierActive)
};

} // namespace actions
} // namespace framework
} // namespace simulator
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_VIRTUAL_BUILD
