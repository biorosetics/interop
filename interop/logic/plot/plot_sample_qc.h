/** Plot reads identified versus index
 *
 *  @file
 *  @date 5/10/16
 *  @version 1.0
 *  @copyright GNU Public License.
 */
#pragma once
#include <map>
#include "interop/util/statistics.h"
#include "interop/constants/enums.h"
#include "interop/model/model_exceptions.h"
#include "interop/model/run_metrics.h"
#include "interop/model/plot/bar_point.h"
#include "interop/logic/plot/plot_data.h"

namespace illumina { namespace interop { namespace logic { namespace plot {

   /** Populate reads identified versus the index
    *
    * @param metrics set of metric records
    * @param proxy functor that takes a metric record and returns a metric value
    * @param options filter for metric records
    * @param type type of metric to extract using the proxy functor
    * @param points collection of points where x is lane number and y is the candle stick metric values
    */
    template<typename Point>
    float populate_reads_identified(const model::metric_base::metric_set<model::metrics::index_metric>& index_metrics,
                                   const model::metric_base::metric_set<model::metrics::tile_metric>& tile_metrics,
                                   const size_t lane,
                                   model::plot::data_point_collection<Point>& points)
    {
        typedef model::metric_base::metric_set<model::metrics::index_metric> index_metric_set_t;
        typedef std::map<std::string, size_t> index_count_map_t;
        typedef typename index_count_map_t::iterator map_iterator;
        typedef typename index_count_map_t::const_iterator const_map_iterator;
        typedef typename model::metrics::index_metric::const_iterator const_index_iterator;

        index_count_map_t index_count_map;
        float pf_cluster_count_total = 0;
        for(typename index_metric_set_t::const_iterator b = index_metrics.begin(), e = index_metrics.end();b != e;++b)
        {
            if(lane != b->lane()) continue;
            try
            {
                const model::metrics::tile_metric &tile_metric = tile_metrics.get_metric(b->lane(), b->tile());
                pf_cluster_count_total += tile_metric.cluster_count_pf();
                for(const_index_iterator ib = b->indices().begin(), ie =  b->indices().end();ib != ie;++ib)
                {
                    map_iterator found_index = index_count_map.find(ib->index_seq());
                    if(found_index == index_count_map.end()) index_count_map[ib->index_seq()] = ib->count();
                    else found_index->second += ib->count();
                }
            }
            catch(const model::index_out_of_bounds_exception&){continue;} // TODO: check better?
        }
        points.resize(index_count_map.size());
        size_t i=0;
        float max_height=0;
        for(const_map_iterator b = index_count_map.begin(), e = index_count_map.end();b != e;++b, ++i)
        {
            const float height = (pf_cluster_count_total==0) ? 0 : b->second / pf_cluster_count_total * 100.0f;
            points[i].set(i+1.0f, height, 1.0f);
            max_height = std::max(max_height, height);
        }
        return max_height;
    }

    /** Plot reads identified versus index
     *
     * @param metrics run metrics
     * @param lane lane number
     * @param options options to filter the data
     * @param data output plot data
     */
    inline void plot_sample_qc(const model::metrics::run_metrics& metrics,
                               const size_t lane,
                               model::plot::plot_data<model::plot::bar_point>& data)
    {
        typedef constants::enumeration<constants::bar_plot_options> bar_plot_options_t;
        typedef model::plot::series<model::plot::bar_point> bar_series_t;
        data.assign(1, bar_series_t("% reads", "Green", bar_series_t::Bar));
        data[0].add_option(bar_plot_options_t::to_key(constants::CenteredBar));
        data.set_xlabel("Index Number");
        data.set_ylabel("% Reads Identified (PF)");

        const float max_height = populate_reads_identified(metrics.get_set<model::metrics::index_metric>(),
                                                           metrics.get_set<model::metrics::tile_metric>(),
                                                           lane,
                                                           data[0]);
        auto_scale(data);
        data.set_range(data.x_axis().min(), static_cast<float>(data[0].size()), data.y_axis().min(), max_height+5);
    }


}}}}
