#include <t8.h>
#include <t8.h>                                 /* General t8code header, always include this. */
#include <t8_cmesh.h>                           /* cmesh definition and basic interface. */
#include <t8_cmesh/t8_cmesh_examples.h>         /* A collection of exemplary cmeshes */
#include <t8_forest/t8_forest_general.h>        /* forest definition and basic interface. */
#include <t8_forest/t8_forest_io.h>             /* save forest */
#include <t8_schemes/t8_default/t8_default.hxx> /* default refinement scheme. */
#include <t8_forest/t8_forest_iterate.h>

typedef struct {
  double point[3];
  t8_locidx_t result;
  int found;
} point_query_t;

struct element_data_storage {
  /* Storage for our element values. */
  double values;
};

static void point_query_fn(
    t8_forest_t forest,
    t8_locidx_t ltreeid,
    const t8_element_t *element,
    int is_leaf,
    const t8_element_array_t *leaf_elements,
    t8_locidx_t tree_leaf_index,
    sc_array_t *queries,
    sc_array_t *query_indices,
    int *query_matches,
    size_t num_active_queries)
{
    /* Nothing to do — we already populated queries manually */
}
static int
point_search_fn(t8_forest_t forest,
                t8_locidx_t ltreeid,
                const t8_element_t *element,
                int is_leaf,
                const t8_element_array_t *leaf_elements,
                t8_locidx_t tree_leaf_index)
{
  /* Get the query array passed to t8_forest_search() */
  point_query_t *q = (point_query_t*) t8_forest_get_user_data(forest);

  /* We assume only one point query */
  // point_query_t *q = (point_query_t*) sc_array_index_int(queries, 0);

  int inside = 0;
  double coords[3] = { q->point[0], q->point[1], q->point[2] };
  t8_forest_element_points_inside(forest, ltreeid, element,
                                  coords, 1, &inside, 1e-8);
  if (q->found) {
    return 0;  /* Already found — skip work */
  }
  t8_cmesh_t cmesh = t8_forest_get_cmesh(forest);
  t8_eclass_t tree_class = t8_cmesh_get_tree_class(cmesh, ltreeid);
  const t8_scheme_c* scheme = t8_forest_get_scheme(forest);
  int level = t8_element_get_level(scheme, tree_class, element);
  /* Test if the point is inside this element */
  t8_locidx_t element_index = t8_forest_get_tree_element_offset (forest, ltreeid) + tree_leaf_index;
  if (inside && is_leaf) {
        q->result = t8_forest_get_tree_element_offset(forest, ltreeid) + tree_leaf_index;
        q->found = 1;
        return 0;
    }
  return 1;
}
t8_locidx_t
search_single_point(t8_forest_t forest, double x, double y, double z)
{
  /* Prepare query struct */
  point_query_t q;
  q.point[0] = x;
  q.point[1] = y;
  q.point[2] = z;
  q.found = 0;

  t8_forest_set_user_data(forest, &q);
  /* Run search */
  t8_forest_search(forest,
                   point_search_fn,
                   NULL,
                   NULL);

  /* Read result */
  t8_locidx_t result = -1;
  if (q.found) {
    result = q.result;
  }
  return result;
}

bool get_element_value(sc_array_t *element_data, t8_locidx_t ielement, float &value_holder)
{
    if(element_data == NULL) {
      std::cout <<"element data is null";
      return false;
    }
    auto theArray = *((element_data_storage *) t8_sc_array_index_locidx(element_data, ielement));

    value_holder =(float) theArray.values;
    return true;
}