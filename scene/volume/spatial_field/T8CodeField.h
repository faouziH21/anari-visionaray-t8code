// Copyright 2023-2025 Stefan Zellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// t8code
#include <t8.h>
#include <t8_forest/t8_forest_general.h>        /* forest definition and basic interface. */
#include <t8_forest/t8_forest_geometrical.h>    /* geometrical information */
#include <t8_geometry/t8_geometry.h>
#include <t8_cmesh.h>
// ours
#include "SpatialField.h"

namespace visionaray {

struct T8CodeField : public SpatialField
{
  T8CodeField(VisionarayGlobalState *d);
  ~T8CodeField();
  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

  aabb bounds() const override; //get bounds through t8code

  void setForest (t8_forest_t forest);
  t8_forest_t returnForest();
  private:
   sc_array_t *t8_element_data = NULL;
   t8_forest_t t8_forest = NULL;
};

} // namespace visionaray
