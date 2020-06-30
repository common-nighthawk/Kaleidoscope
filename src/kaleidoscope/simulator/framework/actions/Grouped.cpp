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

#ifdef KALEIDOSCOPE_VIRTUAL_BUILD

#include "kaleidoscope/simulator/framework/actions/Grouped.h"
#include "kaleidoscope/simulator/framework/Simulator.h"

namespace kaleidoscope {
namespace simulator {
namespace framework {
namespace actions {

void Grouped<ReportAction_>::Action::determineGroupType() {
  auto report_type_ = actions_[0]->getReportTypeId();

  for (const auto &action : actions_) {

    auto new_type = action->getReportTypeId();

    if (new_type != AnyTypeReportTypeId) {
      if (report_type_ == AnyTypeReportTypeId) {
        report_type_ = new_type;
      } else if (report_type_ != new_type) {
        this->getSimulator()->error()
            << "Error grouping report actions. Trying to group non-generic actions of different type";
        actions_.clear();
        report_type_ = AnyTypeReportTypeId;
        break;
      }
    }
  }
}
} // namespace actions
} // namespace framework
} // namespace simulator
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_VIRTUAL_BUILD
