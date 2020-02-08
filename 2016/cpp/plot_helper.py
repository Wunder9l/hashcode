def plot_scatter_with_lines(ax, x, y, title):
    ax.plot(x, y, 'C3', zorder=1, lw=3)
    ax.scatter(x, y, s=60, zorder=2)
    ax.set_title(title)


def plot(states):
    import matplotlib.pyplot as plt
    fig, ax = plt.subplots(2, 2)
    energies, temperatures = [s.energy for s in states], [s.t for s in states]
    ax[1, 0].plot(temperatures)
    ax[1, 0].set_title("Temperature's change")
    ax[1, 1].plot(energies)
    ax[1, 1].set_title("Energy's change")

    # plot_scatter_with_lines(ax[0, 0], [c.x for c in states[0].cities], [c.y for c in states[0].cities],
    #                         'First iteration')
    #
    # plot_scatter_with_lines(ax[0, 1], [c.x for c in states[-1].cities], [c.y for c in states[-1].cities],
    #                         'Last iteration')

    plt.show()


class State:
    def __init__(self, line):
        data = line.split()
        self.i = int(data[0])
        self.energy = float(data[1])
        self.t = float(data[2])
        self.orders = list(map(int, data[3:]))


def read_state(filename):
    states = []
    with open(filename) as f:
        for line in f.readlines():
            states.append(State(line))
    return states


if __name__ == '__main__':
    import sys

    # states = read_state('/home/artem/projects/algorithms/annealing_simulation/cpp/cmake-build-debug/out.txt')
    states = read_state(sys.argv[1])
    print()
    plot(states)
