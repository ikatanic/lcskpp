// Copyright 2017: I. Katanic, G. Matula
#ifndef LCSKPP_H_
#define LCSKPP_H_

#include <string>
#include <vector>
#include <utility>

// Returns length of LCSk++ of given strings a and b.
// LCSK++ indices will be reconstructed to vector pointed to by
// `reconstruction` arg pointer. If it's set to NULL reconstruction is skipped.
int lcskpp(const std::string& a, const std::string& b, int k,
          std::vector<std::pair<int, int>>* reconstruction);

#endif  // LCSKPP_H_
