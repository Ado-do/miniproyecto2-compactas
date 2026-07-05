#include "FMIndex.hpp"
#include "utils.hpp"

#include <cassert>

using namespace std;

int main(int argc, char *argv[]) {
    // Recibir archivo via CLI
    assert(argc == 2);
    vector<uint8_t> text = read_file_to_vector(argv[1]);

    FMIndex<OccMyWT> fm(text);
    string s = "foo";
    vector<uint8_t> pattern(s.begin(), s.end());
    assert(fm.count(pattern) == 2);

    s = "Foo";
    pattern.assign(s.begin(), s.end());
    assert(fm.count(pattern) == 1);

    s = "oo";
    pattern.assign(s.begin(), s.end());
    assert(fm.count(pattern) == 3);

    return 0;
}
