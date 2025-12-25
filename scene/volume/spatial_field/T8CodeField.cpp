#include "T8CodeField.h"
#include <nanovdb/io/IO.h>

namespace visionaray{

T8CodeField::T8CodeField(VisionarayGlobalState *d)
    :   SpatialField(d)
{
    vfield.type = dco::SpatialField::T8Code;
    std::cout<<"created T8Code field"<< std::endl;
    // TODO: implement a t8code spatial field in DeviceCopyableObjects.h
}
T8CodeField::~T8CodeField(){
  // T8_FREE(t8_element_data);
}
//holt pointer
void T8CodeField::commitParameters()
{
  bool result = getParam("forest", ANARI_VOID_POINTER, &t8_forest);
  bool data = getParam("forest_data", ANARI_VOID_POINTER, &t8_element_data);
}

bool T8CodeField::isValid() const
{
  if (t8_forest == NULL || t8_element_data == NULL){
    return false;
  }
  return true;
}

void T8CodeField::finalize()
{
  vfield.asT8code.forest = t8_forest;
  vfield.asT8code.element_data = t8_element_data;
  vfield.voxelSpaceTransform = mat4x3(mat3::identity(),float3{0.f,0.f,0.f});

  setCellSize(1.f);

  dispatch();
}

aabb T8CodeField::bounds() const
{
    if (!isValid())
    return {};
  const double INF = std::numeric_limits<double>::infinity();
  double bbox [6] = {INF, -INF, INF, -INF, INF, -INF};
/*
  According to t8code:
  bounds[0] = x_min
  bounds[1] = x_max
  bounds[2] = y_min
  bounds[3] = y_max
  bounds[4] = z_min
  bounds[5] = z_max
*/
  t8_cmesh_t cmesh = t8_forest_get_cmesh(t8_forest);
  t8_cmesh_get_local_bounding_box(cmesh, bbox);
  return aabb{{(float)bbox[0], (float)bbox[2], (float)bbox[4]},
              {(float)bbox[1], (float)bbox[3], (float)bbox[5]}};
}
void T8CodeField::setForest(t8_forest_t forest) {
  t8_forest = forest;
}

t8_forest_t T8CodeField::returnForest(){
  return t8_forest;
}

}