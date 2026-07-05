#include "OccMyWT.hpp"
#include "OccBruteForce.hpp"
#include "OccSDSL.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    // Recibir archivo via CLI
    assert(argc == 2);
    vector<uint8_t> text = read_file_to_vector(argv[1]);
    size_t n = text.size();
    vector<uint8_t> bwt = get_bwt(text);

    unsigned char c = text[10];
    size_t cnt_text = count(text.begin(), text.end(), c);
    size_t cnt_bwt = count(bwt.begin(), bwt.end(), c);
    printf("* '%c': cnt_text = %zu, cnt_bwt = %zu\n", c, cnt_text, cnt_bwt);
    assert(cnt_text == cnt_bwt);

    string s(bwt.begin(), bwt.end());

    OccBruteForce o0(bwt);
    OccMyWT o1(bwt);
    OccBalancedWT o2(bwt);
    OccHuffmanWT o3(bwt);

    uint8_t v = c;
    uint64_t k = n;

    uint64_t r0 = o0.occ(v, k);
    uint64_t r1 = o1.occ(v, k);
    uint64_t r2 = o2.occ(v, k);
    uint64_t r3 = o3.occ(v, k);
    printf("* occ(%c, %zu): r0 = %zu, r1 = %zu, r2 = %zu, r3 = %zu\n", v, k, r0, r1, r2, r3);
    assert(r0 == r1);
    assert(r1 == r2);
    assert(r2 == r3);

    return 0;
}
