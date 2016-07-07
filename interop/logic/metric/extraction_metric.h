/** Logic for extraction metrics
 *
 *  @file
 *  @date  7/7/16
 *  @version 1.0
 *  @copyright GNU Public License.
 */
#pragma once
#include <vector>
#include <set>
#include "interop/model/metrics/extraction_metric.h"
#include "interop/model/model_exceptions.h"
#include "interop/model/metric_base/metric_set.h"


namespace illumina { namespace interop { namespace logic { namespace metric
{


    /** Example used for the performance test in C#
     *
     * This shows a simple way to speed up analysis of data using the C# binding.
     *
     * @param metrics extraction metric set
     * @param focus_scores array of focus scores
     * @param channel channel for focus
     * @param n size of array
     */
    inline void copy_focus(const model::metric_base::metric_set<model::metrics::extraction_metric>& metrics,
                           float *focus_scores,
                           const size_t channel,
                           const size_t n) throw(model::invalid_parameter)
    {
        typedef model::metric_base::metric_set<model::metrics::extraction_metric>::const_iterator const_iterator;
        if(metrics.size()==0)return;
        if(n < metrics.size()) throw model::invalid_parameter("Buffer size too small for metric set");
        if(channel >= metrics.metrics()[0].channel_count())
            throw model::invalid_parameter("Channel exceeds channel count");
        for (const_iterator it = metrics.begin(); it != metrics.end(); ++it, ++focus_scores)
            *focus_scores = it->focus_score(channel);
    }


}}}}
