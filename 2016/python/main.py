import matplotlib.pyplot as plt


class Warehouse:
    def __init__(self, x, y, products):
        self.x = x
        self.y = y
        self.products = products


class Order:
    def __init__(self, x, y, products):
        self.x = x
        self.y = y
        self.products = products

    def sum_weight(self, products):
        return sum(products[p] for p in self.products)

def scatter_plot(ax, filename, warehouses, orders):
    orders_color = [len(o.products) for o in orders]
    # print(orders_color)
    ax.scatter([o.x for o in orders], [o.y for o in orders], c=orders_color, s=5)
    warehouses_sizes = [10 + sum(w.products) for w in warehouses]
    ax.scatter([w.x for w in warehouses[1:]], [w.y for w in warehouses[1:]], c='r', s=10)
    ax.scatter([w.x for w in warehouses[:1]], [w.y for w in warehouses[:1]], c='m', s=50)
    # ax.title(f'{filename}: W={len(warehouses)}, O={len(orders)}, P={sum(len(o.products) for o in orders)}')

def plot(filename):
    with open(filename) as f:
        rows, columns, drones, deadline, max_payload = map(int, f.readline().split())
        products_cnt = int(f.readline())
        products = list(map(int, f.readline().split()))
        warehouses_cnt = int(f.readline())
        warehouses = []
        for i in range(warehouses_cnt):
            x, y = map(int, f.readline().split())
            w_p = list(map(int, f.readline().split()))
            warehouses.append(Warehouse(x, y, w_p))
        orders_cnt = int(f.readline())
        orders = []
        for i in range(orders_cnt):
            x, y = map(int, f.readline().split())
            p_cnt = int(f.readline())
            o_p = list(map(int, f.readline().split()))
            orders.append(Order(x, y, o_p))

        products_needed = [0] * products_cnt
        for o in orders:
            for p in o.products:
                products_needed[p] += 1
        for w_i, w in enumerate(warehouses):
            good = True
            for i, needed in enumerate(products_needed):
                if needed > w.products[i]:
                    # print(f'Warehouse {w_i} has {w.products[i]} of products {i} where needed {needed}')
                    good = False
            if good:
                print(f'Warehouse {w_i} is good')

        fig, ax = plt.subplots(3,1, figsize=(8,10))

        plt.title(f'{filename}: {drones}x{max_payload}, W={len(warehouses)}, O={len(orders)}, P={sum(len(o.products) for o in orders)}')
        scatter_plot(ax[0], filename, warehouses, orders)
        ax[1].hist([len(o.products) for o in orders], bins=50)
        ax[2].hist([o.sum_weight(products) for o in orders], bins=50)
        plt.show()


if __name__ == '__main__':
    first  = 'busy_day.in'
    second = 'mother_of_all_warehouses.in'
    third = 'redundancy.in'
    # plt.scatter(range(100), range(100), c=range(100))
    # plt.show()
    plot(first)