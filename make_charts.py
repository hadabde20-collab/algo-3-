"""Generates the charts from results/benchmark.csv (optional helper script)."""
import csv, sys, collections
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

rows = list(csv.DictReader(open("results/benchmark.csv")))
data = collections.defaultdict(lambda: collections.defaultdict(list))
for r in rows:
    data[(r["input"], "wall")][r["algorithm"]].append((int(r["size"]), float(r["wall_ms"])))
    data[(r["input"], "mem")][r["algorithm"]].append((int(r["size"]), float(r["memory_kb"])))

for inp in ["random", "sorted", "reverse", "duplicates"]:
    fig, axes = plt.subplots(1, 2, figsize=(11, 4))
    for ax, kind, title, unit in [(axes[0], "wall", "Execution time", "ms"),
                                  (axes[1], "mem", "Peak extra memory", "KB")]:
        for algo, pts in data[(inp, kind)].items():
            pts.sort()
            xs, ys = zip(*pts)
            ax.plot(xs, [max(y, 1e-3) for y in ys], marker="o", label=algo)
        ax.set_xscale("log"); ax.set_yscale("log")
        ax.set_xlabel("array size n"); ax.set_ylabel(unit)
        ax.set_title(f"{title} - {inp} input"); ax.grid(True, which="both", alpha=.3)
    axes[0].legend(fontsize=8)
    plt.tight_layout()
    plt.savefig(f"results/chart_{inp}.png", dpi=130)
    plt.close()
print("charts written")
