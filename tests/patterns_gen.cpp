#include "utils.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    assert(argc != 2);
    vector<uint8_t> text = read_file_to_vector(argv[1]);

    const size_t n = text.size() * 0.1; // 10%
    vector<uint8_t> portion(text.begin(), text.begin() + n);
    const size_t m = 100;
    const size_t n_patterns = 5;
    auto patterns = generate_patterns(portion, m, n_patterns);

    for (size_t i = 0; i < n_patterns; i++) {
        string pattern(patterns[i].begin(), patterns[i].end());
        cout << "Pattern " << i << ":\n\t" << quoted(pattern) << '\n';
        assert(patterns[i].size() == m);
    }

    return 0;
}
