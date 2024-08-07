import subprocess
import json
import argparse
import pathlib
import os
import random
import string
import math
import pprint

REPO_ROOT = pathlib.Path(__file__).parent.parent.absolute()
EXECUTABLE_PATH = "build/tests/test_parser"

TOLERANCE = 1e-9


def deep_compare(d1, d2):
    if type(d1) != type(d2):
        return False

    if isinstance(d1, dict) and isinstance(d2, dict):
        if d1.keys() != d2.keys():
            return False
        return all(deep_compare(d1[key], d2[key]) for key in d1)
    elif isinstance(d1, list) and isinstance(d2, list):
        if len(d1) != len(d2):
            return False
        return all(deep_compare(a, b) for a, b in zip(d1, d2))
    elif isinstance(d1, float) and isinstance(d2, float):
        return math.isclose(d1, d2, rel_tol=TOLERANCE)
    else:
        return d1 == d2


def generate_random_json(max_depth: int, max_ele_count: int, max_key_len: int, depth: int = 0):
    rand = random.random()
    if depth > max_depth and rand > 0.4:
        rand -= 0.4
        rand /= 1.5
    if rand > 0.7:  # Object Case
        ret = {}
        ele_count = random.randint(0, max_ele_count)
        for i in range(ele_count):
            key = ''.join(random.choices(string.ascii_letters +
                          string.digits, k=random.randint(1, max_key_len)))
            ret[key] = generate_random_json(
                max_depth, max_ele_count, max_key_len, depth+1)
        return ret
    elif rand > 0.4:  # Array case
        ret = []
        ele_count = random.randint(0, max_ele_count)
        for i in range(ele_count):
            ret.append(generate_random_json(
                max_depth, max_ele_count, max_key_len, depth+1))
        return ret
    elif rand > 0.25:  # String case
        strlen = random.randint(1, 100)
        return ''.join(random.choices(string.ascii_letters + string.digits, k=strlen))
    elif rand > 0.10:  # Number case
        rand = random.random()
        if rand > 0.4:
            return random.randint(-1e15, 1e15)
        elif rand > 0.2:
            return 1/random.uniform(-1e10, 1e10)
        else:
            return random.uniform(-1e10, 1e10)
    elif rand > 0.05:  # Boolean case
        rand = random.random()
        if rand > 0.5:
            return False
        else:
            return True
    else:  # Null case
        return None


def test_parser(random_json):
    json_string = json.dumps(random_json)
    parser = subprocess.run(
        ["./"+EXECUTABLE_PATH, json_string], capture_output=True, text=True)
    dumped_json_string = parser.stdout
    try:
      dumped_json = json.loads(dumped_json_string)
    except:
      print(f"Unable to parse output as JSON\nOutput: {dumped_json_string}")
      exit(1)
    if not deep_compare(random_json, dumped_json):
        print("Matching failure between random json and dumped json")
        print(f"Random JSON: \n{random_json}\n")
        print(f"Dumped JSON: \n{dumped_json}")
        exit(1)


def unsigned_int(arg):
    arg = int(arg)
    if arg <= 0:
        raise argparse.ArgumentError("Value needs to be a positive integer")
    return arg


def parse_args():
    parser = argparse.ArgumentParser(description="JSON parser test")
    parser.add_argument(
        "--cycles", help="Number of JSON objects to test with", default=1000, required=False, type=unsigned_int)
    parser.add_argument(
        "--max-depth", help="Maximum depth of a generated JSON object", default=5, required=False, type=unsigned_int)
    parser.add_argument(
        "--max-ele-count", help="Maximum number of elements of a generated JSON object", default=10, required=False, type=unsigned_int)
    parser.add_argument(
        "--max-key-len", help="Maximum length of key in a generated JSON object", default=10, required=False, type=unsigned_int)
    return parser.parse_args()


def main(cycles: int, max_depth: int, max_ele_count: int, max_key_len: int):
    os.chdir(REPO_ROOT)
    subprocess.run(["make","test"])
    for i in range(cycles):
        random_json = generate_random_json(
            max_depth, max_ele_count, max_key_len)
        test_parser(random_json)
        print(f"Test {i+1} was successful")


if __name__ == '__main__':
    args = parse_args()
    main(args.cycles, args.max_depth, args.max_ele_count, args.max_key_len)
