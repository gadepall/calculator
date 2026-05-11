import numpy as np
import mpmath as mp
import matplotlib.pyplot as plt

mp.mp.dps = 50

START = 1.0
END = 5.0

def get_err(est, ref):
    ref = float(ref)
    eps = np.spacing(ref) if ref != 0 else np.nextafter(0.0, 1.0)
    diff = abs(est - ref)
    return 0.0 if diff == 0 else diff / eps

def func(x, y):
    return np.float32(1.0) / np.float32(x)

def eu(f, x, y, h, n):
    x, h, y = np.float32(x), np.float32(h), np.float32(y)
    xs, ys = [x], [y]
    for _ in range(n):
        k1 = np.float32(f(x, y))
        y = np.float32(y + np.float32(h * k1))
        x = np.float32(x + h)
        xs.append(x)
        ys.append(y)
    return np.array(xs), np.array(ys)

def be(f, x, y, h, n):
    x, h, y = np.float32(x), np.float32(h), np.float32(y)
    xs, ys = [x], [y]
    for _ in range(n):
        x_next = np.float32(x + h)
        y = np.float32(y + np.float32(h * np.float32(f(x_next, y))))
        x = x_next
        xs.append(x)
        ys.append(y)
    return np.array(xs), np.array(ys)

def tr(f, x, y, h, n):
    x, h, y = np.float32(x), np.float32(h), np.float32(y)
    half_h = np.float32(h / np.float32(2.0))
    xs, ys = [x], [y]
    for _ in range(n):
        x_next = np.float32(x + h)
        k1 = np.float32(f(x, y))
        k2 = np.float32(f(x_next, y)) # f doesn't depend on y, so we don't need a predictor
        y = np.float32(y + np.float32(half_h * np.float32(k1 + k2)))
        x = x_next
        xs.append(x)
        ys.append(y)
    return np.array(xs), np.array(ys)

def he(f, x, y, h, n):
    x, h, y = np.float32(x), np.float32(h), np.float32(y)
    half_h = np.float32(h / np.float32(2.0))
    xs, ys = [x], [y]
    for _ in range(n):
        k1 = np.float32(f(x, y))
        y_pred = np.float32(y + np.float32(h * k1))
        x_next = np.float32(x + h)
        k2 = np.float32(f(x_next, y_pred))
        
        y = np.float32(y + np.float32(half_h * np.float32(k1 + k2)))
        x = x_next
        xs.append(x)
        ys.append(y)
    return np.array(xs), np.array(ys)

def rk(f, x, y, h, n):
    x, h, y = np.float32(x), np.float32(h), np.float32(y)
    half_h = np.float32(h / np.float32(2.0))
    sixth_h = np.float32(h / np.float32(6.0))
    xs, ys = [x], [y]
    for _ in range(n):
        k1 = np.float32(f(x, y))
        k2 = np.float32(f(np.float32(x + half_h), np.float32(y + np.float32(half_h * k1))))
        k3 = np.float32(f(np.float32(x + half_h), np.float32(y + np.float32(half_h * k2))))
        k4 = np.float32(f(np.float32(x + h), np.float32(y + np.float32(h * k3))))
        
        sum_k = np.float32(k1 + np.float32(2.0 * k2) + np.float32(2.0 * k3) + k4)
        y = np.float32(y + np.float32(sixth_h * sum_k))
        x = np.float32(x + h)
        xs.append(x)
        ys.append(y)
    return np.array(xs), np.array(ys)


x0 = START
y0 = 0.0  # ln(1.0) = 0.0

h_values = [0.5 / (2**i) for i in range(17)]

max_errors = {
    'Forward Euler': [],
    'Backward Euler': [],
    'Trapezoidal': [],
    'Heun': [],
    'RK4': []
}

for h in h_values:
    print(f"On h value: {h}")
    N = int((END - START) / h)
    
    results = {
        'Forward Euler': eu(func, x0, y0, h, N),
        'Backward Euler': be(func, x0, y0, h, N),
        'Trapezoidal': tr(func, x0, y0, h, N),
        'Heun': he(func, x0, y0, h, N),
        'RK4': rk(func, x0, y0, h, N)
    }
    
    for algo_name, (xs, ys) in results.items():
        max_e = 0
        
        for i, x_val in enumerate(xs):
            # Compare against MPFR 50-decimal-place ground truth for ln(x)
            e = get_err(ys[i], mp.log(x_val))
            if e > max_e:
                max_e = e
                
        max_errors[algo_name].append(max_e)

plt.figure(figsize=(10, 7))

styles = {
    'Forward Euler': 'r-o',
    'Backward Euler': 'm-s',
    'Trapezoidal': 'b-^',
    'Heun': 'c-d',
    'RK4': 'g-*'
}

for algo, errors in max_errors.items():
    plt.loglog(h_values, errors, styles[algo], label=algo, markersize=6)

plt.xlabel('Step Size $h$ (Log Scale)')
plt.ylabel('Maximum ULP Error (Log Scale)')
plt.title(r'''Accuracy vs Step Size for $\ln(x)$
$x \in [1.0, 5.0]$ (ATMEGA328P Limits)''')
plt.grid(True, which="both", ls="--", alpha=0.5)
plt.legend()
plt.gca().invert_xaxis()
plt.savefig("../figs/analysis4.png")
plt.show()
