import numpy as np
import mpmath as mp
import pandas as pd
import matplotlib.pyplot as plt

mp.mp.dps = 50
N = 100
END = 6.3

def get_err(est, ref):
    ref = float(ref)
    eps = np.spacing(ref) if ref != 0 else np.nextafter(0.0, 1.0)
    diff = abs(est - ref)
    return 0.0 if diff == 0 else diff / eps

def eu(f, t, y, h, n):
    t, y = t, np.array(y, dtype=np.float64)
    ts, ys = [t], [y]
    for _ in range(n):
        y = y + h * f(t, y)
        t += h
        ts.append(t)
        ys.append(y)
    return np.array(ts), np.array(ys)

def he(f, t, y, h, n):
    t, y = t, np.array(y, dtype=np.float64)
    ts, ys = [t], [y]
    for _ in range(n):
        k1 = f(t, y)
        k2 = f(t + h, y + h * k1)
        y = y + (h / 2.0) * (k1 + k2)
        t += h
        ts.append(t)
        ys.append(y)
    return np.array(ts), np.array(ys)

def rk(f, t, y, h, n):
    t, y = t, np.array(y, dtype=np.float64)
    ts, ys = [t], [y]
    for _ in range(n):
        k1 = f(t, y)
        k2 = f(t + h/2, y + h*k1/2)
        k3 = f(t + h/2, y + h*k2/2)
        k4 = f(t + h, y + h*k3)
        y = y + (h / 6.0) * (k1 + 2*k2 + 2*k3 + k4)
        t += h
        ts.append(t)
        ys.append(y)
    return np.array(ts), np.array(ys)

def be(A, t, y, h, n):
    I = np.eye(len(y))
    M = np.linalg.inv(I - h * A)
    t, y = t, np.array(y, dtype=np.float64)
    ts, ys = [t], [y]
    for _ in range(n):
        y = M @ y
        t += h
        ts.append(t)
        ys.append(y)
    return np.array(ts), np.array(ys)

def tr(A, t, y, h, n):
    I = np.eye(len(y))
    M = np.linalg.inv(I - h/2 * A) @ (I + h/2 * A)
    t, y = t, np.array(y, dtype=np.float64)
    ts, ys = [t], [y]
    for _ in range(n):
        y = M @ y
        t += h
        ts.append(t)
        ys.append(y)
    return np.array(ts), np.array(ys)

def bi(t, y, h, n):
    den = 4.0 + h**2
    c1 = (8.0 - 2.0 * h**2) / den
    ts, vals = [t], np.zeros((n + 1, 2))
    vals[0, 0] = y[0]
    t_next = t + h
    vals[1, 0] = float(mp.sin(t_next))
    ts.append(t_next)
    
    for i in range(2, n + 1):
        t_now = t + i * h
        vals[i, 0] = c1 * vals[i-1, 0] - vals[i-2, 0]
        ts.append(t_now)
    return np.array(ts), vals

A = np.array([[0.0, 1.0], [-1.0, 0.0]])
def func(t, y): return A @ y

t0, y0 = 0.0, [0.0, 1.0]
h = (END - t0) / N

print(f"h={h:.5f}")
data_stats = []
res = {
    'Forward Euler': eu(func, t0, y0, h, N),
    'Backward Euler': be(A, t0, y0, h, N),
    'Trapezoidal': tr(A, t0, y0, h, N),
    'Heun': he(func, t0, y0, h, N),
    'RK4': rk(func, t0, y0, h, N),
    'Diff Eq (Bilinear)': bi(t0, y0, h, N)
}

plt.figure(figsize=(12, 7))
cols = {
    'Forward Euler': 'r:',
    'Backward Euler': 'm-.',
    'Trapezoidal': 'b--',
    'Heun': 'c--',
    'RK4': 'g-',
    'Diff Eq (Bilinear)': 'k.-'
}

for k, (ts, ys) in res.items():
    errs = []
    sin_v = ys[:, 0]
    max_e = 0
    sse = 0
    for i, t in enumerate(ts):
        e = get_err(sin_v[i], mp.sin(t))
        errs.append(e)
        max_e = max(max_e, e)
        sse += (sin_v[i] - float(mp.sin(t)))**2

    rmse = np.sqrt(sse / len(ts))
    data_stats.append({
        'Algorithm': k,
        'Max Error (ULP)': max_e,
        'RMS Error': rmse
    })
    lw = 2.5 if k == 'Diff Eq (Bilinear)' else 1.5
    plt.plot(ts, errs, cols.get(k, '-'), label=k, lw=lw, alpha=0.8)

plt.yscale('log')
plt.title(f'Accuracy Comparison: ODE Solvers vs Difference Equation\n(Step size h={h:.4f})', fontsize=14)
plt.xlabel('Input (x)', fontsize=12)
plt.ylabel('Error (ULP) - Log Scale', fontsize=12)
plt.grid(True, which="both", ls="-", alpha=0.2)
plt.legend(fontsize=10)
plt.tight_layout()

plt.savefig('../figs/analysis.png', dpi=300)
print("Plot saved")
df = pd.DataFrame(data_stats).sort_values('Max Error (ULP)')
print("\nAccuracy Statistics:")
print(df.to_string(index=False))

plt.show()
