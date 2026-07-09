set datafile separator ","
set encoding utf8

set terminal pdfcairo enhanced font "sans,10" size 5,3.5
set output "draws/time_neighbors.pdf"

set title "Query Performance: Neighbors (Log Scale)" font "sans-bold,12"
set ylabel "Average Time per Operation ({/Symbol m}s)" font "sans,10"
set logscale y
set grid y

# Define colors
set style line 1 lc rgb '#3b82f6' # Adjacency (Blue)
set style line 2 lc rgb '#8b5cf6' # k2-tree (Purple)
set style line 3 lc rgb '#10b981' # pemb (Emerald Green)

set boxwidth 0.22
set style fill solid 0.8 border -1

# Map graph file names to positions on x-axis
graph_idx(name) = (name eq "tiger_map_hawaii.pg" ? 1.0 : \
                   (name eq "planar_embedding1000000.pg" ? 2.0 : \
                    (name eq "worldcitiespop.pg" ? 3.0 : NaN)))

set xtics ("Hawaii" 1, "Planar 1M" 2, "World Cities" 3) nomirror
set ytics nomirror
set key top left Left reverse

plot \
  "< grep ',adjacency,' ../benchmark_full.csv | grep ',neighbors,'" using (graph_idx(stringcolumn(1)) - 0.25):8 with boxes ls 1 title "Adjacency List", \
  "< grep ',k2-tree,' ../benchmark_full.csv | grep ',neighbors,'" using (graph_idx(stringcolumn(1))):8 with boxes ls 2 title "k2-tree Basic", \
  "< grep ',pemb,' ../benchmark_full.csv | grep ',neighbors,'" using (graph_idx(stringcolumn(1)) + 0.25):8 with boxes ls 3 title "pemb (Turán)", \
  "< grep ',adjacency,' ../benchmark_full.csv | grep ',neighbors,'" using (graph_idx(stringcolumn(1)) - 0.25):8:($8-$9 > 0 ? $8-$9 : 1e-6):($8+$9) with yerrorbars lc rgb 'black' pt 0 notitle, \
  "< grep ',k2-tree,' ../benchmark_full.csv | grep ',neighbors,'" using (graph_idx(stringcolumn(1))):8:($8-$9 > 0 ? $8-$9 : 1e-6):($8+$9) with yerrorbars lc rgb 'black' pt 0 notitle, \
  "< grep ',pemb,' ../benchmark_full.csv | grep ',neighbors,'" using (graph_idx(stringcolumn(1)) + 0.25):8:($8-$9 > 0 ? $8-$9 : 1e-6):($8+$9) with yerrorbars lc rgb 'black' pt 0 notitle
