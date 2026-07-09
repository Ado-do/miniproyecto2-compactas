set datafile separator ","

set terminal pdfcairo enhanced size 5,3.5
set output "draws/space_bits.pdf"

set title "Memory Space Efficiency (Bits per Edge)"
set ylabel "Bits per Edge (bits/m)"
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
set key right top Left reverse

plot \
  "< grep ',adjacency,' ../benchmark_full.csv | grep ',degree,'" using (graph_idx(stringcolumn(1)) - 0.25):(($6*8.0)/$5) with boxes ls 1 title "Adjacency List", \
  ""                                                             using (graph_idx(stringcolumn(1)) - 0.25):(($6*8.0)/$5):(sprintf("%.1f", ($6*8.0)/$5)) with labels tc ls 1 offset character 0,0.5 notitle, \
  \
  "< grep ',k2-tree,' ../benchmark_full.csv | grep ',degree,'" using (graph_idx(stringcolumn(1))):(($6*8.0)/$5) with boxes ls 2 title "k2-tree Basic", \
  ""                                                           using (graph_idx(stringcolumn(1))):(($6*8.0)/$5):(sprintf("%.1f", ($6*8.0)/$5)) with labels tc ls 2 offset character 0,0.5 notitle, \
  \
  "< grep ',pemb,' ../benchmark_full.csv | grep ',degree,'" using (graph_idx(stringcolumn(1)) + 0.25):(($6*8.0)/$5) with boxes ls 3 title "pemb (Turán)", \
  ""                                                        using (graph_idx(stringcolumn(1)) + 0.25):(($6*8.0)/$5):(sprintf("%.1f", ($6*8.0)/$5)) with labels tc ls 3 offset character 0,0.5 notitle
