#include "FMIndex.hpp"
#include "bench-lib/benchmark.hpp"
#include "bench-lib/benchmark_manager.hpp"
#include "utils.hpp"

#include <vector>
#include <cstdint>

using namespace std;

const vector<size_t> sizes{32, 64, 128, 256, 512};
const size_t n_patterns = 100;
string csv_name = "patterns_results";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <text_file>\n";
        return 1;
    }
    string text_path = argv[1];
    string text_name = sdsl::util::basename(text_path);
    csv_name = text_name + "_" + csv_name;
    cout << "* Experimentos sobre el texto " << quoted(text_name) << " utilizando patrones de diferente largo!\n";

    vector<uint8_t> text = read_file_to_vector(text_path);

    FMIndex<OccMyWT>        fm1(text);
    FMIndex<OccBruteForce>  fm2(text);
    FMIndex<OccBalancedWT>  fm3(text);
    FMIndex<OccHuffmanWT>   fm4(text);

    {
        BenchLib::Benchmark bench;
        vector<uint8_t> pattern = (generate_patterns(text, sizes[0], 1))[0];
        bench.add(text_name + "+OccBruteForce", [&fm2](vector<uint8_t> &pattern) {
            return fm2.count(pattern);
        }, pattern).set_label(text_name.substr(0, 3) + "+occ2").set_input_size(sizes[0]).set_size_in_megabytes(fm2.size_mb());
        bench.run();
        bench.write_csv(csv_name);
    }

    for (size_t i = 0; i < sizes.size(); i++) {
        size_t sz = sizes[i];
        vector<vector<uint8_t>> patterns = generate_patterns(text, sz, n_patterns);

        BenchLib::BenchmarkManager manager;
        for (size_t j = 0; j < n_patterns; j++) {
            BenchLib::Benchmark bench;

            bench.add(text_name + "+OccMyWT", [&fm1](vector<uint8_t> &pattern) {
                return fm1.count(pattern);
            }, patterns[j]).set_label(text_name.substr(0, 3) + "+occ1").set_input_size(sz).set_size_in_megabytes(fm1.size_mb());

            bench.add(text_name + "+OccBalancedWT", [&fm3](vector<uint8_t> &pattern) {
                return fm3.count(pattern);
            }, patterns[j]).set_label(text_name.substr(0, 3) + "+occ3").set_input_size(sz).set_size_in_megabytes(fm3.size_mb());

            bench.add(text_name + "+OccHuffmanWT", [&fm4](vector<uint8_t> &pattern) {
                return fm4.count(pattern);
            }, patterns[j]).set_label(text_name.substr(0, 3) + "+occ4").set_input_size(sz).set_size_in_megabytes(fm4.size_mb());

            bench.run();

            size_t r1 = bench.get_result<size_t>(0);
            size_t r3 = bench.get_result<size_t>(1);
            size_t r4 = bench.get_result<size_t>(2);

            assert(r1 == r3);
            assert(r3 == r4);

            manager.add_results(bench.get_tasks());
        }
        manager.average_by_task();
        manager.append_csv(csv_name);
    }

    cout << "* Resultados guardados en : results/" << csv_name << ".csv" << endl;

    return 0;
}
