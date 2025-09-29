import os
import re
import json
import matplotlib.pyplot as plt
from collections import defaultdict
import numpy as np
from scipy.stats import norm

# Defintions
def get_value(line):
    parts = line.strip().split()
    value = float(parts[-1])
    return value if value > 0 else 0

def get_app(line):
    ue_number = ""
    parts = line.strip().split()
    module_path = parts[1]
    path_parts = module_path.split('.')
    for part in path_parts:
        if part.startswith('ue[') and part.endswith(']'):
            ue_number = int(part[3:-1])
            break
    return ue_number

def compute_confidence_interval(missed_values, confidence=0.95):
    """
    Missed Deadlines Per Repetition
    """
    if len(missed_values) < 2:
        lower = min(missed_values)
        upper = max(missed_values)
    else:
        n = len(missed_values)
        miss_counter_mean = np.mean(missed_values)
        s = np.std(missed_values)
        alpha = 1 - confidence
        z = norm.ppf(1 - alpha/2)
        margin = z*(s/np.sqrt(n))
        upper = miss_counter_mean + margin
        lower = miss_counter_mean - margin
    return (lower, upper)

def per_run_confidence_intervals(missed_values, pkt_values, confidence=0.95):
    ci_list = []
    for missed, pkts in zip(missed_values, pkt_values):
        ci = compute_confidence_interval([missed], [pkts], confidence=confidence)
        ci_list.append(ci)
    return ci_list

def process_file(path):
    with open(path, "r") as fh:
        data = json.load(fh)
    missed_total, pkts_total = 0.0, 0.0
    for entry in data:
        missed_total += float(entry.get("MissedDeadlineCounter", 0.0))
        pkts_total   += float(entry.get("PktCounter", 0.0))
    return missed_total, pkts_total

def collect_results(version):
    pattern = re.compile(rf".*_{version}_(?P<sched>edf|pf)(?P<rb>\d+)", re.IGNORECASE)
    data = defaultdict(lambda: defaultdict(list))
    for dirpath, _, files in os.walk("data"):
        for fname in files:
            if not fname.lower().endswith(".json"):
                continue
            full = os.path.join(dirpath, fname)
            m = pattern.search(full) or pattern.search(fname)
            if not m:
                continue
            sched = m.group("sched").upper()
            if sched == "EDF":
                sched = "NR-"+sched
            rb = int(m.group("rb"))
            missed, pkts = process_file(full)
            data[sched][rb].append((missed, pkts))
    return data

def compute_averages(data):
    avg = {}
    for sched, rb_dict in data.items():
        avg[sched] = {}
        for rb, vals in sorted(rb_dict.items()):
            missed = np.array([m for m,_ in vals])
            pkts   = np.array([p for _,p in vals])
            mean_missed = missed.mean() if len(missed) else 0
            mean_pkts   = pkts.mean() if len(pkts) else 0
            avg[sched][rb] = 100 - (mean_missed / mean_pkts * 100) if mean_pkts > 0 else 0
    return avg

def plot_results(all_avgs, LOG_X=False, LOG_Y=False):
    plt.figure(figsize=(10,6))
    markers = ["o", "s", "^", "D", "x"]
    for i, (version, avg) in enumerate(all_avgs.items()):
        for sched, rb_map in avg.items():
            rbs  = sorted(rb_map.keys())
            vals = [rb_map[rb] for rb in rbs]

            if LOG_Y:
                rbs_plot = [x for x, y in zip(rbs, vals) if y > 0]
                vals_plot = [y for y in vals if y > 0]
            else:
                rbs_plot = rbs
                vals_plot = vals

            if version == "v7":
                version = "periodic"
            elif version == "v8":
                version = "sporadic"
            plt.plot(rbs_plot, vals_plot, marker=markers[i % len(markers)],
                     linewidth=2, label=f"{sched} ({version})")

            # Annotate each point with its value
            for x, y in zip(rbs_plot, vals_plot):
                plt.text(x, y*1.2 if LOG_Y else y*1.05,
                         f"{y:.2f}", ha="center", va="bottom",
                         fontsize=8, rotation=0)

    plt.xlabel("Number of RBs")
    plt.ylabel("Schedulability (%)")
    if LOG_X: plt.xscale("log")
    if LOG_Y: plt.yscale("log")
    plt.grid(True, which="both", linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.show()

def print_table(avg, version):
    if version == "v7":
        version = "periodic"
    elif version == "v8":
        version = "sporadic"
    print(f"\n=== Version: {version} ===")
    for sched, rb_map in avg.items():
        print(f"Scheduler: {sched}")
        print("RB\tSchedulability(%)")
        for rb in sorted(rb_map.keys()):
            print(rb, "\t", f"{rb_map[rb]:.8f}")