#include <t8.h>
#include <t8.h>                                 /* General t8code header, always include this. */
#include <t8_cmesh.h>                           /* cmesh definition and basic interface. */
#include <t8_cmesh/t8_cmesh_examples.h>         /* A collection of exemplary cmeshes */
#include <t8_forest/t8_forest_general.h>        /* forest definition and basic interface. */
#include <t8_forest/t8_forest_io.h>             /* save forest */
#include <t8_schemes/t8_default/t8_default.hxx> /* default refinement scheme. */

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

  if (q->found) {
    return 0;  /* Already found — skip work */
  }
  t8_cmesh_t cmesh = t8_forest_get_cmesh(forest);
  t8_eclass_t tree_class = t8_cmesh_get_tree_class(cmesh, ltreeid);
  const t8_scheme_c* scheme = t8_forest_get_scheme(forest);
  double coords[3] = { q->point[0], q->point[1], q->point[2] };
  int inside = 0;
  int level = t8_element_get_level(scheme, tree_class, element);
  /* Test if the point is inside this element */
  t8_locidx_t element_index = t8_forest_get_tree_element_offset (forest, ltreeid) + tree_leaf_index;
  t8_forest_element_points_inside(forest, ltreeid, element,
                                  coords, 1, &inside, 1e-8);
  if (inside) {
    if(is_leaf){
        std::cout << "ni hao " << std::endl;
        q->result = t8_forest_get_tree_element_offset(forest, ltreeid) + tree_leaf_index;
        q->found = 1;
        return 0;
    } else {
        return 1;
    }
  }

  return 0;
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
                   point_query_fn,
                   NULL);

  /* Read result */
  t8_locidx_t result = -1;
  if (q.found) {
    result = q.result;
  }

  return result;
}