class FileInfo:
    def __init__(self, lines):
        self.name, self.compile_time, self.replication_time = lines[0].split()
        self.compile_time = int(self.compile_time)
        self.replication_time = int(self.replication_time)
        self.dependencies = lines[1].split()[1:]


class TargetInfo:
    def __init__(self, line):
        self.name, self.deadline, self.goal_points = line.split()
        self.deadline = int(self.deadline)
        self.goal_points = int(self.goal_points)


class Server:
    def __init__(self, i):
        self.cur_time = 0
        self.id = i

    def add_task(self, filename, state):
        compiling_file = state.task_info.files_dict[filename]
        if compiling_file.dependencies:
            dependecy_ready = max(state.get_ready_time(d, self.id) for d in compiling_file.dependencies)
            self.cur_time = max(self.cur_time, dependecy_ready)
        self.cur_time += compiling_file.compile_time
        state.add_compile(filename, self.id, self.cur_time)


class TaskInfo:
    def __init__(self, lines):
        self.files_cnt, self.targets_cnt, self.servers_cnt = map(int, lines[0].split())
        self.files = [FileInfo(lines[1 + 2 * i:3 + 2 * i]) for i in range(self.files_cnt)]
        self.files_dict = {f.name: f for f in self.files}
        self.targets = [TargetInfo(lines[1 + 2 * self.files_cnt + i]) for i in range(self.targets_cnt)]
        self.targets_dict = {t.name: t for t in self.targets}
        self.servers = [Server(i) for i in range(self.servers_cnt)]
