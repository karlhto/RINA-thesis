#!/usr/bin/env python3
# This script is bad

import csv
import argparse
import configparser

from pathlib import Path

# for hinting return values and args
from typing import Generator, List


def find_fingerprints(path: str) -> Generator[tuple, None, None]:
    ''' Takes an ini file, yield pairs of section name and fingerprint '''
    runs_base = 1
    config = configparser.ConfigParser()
    config.read(path)
    for key in config:
        runs = runs_base
        section = config[key]
        if 'repeat' in section:
            runs = int(section['repeat'])
            if section.name == "General":
                runs_base = runs
        if 'fingerprint' in section:
            yield (runs, key, section['fingerprint'].strip('"'))


def compile_row(path: Path, config: str,
                fingerprint: str, run: int) -> List[str]:
    ''' Compiles a CSV ready row '''
    cmdstring = '-f omnetpp.ini -r ' + str(run)
    if "Config" in config:
        cmdstring += ' -c ' + config.split(' ')[1]
    return [path.parent, cmdstring, '', fingerprint, 'PASS', '']


def build_csv(paths: List[Path], req_runs: int):
    ''' Builds a CSV compatible with INET's fingerprint tester '''
    with open('examples.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        for path in paths:
            for runs, config, fingerprint in find_fingerprints(path):
                for run in range(min(runs, req_runs)):
                    row = compile_row(path, config, fingerprint, run)
                    writer.writerow(row)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', default='.',
                        help='Directory to look in')
    parser.add_argument('-f', '--inifile',
                        default='omnetpp.ini', help='Filename to look for')
    parser.add_argument('-r', '--runs', type=int,
                        default='5', help='Amount of requested runs')
    args = parser.parse_args()
    path = args.dir
    runs = args.runs
    filename = args.inifile

    files = Path(path).rglob(filename)
    build_csv(files, runs)
