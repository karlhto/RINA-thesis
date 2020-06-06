#!/usr/bin/env python3
# This script is bad

import re
import csv
import glob
import argparse
import configparser

from pathlib import Path


def find_files(init_path: str, filename: str) -> list:
    ''' Compiles list of paths '''
    path_list = []
    for path in Path(init_path).rglob(filename):
        path_list.append(path)
    return path_list


def find_fingerprints(path: str) -> dict:
    ''' Takes an ini file, returns dict with config name: fingerprint pairs '''
    fingerprints = {}

    config = configparser.ConfigParser()
    config.read(path)
    for key in config:
        section = config[key]
        if 'fingerprint' in section:
            fingerprints[key] = section['fingerprint'].strip('"')

    return fingerprints


def compile_row(path: Path, config: str, fingerprint: str) -> list:
    ''' Compiles a CSV ready row '''
    cmdstring = '-f omnetpp.ini'
    if "Config" in config:
        cmdstring += ' -c ' + config.split(' ')[1]
    return [path.parent, cmdstring, '', fingerprint, 'PASS', '']


def build_csv(paths: list):
    ''' Builds a CSV compatible with fingerprinttest '''
    with open('examples.csv', 'w') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        for path in paths:
            fingerprints = find_fingerprints(path)
            for config, fingerprint in fingerprints.items():
                row = compile_row(path, config, fingerprint)
                writer.writerow(row)


if __name__ == '__main__':
    # TODO add args and less terrible everything
    files = find_files('examples', 'omnetpp.ini')
    build_csv(files)
