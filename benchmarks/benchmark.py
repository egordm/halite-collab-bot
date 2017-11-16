import shutil
from random import randint
from subprocess import Popen, PIPE
import json
import os

player = r'..\cmake-build-debug\halite_orion.exe'

enemies = {
    'v3': r'..\sandbox\version\v3.exe',
    'v4': r'..\sandbox\version\v4.exe'
}

client = '..\sandbox\halite.exe'
replay_dir = 'replays'

benchmarks = {
    '1v1': {
        'games': 100,
        'agents': 2,
        'width': 'rand',
        'height': 'rand',
        'seed': 'rand',
        'enemy': enemies['v3']
    },
    '1v3': {
        'games': 100,
        'agents': 4,
        'width': 'rand',
        'height': 'rand',
        'seed': 'rand',
        'enemy': enemies['v3']
    }
}

benchmark_results = {

}

if os.path.exists(replay_dir): shutil.rmtree(replay_dir)
os.makedirs(replay_dir, exist_ok=True)


def parse_attribute(attribute, rand):
    if attribute == 'rand': attribute = rand
    if isinstance(attribute, list): return randint(attribute[0], attribute[1])
    return attribute


def create_command(benchmark, enemy, replay_subdir):
    if isinstance(benchmark, str): benchmark = benchmarks[benchmark]
    width = parse_attribute(benchmark['width'], [120, 384])
    height = parse_attribute(benchmark['height'], [120, 256])
    seed = parse_attribute(benchmark['seed'], [0, 100000000])
    rep_dir = '{}\{}'.format(replay_dir, replay_subdir)

    os.makedirs(rep_dir, exist_ok=True)
    rat = '{} -q -d "{} {}" -s {} -i {} "{}"'.format(client, width, height, seed, rep_dir, player)
    for i in range(benchmark['agents'] - 1):
        rat += ' "{}"'.format(enemy)
    return rat


for enemy_name, enemy in enemies.items():
    benchmark_results[enemy_name] = {}
    for name, benchmark in benchmarks.items():
        benchmark_results[enemy_name][name] = {'win': 0, 'loss': 0, 'lost_games': []}
        processes = [Popen(create_command(benchmark, enemy, name), stdout=PIPE, bufsize=1, universal_newlines=True)
                     for i in range(benchmark['games'])]
        print('Benchmarking: {} against {}'.format(name, enemy_name))

        timeout = 60
        while processes:
            for p in processes[:]:
                if p.poll() is None: continue
                result = json.loads(p.stdout.read())
                if result['stats']['0']['rank'] == 1:
                    benchmark_results[enemy_name][name]['win'] += 1
                else:
                    benchmark_results[enemy_name][name]['loss'] += 1
                    benchmark_results[enemy_name][name]['lost_games'].append(result['replay'])
                p.stdout.close()
                processes.remove(p)

print('====== FINISHED BENCHMARKING ======')
for enemy_name, results in benchmark_results.items():
    for name, result in results.items():
        acc = result['win'] / (result['win'] + result['loss']) * 100
        print('==== BENCHMARK: {} AGAINST {} ===='.format(name, enemy_name))
        print('Ran benchmark: "{}" with result: {}/{} overall acc {}'.format(name, result['win'], result['win'] + result['loss'], acc))
        print('=== LOST GAMES ===')
        for replay in result['lost_games']:
            print(replay)
