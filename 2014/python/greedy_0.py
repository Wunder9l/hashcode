from city import City, Junction


class MinPathFromStart:
    MAX_LENGTH = int(1e9)

    def __init__(self, length, cost, street, visited=False):
        self.length = length
        self.cost = cost
        self.street = street
        self.visited = visited


def dijkstra_select_next(junctions):
    i = -1
    junction = None
    cost = MinPathFromStart.MAX_LENGTH
    for j, k in enumerate(junctions):
        if not k.min_path_from_start.visited and cost > k.min_path_from_start.cost:
            junction = k
            i = j
    return i, junction


class Greedy0:
    def __init__(self, input_file):
        self.city = City(open(input_file).readlines())

    def solve(self, output_file):
        self.update_city()
        with open(output_file, 'w') as f:
            f.write(f'{self.city.cars}\n')
            for c in self.city.cars:
                itinerary = self.make_itinerary()
                f.write('\n'.join(str(x) for x in itinerary))

    def can_be_added(self, junction: Junction, cost):
        return junction.min_path_from_start.cost + cost < self.city.seconds

    def add_to_path(self, itinerary, from_j, street):
        from_j.min_path_from.length -= street.length
        street.length = 0
        index = street.a if self.city.junctions[street.a] == from_j else street.b
        itinerary.append(index)
        return street.cost

    def choose_finish_junction(self, itinerary):
        sum_cost = 0
        streets = sorted(self.city.streets, key=lambda s: s.length, reverse=True)
        next_j = None
        for s in streets:
            if self.can_be_added(s.a, s.cost):
                if self.can_be_added(s.b, s.cost):
                    next_j = max(s.a, s.b, key=lambda
                        x: x.min_cost_from_start.length if x.min_cost_from_start.street != s else x.min_cost_from_start.length - s.length)
                    start = self.city.junctions[s.a if s.a != next_j else s.b]
                    next_j = self.city.junctions[next_j]
                else:
                    start = self.city.junctions[s.b]
                    next_j = self.city.junctions[s.a]
                sum_cost = self.add_to_path(itinerary, start, s)
                break
            elif self.can_be_added(s.b, s.cost):
                start = self.city.junctions[s.a]
                next_j = self.city.junctions[s.b]
                sum_cost = self.add_to_path(itinerary, start, s)
                break
        return sum_cost, next_j

    def make_itinerary(self):
        itinerary = []
        sum_cost, next_j = self.choose_finish_junction(itinerary)
        return reversed(itinerary)

    def update_city(self):
        for each in self.city.junctions:
            each.fr = []
        for s in self.city.streets:
            self.city.junctions[s.a].fr.append(s)
            if s.bidirectional:
                self.city.junctions[s.b].fr.append(s)
        self.run_dijkstra()

    def run_dijkstra(self):
        for each in self.city.junctions:
            each.min_path_from_start = MinPathFromStart(0, MinPathFromStart.MAX_LENGTH, None)
        self.city.junctions[self.city.start].min_path_from_start = MinPathFromStart(0, 0, None, True)
        for v in range(len(self.city.junctions)):
            i, start = dijkstra_select_next(self.city.junctions)
            start.min_path_from_start.visited = True
            for street in start.fr:
                to = street.b if street.b != i else street.a
                if self.city.junctions[to].min_path_from_start.cost > start.min_path_from_start.cost + street.cost:
                    self.city.junctions[to].min_path_from_start = MinPathFromStart(
                        start.min_path_from_start.length + street.length,
                        start.min_path_from_start.cost + street.cost,
                        street)
