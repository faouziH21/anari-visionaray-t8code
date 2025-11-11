#include "T8codeField.h"
#include <nanovdb/io/IO.h>

namespace visionaray{

T8CodeField::T8CodeField(VisionarayGlobalState *d)
    :   SpatialField(d)
{
    // TODO: implement a t8code spatial field in DeviceCopyableObjects.h
}
T8CodeField::~T8CodeField(){
  T8_FREE(t8_element_data);
}

void T8CodeField::commitParameters()
{
  t8_element_data = T8_ALLOC (double, t8_forest_get_local_num_leaf_elements(forest))
}


void T8CodeField::finalize()
{
  vfield.asT8code.cmesh = this.cmesh;

  vfield.voxelSpaceTransform = mat4x3(mat3::identity(),float3{0.f,0.f,0.f});

  setCellSize(1.f);

  buildGrid();

  vfield.gridAccel = m_gridAccel.visionarayAccel();

  dispatch();
}

aabb T8codeField::bounds() const
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
  t8_cmesh_get_local_bounding_box(cmesh, bbox);
  return aabb{{(float)bounds[0], (float)bounds[2], (float)bounds[4]},
              {(float)bounds[1], (float)bounds[3], (float)bounds[5]}};
}
void T8codeField::buildGrid(){

 int3 gridDims{16, 16, 16};
  box3f worldBounds = {bounds().min,bounds().max};
  box3f gridBounds = worldBounds;
  m_gridAccel.init(gridDims, worldBounds, gridBounds);

  dco::GridAccel &vaccel = m_gridAccel.visionarayAccel();
  // TODO: accessor function from t8code
  // auto acc = vfield.asT8code.grid->getAccessor();

  float3 mcSize = (worldBounds.max-worldBounds.min) / float3(gridDims);

  for (unsigned mcz=0; mcz<gridDims.z; ++mcz) {
    for (unsigned mcy=0; mcy<gridDims.y; ++mcy) {
      for (unsigned mcx=0; mcx<gridDims.x; ++mcx) {
        const vec3i mcID(mcx,mcy,mcz);
        box3f mcBounds{worldBounds.min+mcSize*float3(mcx,mcy,mcz),
                       worldBounds.min+mcSize*float3(mcx+1,mcy+1,mcz+1)};
        //use nanovdb's bbox for simplicity
        nanovdb::CoordBBox bbox(
            {(int)mcBounds.min.x,(int)mcBounds.min.y,(int)mcBounds.min.z},
            // + 1 because (int) rounds down
            {(int)mcBounds.max.x+1,(int)mcBounds.max.y+1,(int)mcBounds.max.z+1});

        for (nanovdb::CoordBBox::Iterator iter = bbox.begin(); iter; ++iter) {
          // TODO: accessor function from t8code
          // float value = acc.getValue(*iter);
          updateMC(mcID,gridDims,value,vaccel.valueRanges);
          updateMCStepSize(mcID,gridDims,0.5f,vaccel.stepSizes); // TODO!?
        }
      }
    }
  }
}

}