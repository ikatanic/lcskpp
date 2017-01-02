#include <string>
#include <utility>
#include <vector>

namespace Pavetic {
// A calculation of LCSk++ which switches between
// lcskpp_sparse_slow and lcskpp_sparse_fast, 
// depending on the number of match pairs in the
// strings.
// 
// If lcskpp_reconstruction equals NULL, only the
// value of the metric is computed, without reconstructing it.
void lcskpp(
    const std::string& a, const std::string& b, 
    const int k, int* lcskpp_length,
    std::vector<std::pair<int, int> >* lcskpp_reconstruction);

}
