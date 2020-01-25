from city import City
import sys


class Checker:
    def __init__(self, input_file):
        self.city = City(open(input_file).readlines())

    def check(self, filename):
        with open(filename) as f:
            cars = int(f.readline())
            if cars != self.city.cars:
                print('Cars not equal')
                return -1
            result = 0
            visited_streets = set()
            for c in range(cars):
                junctions_num = int(f.readline())
                junctions = [int(f.readline()) for i in range(junctions_num)]
                res = self.check_itinerary(visited_streets, junctions)
                if res < 0:
                    return res
                result += res
            return result

    def check_itinerary(self, visited_streets, junctions):
        start = junctions[0]
        res = 0
        cost = 0
        for end in junctions[1:]:
            street = self.find_street(start, end)
            if not street:
                print(f'Can not find street from {start} to {end}')
                return -1
            cost += street.cost
            if street not in visited_streets:
                res += street.length
                visited_streets.add(street)
        if cost > self.city.seconds:
            print(f'Too large cost for itinerary {junctions}')
            return -1
        return res

    def find_street(self, start, end):
        for street in self.city.streets:
            if (street.a == start and street.b == end) or (
                    street.a == end and street.b == start and street.bidirectional):
                return street
        return None


if __name__ == '__main__':
    checker = Checker(sys.argv[1])
    print(f'Result of Checker({sys.argv[1]}).check({sys.argv[2]}) = {checker.check(sys.argv[2])}')
