import matplotlib.pyplot as plt

coords = input('Give coords: ')
coords = eval('[{}]'.format(coords))
x, y = zip(*coords)
plt.plot(x, y)
plt.show()