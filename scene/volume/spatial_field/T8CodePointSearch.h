#pragma once

#include <t8.h>
#include <t8_cmesh.h>
#include <t8_forest/t8_forest_general.h>
#include <t8_forest/t8_forest_io.h>
#include <t8_schemes/t8_default/t8_default.hxx>

typedef struct {
    double point[3];     /**< 3D query point */
    t8_locidx_t result;  /**< Output: global element index */
    int found;           /**< Whether the element was found */
} point_query_t;

struct element_data_storage {
  /* Storage for our element values. */
  double values;
};
int point_search_fn(t8_forest_t forest,
                    t8_locidx_t ltreeid,
                    const t8_element_t *element,
                    int is_leaf,
                    const t8_element_array_t *leaf_elements,
                    t8_locidx_t tree_leaf_index);

void point_query_fn(t8_forest_t forest,
                    t8_locidx_t ltreeid,
                    const t8_element_t *element,
                    int is_leaf,
                    const t8_element_array_t *leaf_elements,
                    t8_locidx_t tree_leaf_index,
                    sc_array_t *queries,
                    sc_array_t *query_indices,
                    int *query_matches,
                    size_t num_active_queries);

t8_locidx_t search_single_point(t8_forest_t forest,
                                double x, double y, double z);

double get_element_value(sc_array_t *element_data, t8_locidx_t ielement, float &value_holder);

