#include "OccMyWT.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    // Recibir archivo via CLI
    assert(argc == 2);
    const string file_path = argv[1];

    vector<uint8_t> text = read_file_to_vector(file_path);
    size_t n = text.size();

    vector<uint32_t> buf(text.begin(), text.end());
    WaveletTree wt(buf);
    for (size_t i = 0; i < n; i++) {
        // Comprobar correctitud de construcción del WaveletTree
        assert(buf[i] == wt.access(i));
    }

    size_t real_cnt = count(text.begin(), text.end(), 'T');
    size_t wt_cnt = wt.rank('T', n);
    printf("real: %zu, wt: %zu\n", real_cnt, wt_cnt);

    // Comprobar correctitud del rank
    assert(real_cnt == wt_cnt);

    return 0;
}
