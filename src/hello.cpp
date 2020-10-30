

#include <rcppsimdgeojson.hpp>

// [[Rcpp::export(.hello)]]
bool hello() { return true; }

// [[Rcpp::export(.fparse_sfc)]]
SEXP fparse_sfc(const Rcpp::CharacterVector& x,
                const bool expand_geometries = false) {
  if (std::size(x) > 0) {
    simdjson::dom::parser parser;

    if (const auto [parsed, error] = parser.parse(std::string_view(x[0]));
        !error) {
      return rcppsimdgeojson::dispatch_parse(parsed, expand_geometries);
    }
  }

  return R_NilValue;
}

// [[Rcpp::export(.fload_sfc)]]
SEXP fload_sfc(const Rcpp::CharacterVector& x,
               const bool expand_geometries = false) {

  if (std::size(x) > 0) {
    simdjson::dom::parser parser;

    if (const auto [parsed, error] = parser.load(std::string(x[0])); !error) {
      return rcppsimdgeojson::dispatch_parse(parsed, expand_geometries);
    }
  }

  return R_NilValue;
}
