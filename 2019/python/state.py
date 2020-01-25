from models import TaskInfo


class State:
    def __init__(self, lines):
        self.task_info = TaskInfo(lines)
        self.compiled = {}

    def get_ready_time(self, filename, server):
        if filename in self.compiled:
            if server in self.compiled[filename]:
                return self.compiled[filename][server]
            return min(self.compiled[filename].values()) + self.task_info.files_dict[filename].replication_time

    def add_compile(self, filename, server, timestamp):
        if filename in self.compiled:
            self.compiled[filename][server] = timestamp
        else:
            self.compiled[filename] = {server: timestamp}
