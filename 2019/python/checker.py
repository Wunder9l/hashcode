from state import State


class Checker:
    def __init__(self, input_file='', lines=None):
        if not lines:
            lines = open(input_file).readlines()
        self.state = State(lines)

    def check_solution(self, filename='', lines=None, debug=True):
        if debug:
            try:
                self.build_state(filename, lines)
            except:
                # print('Incorrect solution')
                return -1
        else:
            self.build_state(filename, lines)

        points = 0
        for file, values_dict in self.state.compiled.items():
            if file in self.state.task_info.targets_dict:
                server, timestamp = min(values_dict.items(), key=lambda x: x[1])
                target = self.state.task_info.targets_dict[file]
                gp = target.goal_points if target.deadline >= timestamp else 0
                sp = max(0, target.deadline - timestamp)
                if debug:
                    print(f'File {file} compiled on server #{server} at {timestamp}, goal points={gp}, speed points={sp}')
                points += gp + sp
        if debug:
            print(f'Total points {points}')
        return points

    def build_state(self, filename, lines):
        # lines_cnt = int(f.readline())
        if not lines:
            lines = open(filename).readlines()[1:]
        for line in lines:
            file, server = line.split()
            self.state.task_info.servers[int(server)].add_task(file, self.state)


if __name__ == '__main__':
    filename = '/home/artem/projects/hashcode/2019/python/final_round_2019.in/a_example.in'
    checker = Checker(filename)
    checker.check_solution(filename='/home/artem/projects/hashcode/2019/python/test', debug=False)