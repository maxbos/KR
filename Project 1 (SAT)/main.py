#!/usr/bin/python3

import sys
import argparse


def main(strategy_config):
    (strategy, input_fpath) = strategy_config
    dp = DavisPutnam(strategy, input_fpath)


class DavisPutnam(object):
    def __init__(self, strategy, input_fpath):
        """"""
        self.strategy = strategy
        self.input_fpath = input_fpath


if __name__ == '__main__':
    assert len(sys.argv) > 1, 'python3 main.py [-S1/-S2/-S3] INPUT_FILE'

    parser = argparse.ArgumentParser()
    parser.add_argument('-S1', type=str, help='Basic Davis Putnam')
    parser.add_argument('-S2', type=str, help="DP with h2")
    parser.add_argument('-S3', type=str, help="DP with h3")
    args = parser.parse_args()

    def get_strategy(args):
        for strategy, input_fpath in vars(args):
            if input_fpath:
                return (strategy, input_fpath)

    strategy_config = get_strategy(args)
    main(strategy_config)
