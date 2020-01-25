from itertools import permutations

from checker import Checker
from state import State

filename = '/home/artem/projects/hashcode/2019/python/final_round_2019.in/a_example.in'

input_lines = open(filename).readlines()
state = State(input_lines)


with open('test', 'w') as f:
    files = [file.name for file in state.task_info.files]
    lines = [f'{state.task_info.files_cnt * 2}']
    a_i=0
    for a in permutations(files):
        del lines[1:]
        # print(f'Next a iteration,', a_i)
        a_i+=1
        for cf in a:
            lines.append(f'{cf} {0}')
        for b in permutations(files):
            del lines[state.task_info.files_cnt:]
            for cf in b:
                lines.append(f'{cf} {1}')
            try:
                checker = Checker(lines=input_lines)
                res = checker.check_solution(lines=lines[1:], debug=False)
            except Exception as e:
                pass
                # print ('Exception, ', e)
                # print('\n'.join(lines))
            if res > 60:
                checker = Checker(lines=input_lines)
                res = checker.check_solution(lines=lines[1:], debug=True)
            # break
        # break
    # f.write('\n'.join(lines))
