class Junction:
    def __init__(self, line):
        self.lat, self.lon = map(float, line.split())


class Street:
    def __init__(self, line):
        self.a, self.b, d, self.cost, self.length = map(int, line.split())
        self.bidirectional = d == 2


class City:
    def __init__(self, lines):
        junctions_num, street_num, self.seconds, self.cars, self.start = map(int, lines[0].split())
        self.junctions = [Junction(line) for line in lines[1:1 + junctions_num]]
        self.streets = [Street(line) for line in lines[1 + junctions_num:1 + junctions_num + street_num]]
