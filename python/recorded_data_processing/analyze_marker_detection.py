#!/usr/bin/env python3
import argparse
import os
import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('detected_markers',
                        help='output file(s) of marker_detection_analysis program (such as tk1.detected_tags)')
    parser.add_argument('ids', nargs="*", help='file(s) of ids that you want to allow.'
                                               ' If more than one is provided, they will be plotted and compared.')
    parser.add_argument('--no-plot', '-p', action="store_true", help='show the plots')
    args = parser.parse_args()

    detected_markers = np.genfromtxt(args.detected_markers, delimiter=',', dtype=np.int32)
    ids_to_detections_map = {}
    print("name,worst_case,percentile,mean,median,mode")
    for id_filename in args.ids:
        allowed_ids = np.genfromtxt(id_filename, delimiter=',', dtype=np.int32)

        tag_detections = []
        for line in detected_markers:
            tag_id = int(line[1])
            if tag_id in allowed_ids:
                tag_detections.append([int(d) for d in line[:2]])
        tag_detections = np.array(tag_detections)

        times_between_detections = [0]
        last_distinct_idx = 0
        for idx in range(1, tag_detections.shape[0]):
            if tag_detections[idx, 0] != tag_detections[last_distinct_idx, 0]:
                dt_s = (tag_detections[idx, 0] - tag_detections[last_distinct_idx, 0]) / 1e6
                times_between_detections.append(dt_s)
                last_distinct_idx = idx
        times_between_detections = np.array(times_between_detections)

        ids_to_detections_map[id_filename] = {
            'tag_detections': tag_detections,
            'times_between_detections': times_between_detections
        }

        mean = np.mean(times_between_detections)
        median = np.median(times_between_detections)
        maximum = np.max(times_between_detections)
        mode = stats.mode(times_between_detections)[0][0]
        percentile = np.percentile(times_between_detections, 95)
        print("{:9s}, {:0.3f}, {:0.3f}, {:0.3f}, {:0.3f}, {:0.3f}".format(os.path.basename(id_filename),
                                                                          maximum, percentile, mean, median, mode))

    plt.style.use("python/phil.mplstyle")
    if not args.no_plot:
        plt.figure()
        for allowed_ids, detections in ids_to_detections_map.items():
            name = os.path.basename(allowed_ids)
            plt.plot(detections['times_between_detections'], label=name)
        plt.ylabel("time since last detected tag (seconds)")
        ax = plt.gca()
        ax.set_xticks([])
        plt.xlabel("instances of detected tags over time")
        handles, labels = ax.get_legend_handles_labels()
        # sort both labels and handles by labels
        labels, handles = zip(*sorted(zip(labels, handles), key=lambda t: t[0]))
        ax.legend(handles, labels)
        plt.title("Time Between Detected Tags")

        plt.show()


if __name__ == '__main__':
    main()
